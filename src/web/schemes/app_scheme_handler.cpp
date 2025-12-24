#include "app_scheme_handler.h"
#include <filesystem>
#include "spdlog/spdlog.h"
#include "utils/string_utils.h"

namespace wow::web::schemes {
    void process_mime_types(std::map<std::string, std::string> &mime_types) {
        std::ifstream file{"mime_types.txt"};
        std::string line{};
        while (getline(file, line)) {
            const auto pos = line.find(':');
            const auto extension = utils::trim(line.substr(0, pos));
            const auto mime_type = utils::trim(line.substr(pos + 1));
            mime_types[extension] = mime_type;
        }
    }

    bool app_scheme_handler_factory::resource_handler::Open(const CefRefPtr<CefRequest> request, bool &handle_request,
                                                            CefRefPtr<CefCallback> callback) {
        static const std::string prefix = "app://localhost/";
        static const auto prefix_len = prefix.length();
        static std::once_flag once;
        static std::map<std::string, std::string> mime_map{};

        std::call_once(once, [] {
            process_mime_types(mime_map);
        });

        const auto url = request->GetURL().ToString();
        const auto method = request->GetMethod().ToString();
        SPDLOG_INFO("{} {}", method, url);

        const auto path = absolute(std::filesystem::path{"ui"} / url.substr(prefix_len)).string();
        SPDLOG_INFO("{} -> {}", url, path);

        _file = std::ifstream(path, std::ios::in | std::ios::binary);
        if (_file) {
            auto ext = std::filesystem::path{path}.extension().string();
            if (utils::starts_with(ext, ".")) {
                ext = ext.substr(1);
            }

            _mime_type = mime_map[ext];
        }
        handle_request = true;
        return true;
    }

    void app_scheme_handler_factory::resource_handler::GetResponseHeaders(const CefRefPtr<CefResponse> response,
                                                                          int64_t &response_length,
                                                                          CefString &redirectUrl) {
        if (!_file.is_open()) {
            response->SetStatus(404);
            return;
        }

        response->SetStatus(200);
        response->SetMimeType(_mime_type);

        _file.seekg(0, std::ios::end);
        response_length = _file.tellg();
        _full_size = response_length;

        _file.seekg(0, std::ios::beg);
    }

    bool app_scheme_handler_factory::resource_handler::Skip(int64_t bytes_to_skip, int64_t &bytes_skipped,
                                                            CefRefPtr<CefResourceSkipCallback> callback) {
        const auto available = _full_size - _file.tellg();
        bytes_to_skip = std::min(available, bytes_to_skip);

        _file.seekg(bytes_to_skip, std::ios::cur);
        bytes_skipped = bytes_to_skip;
        return true;
    }

    bool app_scheme_handler_factory::resource_handler::Read(void *data_out, int bytes_to_read, int &bytes_read,
                                                            CefRefPtr<CefResourceReadCallback> callback) {
        const auto available = _full_size - _file.tellg();
        bytes_to_read = static_cast<int>(std::min(available, static_cast<int64_t>(bytes_to_read)));

        _file.read(static_cast<char *>(data_out), bytes_to_read);
        bytes_read = bytes_to_read;
        return bytes_read > 0;
    }

    void app_scheme_handler_factory::resource_handler::Cancel() {
    }
}
