#ifndef WOW_UNIX_DBC_STRUCTS_H
#define WOW_UNIX_DBC_STRUCTS_H

#include <cstdint>
#include <string>

namespace wow::io::dbc {
#pragma pack(push, 1)

    struct loc_string {
        std::string text;
    };

    enum class map_instance {
        none,
        party,
        raid,
        pvp,
        arena
    };

    struct map_record {
        int32_t id;
        std::string directory;
        map_instance instance_type;
        uint32_t flags;
        loc_string name;
        int32_t whatever;
        int32_t area_table;
        loc_string desc_alliance;
        loc_string desc_horde;
        int32_t loading_screen;
        float minimap_icon_scale;
        int32_t corpse_map_id;
        float corpse_x;
        float corpse_y;
        int32_t time_of_day_override;
        int32_t expansion_id;
        int32_t raid_offset;
        int32_t num_players;
    };

    struct loading_screen_record {
        int32_t id;
        std::string name;
        std::string path;
        bool has_wide_screen;
    };

#pragma pack(pop)
}

#endif //WOW_UNIX_DBC_STRUCTS_H
