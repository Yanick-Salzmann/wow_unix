#include "event_manager.h"

namespace wow::web::event {
    event_manager & event_manager::listen(const proto::JsEvent::EventCase &event,
        std::function<proto::JsEvent(const proto::JsEvent &)> callback) {
        std::lock_guard lock(_callback_lock);
        _callbacks[event].push_back(callback);
        return *this;
    }
}
