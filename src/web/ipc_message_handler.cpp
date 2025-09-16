#include "ipc_message_handler.h"
#include <google/protobuf/util/json_util.h>

#include "spdlog/spdlog.h"
#include "utils/dialog_utils.h"

namespace wow::web {
    bool ipc_message_handler::OnQuery(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, int64_t query_id,
                                      const CefString &request, bool persistent, CefRefPtr<Callback> callback) {
        proto::JsEvent event{};
        if (const auto status = google::protobuf::util::JsonStringToMessage(request.ToString(), &event); !status.ok()) {
            spdlog::error("Failed to parse json: {}", status.message());
            throw std::runtime_error("Failed to parse json");
        }

        switch (event.event_case()) {
            case proto::JsEvent::kBrowseFolderRequest: {
                auto t = std::thread{
                    [callback]() {
                        proto::JsEvent response{};
                        proto::BrowseFolderResponse* data = response.mutable_browse_folder_response();
                        data->set_canceled(true);

                        std::string path;
                        if (utils::browse_folder_dialog("Select Folder", path)) {
                            data->set_path(path);
                            data->set_canceled(false);
                        }

                        callback->Success(response.SerializeAsString());
                    }
                };
                t.detach();
                break;
            }

            case proto::JsEvent::kInitializeRequest:
                spdlog::info("Received initialize request");
                _callback = callback;
                _callback->Success("{'status': 'ok'}");
                _callback->Success("{'status': 'ok2'}");
                break;

            default:
                spdlog::warn("Unhandled event: {}", event.DebugString());
                break;
        }

        return true;
    }
}
