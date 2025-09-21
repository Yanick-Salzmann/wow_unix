#ifndef WOW_UNIX_UI_EVENT_SYSTEM_H
#define WOW_UNIX_UI_EVENT_SYSTEM_H

#include <memory>

#include "event_manager.h"
#include "io/dbc/dbc_manager.h"


namespace wow::web::event {
    class ui_event_system {
        io::dbc::dbc_manager_ptr _dbc_manager{};

        proto::ListMapsResponse handle_list_maps() const;

    public:
        explicit ui_event_system(const event_manager_ptr &event_manager, const io::dbc::dbc_manager_ptr &dbc_manager);
    };

    typedef std::shared_ptr<ui_event_system> ui_event_system_ptr;
}

#endif //WOW_UNIX_UI_EVENT_SYSTEM_H
