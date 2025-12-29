#include "fmod_utils.hpp"

namespace wow::audio {
    void playing_sound::stop() const {
        channel->stop();
    }

    bool playing_sound::is_playing() const {
        FMOD::Sound *playing{};
        channel->getCurrentSound(&playing);
        auto cnl_playing = false;
        channel->isPlaying(&cnl_playing);
        return playing == sound && cnl_playing;
    }
}
