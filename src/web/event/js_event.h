#ifndef GLAD_JS_EVENT_H
#define GLAD_JS_EVENT_H
#include <string>
#include <vector>

#include <nlohmann/json.hpp>
#include <boost/pfr.hpp>

namespace nlohmann {
    template <typename T>
    requires std::is_aggregate_v<T>
    struct adl_serializer<T> {
        static void from_json(const json& j, T& t) {
            boost::pfr::for_each_field(t, [&j]<std::size_t I>(auto& field, std::integral_constant<std::size_t, I>) mutable {
                auto name = boost::pfr::get_name<I, T>();
                if (auto it = j.find(name); it != j.end()) {
                    it->get_to(field);
                }
            });
        }

        static void to_json(json& j, const T& t) {
            boost::pfr::for_each_field(t, [&j]<std::size_t I>(const auto& field,  std::integral_constant<std::size_t, I>) mutable {
                j[boost::pfr::get_name<I, T>()] = field;
            });
        }
    };
}

namespace wow::web::event {
    enum class js_event_type {
        none = 0,
        initialize_request = 1,
        browse_folder_request,
        browse_folder_response,
        empty_response,
        load_data_event,
        load_update_event,
        list_maps_request,
        list_maps_response,
        list_map_pois_request,
        list_map_pois_response,
        enter_world_request,
        loading_screen_show_event,
        loading_screen_progress_event,
        loading_screen_complete_event,
        area_update_event,
        world_position_update_event,
        fps_update_event,
        system_update_event,
        fetch_game_time_request,
        fetch_game_time_response,
        sound_update_event
    };

    struct initialize_request {
    };

    struct browse_folder_request {
        std::string title{};
        std::string default_path{};
        std::vector<std::string> filters{};
        bool allow_create = false;
    };

    struct browse_folder_response {
        bool cancelled = false;
        std::string path{};
    };

    struct empty_response {
    };

    struct load_data_event {
        std::string folder{};
    };

    struct load_update_event {
        int32_t percentage = 0;
        bool completed = false;
        std::string message{};
    };

    struct list_maps_request {
    };

    struct list_maps_response_map {
        int32_t map_id = 0;
        std::string name{};
        std::string loading_screen{};
    };

    struct list_maps_response {
        std::vector<list_maps_response_map> maps{};
    };

    struct list_map_pois_request {
        int32_t map_id = 0;
    };

    struct map_poi {
        int32_t id = 0;
        std::string name{};
        float x = 0.0f;
        float y = 0.0f;
    };

    struct list_map_pois_response {
        int32_t map_id = 0;
        std::vector<map_poi> pois{};
    };

    struct enter_world_request {
        int32_t map_id = 0;
        float x = 0.0f;
        float y = 0.0f;
    };

    struct loading_screen_show_event {
        std::string image_path{};
    };

    struct loading_screen_progress_event {
        float percentage = 0.0f;
    };

    struct loading_screen_complete_event {
    };

    struct area_update_event {
        int32_t area_id = 0;
        std::string area_name{};
    };

    struct world_position_update_event {
        int32_t map_id = 0;
        std::string map_name{};
        float x = 0.0f;
        float y = 0.0f;
        float z = 0.0f;
    };

    struct fps_update_event {
        float fps = 0.0f;
        int64_t time_of_day = 0;
    };

    struct system_update_event {
        int64_t memory_usage = 0;
        int32_t cpu_usage = 0;
        int32_t gpu_usage = 0;
        int64_t total_memory = 0;
        int32_t cpu_frequency_mhz = 0;
        int64_t gpu_memory_used = 0;
        int64_t gpu_memory_total = 0;
    };

    struct fetch_game_time_request {
    };

    struct fetch_game_time_response {
        int64_t time_of_day = 0;
    };

    struct sound_update_event {
        std::string sound_name{};
    };

    struct js_event {
        js_event_type type = js_event_type::none;
        initialize_request initialize_request_data;
        browse_folder_request browse_folder_request_data;
        browse_folder_response browse_folder_response_data;
        empty_response empty_response_data;
        load_data_event load_data_event_data;
        load_update_event load_update_event_data;
        list_maps_request list_maps_request_data;
        list_maps_response list_maps_response_data;
        list_map_pois_request list_map_pois_request_data;
        list_map_pois_response list_map_pois_response_data;
        enter_world_request enter_world_request_data;
        loading_screen_show_event loading_screen_show_event_data;
        loading_screen_progress_event loading_screen_progress_event_data;
        loading_screen_complete_event loading_screen_complete_event_data;
        area_update_event area_update_event_data;
        world_position_update_event world_position_update_event_data;
        fps_update_event fps_update_event_data;
        system_update_event system_update_event_data;
        fetch_game_time_request fetch_game_time_request_data;
        fetch_game_time_response fetch_game_time_response_data;
        sound_update_event sound_update_event_data;
    };
}

#endif //GLAD_JS_EVENT_H
