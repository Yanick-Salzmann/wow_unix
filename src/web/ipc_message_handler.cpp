#include "ipc_message_handler.h"

#include "spdlog/spdlog.h"

namespace wow::web {
    void ipc_message_handler::submit_js_event(const event::js_event &event) const {
        if (!_callback) {
            return;
        }

        if (event.type == event::js_event_type::none) {
            SPDLOG_ERROR("Received event without any event type");
            throw std::runtime_error{"Received event without any event type"};
        }

        std::string serialized = nlohmann::json(event).dump();
        _callback->Success(serialized);
    }

    bool ipc_message_handler::OnQuery(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, int64_t query_id,
                                      const CefString &request, bool persistent, CefRefPtr<Callback> callback) {
        const auto json_doc = nlohmann::json::parse(request.ToString());
        const auto event = json_doc.get<event::js_event>();


        switch (event.type) {
            case event::js_event_type::initialize_request: {
                SPDLOG_INFO("Received initialize request");
                _callback = callback;
                _event_manager->set_event_callback([this](const event::js_event &e) {
                    submit_js_event(e);
                });

                break;
            }

            default: {
                std::thread{
                    [event, this, callback] {
                        if (const auto response = _event_manager->dispatch(event)) {
                            const std::string serialized = nlohmann::json(*response).dump();
                            callback->Success(serialized);
                        }
                    }
                }.detach();
                break;
            }
        }

        return true;
    }
}
