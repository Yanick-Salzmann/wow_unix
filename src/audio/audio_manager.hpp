#ifndef WOW_UNIX_AUDIO_MANAGER_HPP
#define WOW_UNIX_AUDIO_MANAGER_HPP

#include <fmod.hpp>
#include <memory>

#include "fmod_utils.hpp"
#include "io/mpq_manager.h"

namespace wow::audio {
    class audio_manager {
        FMOD::System* _system{};
        io::mpq_manager_ptr _mpq_manager{};
    public:
        explicit audio_manager(io::mpq_manager_ptr mpq_manager);

        sound_ptr play_file(const std::string &filename) const;

        bool is_playing(const sound_ptr &sound);
    };

    typedef std::shared_ptr<audio_manager> audio_manager_ptr;
}

#endif //WOW_UNIX_AUDIO_MANAGER_HPP