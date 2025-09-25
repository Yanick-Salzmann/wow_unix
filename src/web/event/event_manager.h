#ifndef WOW_UNIX_EVENT_MANAGER_H
#define WOW_UNIX_EVENT_MANAGER_H

#include <list>
#include "js_event.pb.h"

namespace wow::web::event {
    typedef std::function<void(const proto::JsEvent &)> event_callback;

    class event_manager {
        proto::JsEvent EMPTY_RESPONSE{};

        event_callback _event_callback{};

        std::mutex _callback_lock{};
        std::map<proto::JsEvent::EventCase, std::list<std::function<proto::JsEvent(const proto::JsEvent &)> > >
        _callbacks{};

    public:
        event_manager();

        event_manager &listen(const proto::JsEvent::EventCase &event,
                              std::function<proto::JsEvent(const proto::JsEvent &)> callback);

        std::unique_ptr<proto::JsEvent> dispatch(const proto::JsEvent &event);

        void submit(const proto::JsEvent &event) const;

        const proto::JsEvent &empty_response() const {
            return EMPTY_RESPONSE;
        }

        void set_event_callback(const event_callback &callback) {
            _event_callback = callback;
        }
    };

    using event_manager_ptr = std::shared_ptr<event_manager>;
}

#endif //WOW_UNIX_EVENT_MANAGER_H
