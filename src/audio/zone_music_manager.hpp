#ifndef WOW_UNIX_ZONE_MUSIC_MANAGER_HPP
#define WOW_UNIX_ZONE_MUSIC_MANAGER_HPP
#include <thread>

#include "audio_manager.hpp"
#include "io/dbc/dbc_structs.h"

namespace wow::audio {
    class zone_music_manager {
        audio_manager_ptr _audio_manager{};
        io::dbc::dbc_manager_ptr _dbc_manager{};

        sound_ptr _cur_sound{};
        io::dbc::sound_entries_record  _cur_sound_entry{};
        int32_t _last_index = 0;
        int32_t _num_sounds = 0;

        int32_t _last_played_entry = -1;
        bool _is_running = true;

        std::thread _music_loop_thread{};

        void music_loop_thread_func();

        std::string select_next_sound();

    public:
        explicit zone_music_manager(
            audio_manager_ptr audio_manager,
            io::dbc::dbc_manager_ptr dbc_manager
        );

        ~zone_music_manager();

        void area_id_changed(int32_t area_id);
    };

    typedef std::shared_ptr<zone_music_manager> zone_music_manager_ptr;
}

#endif //WOW_UNIX_ZONE_MUSIC_MANAGER_HPP
