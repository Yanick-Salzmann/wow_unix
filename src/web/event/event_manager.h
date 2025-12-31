#ifndef WOW_UNIX_EVENT_MANAGER_H
#define WOW_UNIX_EVENT_MANAGER_H

#include <list>
#include <mutex>
#include <functional>
#include <map>
#include "js_event.h"

namespace wow::web::event {
    typedef std::function<void(const js_event &)> event_callback;

    class event_manager {
        js_event EMPTY_RESPONSE{};

        event_callback _event_callback{};

        std::mutex _callback_lock{};
        std::map<js_event_type, std::list<std::function<js_event (const js_event &)> > >
        _callbacks{};

    public:
        event_manager();

        event_manager &listen(const js_event_type &event,
                              const std::function<js_event (const js_event &)> &callback);

        std::unique_ptr<js_event> dispatch(const js_event &event);

        void submit(const js_event &event) const;

        const js_event &empty_response() const {
            return EMPTY_RESPONSE;
        }

        void set_event_callback(const event_callback &callback) {
            _event_callback = callback;
        }
    };

    using event_manager_ptr = std::shared_ptr<event_manager>;
}

#endif //WOW_UNIX_EVENT_MANAGER_H
