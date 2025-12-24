#include "fmod_utils.hpp"

namespace wow::audio {
    void playing_sound::stop() const {
        channel->stop();
    }
}
