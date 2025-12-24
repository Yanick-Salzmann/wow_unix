#ifndef WOW_UNIX_UI_EVENT_SYSTEM_H
#define WOW_UNIX_UI_EVENT_SYSTEM_H

#include <memory>

#include "event_manager.h"
#include "io/dbc/dbc_manager.h"
#include "scene/world_frame.h"


namespace wow::web::event {
    class ui_event_system {
        io::dbc::dbc_manager_ptr _dbc_manager{};
        scene::world_frame_ptr _world_frame{};
        event_manager_ptr _event_manager{};

        proto::ListMapsResponse handle_list_maps() const;

        proto::ListMapPoisResponse handle_list_map_pois(int32_t map_id) const;

    public:
        explicit ui_event_system(
            const event_manager_ptr &event_manager,
            io::dbc::dbc_manager_ptr dbc_manager,
            scene::world_frame_ptr world_frame
        );

        const event_manager_ptr& event_manager() const {
            return _event_manager;
        }
    };

    using ui_event_system_ptr = std::shared_ptr<ui_event_system>;
}

#endif //WOW_UNIX_UI_EVENT_SYSTEM_H
