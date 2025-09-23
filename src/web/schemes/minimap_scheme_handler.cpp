#include "minimap_scheme_handler.h"

#include "spdlog/spdlog.h"
#include "utils/di.h"

namespace wow::web::schemes {
    bool minimap_scheme_handler_factory::resource_handler::Open(CefRefPtr<CefRequest> request, bool &handle_request,
                                                                CefRefPtr<CefCallback> callback) {
        static std::string prefix = "minimap://localhost/";

        std::thread{
            [this, request, callback] {
                const auto actual_path = request->GetURL().ToString().substr(prefix.length());
                std::stringstream ss;
                ss << actual_path;
                std::string map_id{}, zoom_level{}, tx{}, ty{};

                std::getline(ss, map_id, '/');
                std::getline(ss, zoom_level, '/');
                std::getline(ss, tx, '/');
                std::getline(ss, ty);

                try {
                    utils::app_module->minimap_provider()->read_image(_data, std::stoi(map_id), std::stoi(zoom_level),
                                                                      std::stoi(tx), std::stoi(ty));
                } catch (std::exception &) {
                    _data.clear();
                }

                callback->Continue();
            }
        }.detach();

        return true;
    }

    void minimap_scheme_handler_factory::resource_handler::GetResponseHeaders(CefRefPtr<CefResponse> response,
                                                                              int64_t &response_length,
                                                                              CefString &redirectUrl) {
        if (_data.empty()) {
            response->SetStatus(404);
            return;
        }

        response->SetStatus(200);
        response->SetMimeType("image/png");
        response_length = _data.size();
    }

    bool minimap_scheme_handler_factory::resource_handler::Skip(int64_t bytes_to_skip, int64_t &bytes_skipped,
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

    bool minimap_scheme_handler_factory::resource_handler::Read(void *data_out, int bytes_to_read, int &bytes_read,
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

    void minimap_scheme_handler_factory::resource_handler::Cancel() {
    }
}
