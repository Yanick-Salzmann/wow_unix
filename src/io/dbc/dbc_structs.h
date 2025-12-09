#ifndef WOW_UNIX_DBC_STRUCTS_H
#define WOW_UNIX_DBC_STRUCTS_H

#include <cstdint>
#include <string>
#include <array>

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

    struct area_poi_record {
        int32_t id;
        int32_t importance;
        int32_t normal_icon;
        int32_t normal_icon_50;
        int32_t normal_icon_0;
        int32_t horde_icon;
        int32_t horde_icon_50;
        int32_t horde_icon_0;
        int32_t alliance_icon;
        int32_t alliance_icon_50;
        int32_t alliance_icon_0;
        int32_t faction;
        float x, y, z;
        int32_t map_id;
        int32_t flags;
        int32_t area_table;
        loc_string name;
        loc_string description;
        int32_t world_state;
        int32_t map_link;
    };

    struct area_table_record {
        int32_t id;
        int32_t map_id;
        int32_t parent_id;
        int32_t area_bit;
        int32_t flags;
        int32_t sound_provider;
        int32_t underwater_sound_provider;
        int32_t sound_ambience;
        int32_t zone_music;
        int32_t zone_intro_music;
        int32_t level;
        loc_string name;
        int32_t faction;
        int32_t liquid_Type;
        float min_elevation;
        float ambient_multiplier;
        int32_t light;
    };

    struct light_record {
        int32_t id;
        int32_t map_id;
        float x;
        float y;
        float z;
        float falloff_start;
        float falloff_end;
        int32_t params_clear;
        int32_t params_clear_wat;
        int32_t params_storm;
        int32_t params_storm_wat;
        int32_t params_death;
        int32_t params_unk1;
        int32_t params_unk2;
        int32_t params_unk3;
    };

    struct light_params_record {
        int32_t id;
        int32_t highlight_sky;
        int32_t light_skybox_id;
        float glow;
        float water_shallow_alpha;
        float water_deep_alpha;
        float ocean_shallow_alpha;
        float ocean_deep_alpha;
        int32_t flags;
    };

    struct light_skybox_record {
        int32_t id;
        std::string name;
        int32_t flags;
    };

    struct light_int_band_record {
        int32_t id;
        int32_t num_entries;
        std::array<int32_t, 16> times;
        std::array<int32_t, 16> colors;
    };

    struct light_float_band_record {
        int32_t id;
        int32_t num_entries;
        std::array<int32_t, 16> times;
        std::array<float, 16> values;
    };

#pragma pack(pop)
}

#endif //WOW_UNIX_DBC_STRUCTS_H
