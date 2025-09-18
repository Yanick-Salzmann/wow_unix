#include "ipc_message_handler.h"
#include <google/protobuf/util/json_util.h>

#include "spdlog/spdlog.h"
#include "utils/dialog_utils.h"

namespace wow::web {
    void ipc_message_handler::submit_js_event(const proto::JsEvent &event) const {
        if (!_callback) {
            return;
        }

        std::string serialized{};
        if (const auto status = google::protobuf::util::MessageToJsonString(event, &serialized); !status.ok()) {
            SPDLOG_ERROR("Failed to serialize response: {}", status.message());
            _callback->Failure(0x7F001001, "Failed to serialize response");
        } else {
            _callback->Success(serialized);
        }
    }

    bool ipc_message_handler::OnQuery(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, int64_t query_id,
                                      const CefString &request, bool persistent, CefRefPtr<Callback> callback) {
        proto::JsEvent event{};
        if (const auto status = google::protobuf::util::JsonStringToMessage(request.ToString(), &event); !status.ok()) {
            SPDLOG_ERROR("Failed to parse json: {}", status.message());
            throw std::runtime_error("Failed to parse json");
        }

        switch (event.event_case()) {
            case proto::JsEvent::kInitializeRequest: {
                SPDLOG_INFO("Received initialize request");
                _callback = callback;
                _event_manager->set_event_callback([this](const proto::JsEvent &e) {
                    submit_js_event(e);
                });

                break;
            }

            default: {
                std::thread{
                    [event, this, callback] {
                        if (const auto response = _event_manager->dispatch(event)) {
                            std::string serialized{};
                            if (const auto status = google::protobuf::util::MessageToJsonString(*response, &serialized);
                                !status.ok()) {
                                SPDLOG_ERROR("Failed to serialize response: {}", status.message());
                                callback->Failure(0x7F001001, "Failed to serialize response");
                            } else {
                                callback->Success(serialized);
                            }
                        }
                    }
                }.detach();
                break;
            }
        }

        return true;
    }
}
