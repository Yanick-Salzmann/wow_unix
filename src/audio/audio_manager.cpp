#include "audio_manager.hpp"

#include <fmod.hpp>

#include "spdlog/spdlog.h"

namespace wow::audio {
    audio_manager::audio_manager(io::mpq_manager_ptr mpq_manager) : _mpq_manager(std::move(mpq_manager)) {
        if (FMOD::System_Create(&_system) != FMOD_OK) {
            SPDLOG_ERROR("Cannot create FMOD system");
            throw std::runtime_error("Cannot create FMOD system");
        }

        if (_system->init(32, FMOD_INIT_NORMAL, nullptr) != FMOD_OK) {
            SPDLOG_ERROR("Cannot initialize FMOD system");
            throw std::runtime_error("Cannot initialize FMOD system");
        }

        uint32_t version = 0, build = 0;
        _system->getVersion(&version, &build);
        uint32_t minor = (version & 0xFF);
        uint32_t major = (version >> 8) & 0xFF;
        uint32_t product = (version >> 16) & 0xFFFF;
        SPDLOG_INFO("FMOD version {}.{}.{}.{}", product, major, minor, build);
    }

    sound_ptr audio_manager::play_file(const std::string &filename) const {
        const auto file = _mpq_manager->open(filename);
        if (!file) {
            return {};
        }

        FMOD::Sound *sound;
        FMOD_CREATESOUNDEXINFO sound_info{};
        sound_info.cbsize = sizeof(FMOD_CREATESOUNDEXINFO);
        sound_info.length = file->full_data().size();

        _system->createSound(reinterpret_cast<const char *>(file->full_data().data()), FMOD_OPENMEMORY | FMOD_CREATESAMPLE, &sound_info, &sound);
        FMOD::Channel *channel{};
        _system->playSound(sound, nullptr, false, &channel);

        const auto ps = new playing_sound(sound, channel);
        auto ptr = std::shared_ptr<playing_sound>(ps, [](const playing_sound *e) {
            e->sound->release();
            delete e;
        });

        return ptr;
    }
}
