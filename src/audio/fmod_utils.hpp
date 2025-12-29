#ifndef WOW_UNIX_FMOD_UTILS_HPP
#define WOW_UNIX_FMOD_UTILS_HPP

#include "fmod.hpp"
#include <memory>

namespace wow::audio {
    struct playing_sound {
        FMOD::Sound* sound{};
        FMOD::Channel* channel{};

        void stop() const;
        [[nodiscard]] bool is_playing() const;
    };

    typedef std::shared_ptr<playing_sound> sound_ptr;
}

#endif //WOW_UNIX_FMOD_UTILS_HPP