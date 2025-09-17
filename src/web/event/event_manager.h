#ifndef WOW_UNIX_EVENT_MANAGER_H
#define WOW_UNIX_EVENT_MANAGER_H

#include "js_event.pb.h"

namespace wow::web::event {
    class event_manager {
        std::mutex _callback_lock{};
        std::map<proto::JsEvent::EventCase, std::list<std::function<proto::JsEvent(const proto::JsEvent&)>>> _callbacks{};

    public:
        event_manager& listen(const proto::JsEvent::EventCase &event, std::function<proto::JsEvent(const proto::JsEvent&)> callback);
        std::unique_ptr<proto::JsEvent> dispatch(const proto::JsEvent &event);
    };

    typedef std::shared_ptr<event_manager> event_manager_ptr;
}

#endif //WOW_UNIX_EVENT_MANAGER_H