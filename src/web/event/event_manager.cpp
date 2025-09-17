#include "event_manager.h"

namespace wow::web::event {
    event_manager &event_manager::listen(const proto::JsEvent::EventCase &event,
                                         std::function<proto::JsEvent(const proto::JsEvent &)> callback) {
        std::lock_guard lock(_callback_lock);
        _callbacks[event].push_back(callback);
        return *this;
    }

    std::unique_ptr<proto::JsEvent> event_manager::dispatch(const proto::JsEvent &event) {
        std::list<std::function<proto::JsEvent(const proto::JsEvent &)> > callbacks;
        {
            std::lock_guard lock(_callback_lock);
            if (const auto itr = _callbacks.find(event.event_case()); itr != _callbacks.end()) {
                callbacks = itr->second;
            }
        }

        proto::JsEvent result{};
        for (const auto &callback: callbacks) {
            result = callback(event);
        }

        return callbacks.empty() ? nullptr : std::make_unique<proto::JsEvent>(result);
    }
}
