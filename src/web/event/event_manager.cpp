#include "event_manager.h"

namespace wow::web::event {
    event_manager::event_manager() {
        EMPTY_RESPONSE.type = js_event_type::empty_response;
    }

    event_manager &event_manager::listen(const js_event_type &event,
                                         const std::function<js_event (const js_event &)> &callback) {
        std::lock_guard lock(_callback_lock);
        _callbacks[event].push_back(callback);
        return *this;
    }

    std::unique_ptr<js_event> event_manager::dispatch(const js_event &event) {
        std::list<std::function<js_event (const js_event &)> > callbacks;
        {
            std::lock_guard lock(_callback_lock);
            if (const auto itr = _callbacks.find(event.type); itr != _callbacks.end()) {
                callbacks = itr->second;
            }
        }

        js_event result{};
        for (const auto &callback: callbacks) {
            result = callback(event);
        }

        return callbacks.empty() ? nullptr : std::make_unique<js_event>(result);
    }

    void event_manager::submit(const js_event &event) const {
        if (!_event_callback) {
            return;
        }

        _event_callback(event);
    }
}
