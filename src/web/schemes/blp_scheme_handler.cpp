#include "blp_scheme_handler.h"

#include "io/blp/blp_file.h"
#include "utils/di.h"
#include "utils/string_utils.h"

namespace wow::web::schemes {
    bool blp_scheme_handler_factory::resource_handler::Open(CefRefPtr<CefRequest> request, bool &handle_request,
                                                            CefRefPtr<CefCallback> callback) {
        static const std::string prefix = "blp://localhost/";

        std::thread{
            [this, request, callback] {
                const auto mpq_manager = utils::app_module->mpq_manager();
                const auto path = request->GetURL().ToString();
                const auto file_name = path.substr(prefix.length());
                const auto file = mpq_manager->open(utils::replace_all(file_name, "/", "\\"));
                if (!file) {
                    callback->Continue();
                    return;
                }

                const io::blp::blp_file blp_file{file};
                _data = blp_file.convert_to_png();
                _found = true;

                callback->Continue();
            }
        }.detach();

        handle_request = false;
        return true;
    }

    void blp_scheme_handler_factory::resource_handler::GetResponseHeaders(CefRefPtr<CefResponse> response,
                                                                          int64_t &response_length,
                                                                          CefString &redirectUrl) {
        if (!_found) {
            response->SetStatus(404);
            response_length = 0;
            return;
        }

        response_length = _data.size();
        response->SetMimeType("image/png");
        response->SetStatus(200);
    }

    bool blp_scheme_handler_factory::resource_handler::Skip(int64_t bytes_to_skip, int64_t &bytes_skipped,
                                                            CefRefPtr<CefResourceSkipCallback> callback) {
        if (_offset >= _data.size()) {
            bytes_skipped = 0;
            return true;
        }

        auto total = _data.size();
        total -= _offset;
        if (bytes_to_skip > total) {
            bytes_to_skip = total;
        }

        _offset += bytes_to_skip;
        bytes_skipped = bytes_to_skip;
        return true;
    }

    bool blp_scheme_handler_factory::resource_handler::Read(void *data_out, int bytes_to_read, int &bytes_read,
                                                            CefRefPtr<CefResourceReadCallback> callback) {
        if (_offset >= _data.size()) {
            bytes_read = 0;
            return false;
        }

        auto total = _data.size();
        total -= _offset;
        if (bytes_to_read > total) {
            bytes_to_read = total;
        }

        memcpy(data_out, _data.data() + _offset, bytes_to_read);
        _offset += bytes_to_read;
        bytes_read = bytes_to_read;
        return bytes_read > 0;
    }

    void blp_scheme_handler_factory::resource_handler::Cancel() {
    }
}
