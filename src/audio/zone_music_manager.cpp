#include "zone_music_manager.hpp"

#include <random>
#include "io/dbc/dbc_manager.h"
#include "utils/di.h"

#include "web/event/ui_event_system.h"

namespace wow::audio {
    void zone_music_manager::music_loop_thread_func() {
        while (_is_running) {
            if (auto cur_snd = _cur_sound; cur_snd && !cur_snd->is_playing()) {
                const auto file_name = select_next_sound();
                SPDLOG_INFO("Sound Emitter: {}", file_name);
                if (!file_name.empty()) {
                    _cur_sound = _audio_manager->play_file(file_name);

                    utils::app_module->ui_event_system()->event_manager()->submit(web::proto::JsEvent{});
                }
            }

            std::this_thread::sleep_for(std::chrono::milliseconds(1));
        }
    }

    std::string zone_music_manager::select_next_sound() {
        if (_last_played_entry <= 0) {
            return {};
        }

        auto files = _cur_sound_entry.file_names;
        const auto weights = _cur_sound_entry.frequencies;
        auto base_path = _cur_sound_entry.file_path;

        auto total_weight = 0;
        std::vector<std::pair<std::size_t, int32_t> > weight_indices{};

        for (auto i = 0; i < files.size(); ++i) {
            if (auto weight = weights[i];
                weight > 0 && !files[i].empty()) {
                total_weight += weight;
                weight_indices.emplace_back(i, weight);
            } else {
                break;
            }
        }

        if (!total_weight || weight_indices.empty()) {
            return {};
        }

        static std::mt19937 rng{std::random_device{}()};
        std::uniform_int_distribution dist{0, total_weight - 1};
        const auto roll = dist(rng);

        auto cumulative = 0;
        for (const auto &[index, weight]: weight_indices) {
            cumulative += weight;
            if (roll < cumulative) {
                _last_index = index;
                const auto sound_file_name = fmt::format("{}\\{}", base_path, files[index]);
                publish_sound_event(sound_file_name);
                return sound_file_name;
            }
        }

        _last_index = weight_indices.back().first;
        return fmt::format("{}\\{}", base_path, files[_last_index]);
    }

    void zone_music_manager::publish_sound_event(const std::string &sound_name) {
        auto ev = web::proto::JsEvent{};
        ev.mutable_sound_update_event()->set_sound_name(sound_name);
        utils::app_module->ui_event_system()->event_manager()->submit(ev);
    }

    zone_music_manager::zone_music_manager(
        audio_manager_ptr audio_manager,
        io::dbc::dbc_manager_ptr dbc_manager
    ) : _audio_manager(std::move(audio_manager)),
        _dbc_manager(std::move(dbc_manager)) {
        _music_loop_thread = std::thread{&zone_music_manager::music_loop_thread_func, this};
    }

    zone_music_manager::~zone_music_manager() {
        _is_running = false;
        _music_loop_thread.join();
    }

    void zone_music_manager::area_id_changed(const int32_t area_id) {
        if (!_dbc_manager->area_table_dbc()->has_record(area_id)) {
            return;
        }

        auto area = _dbc_manager->area_table_dbc()->record(area_id);
        while (area.zone_music <= 0) {
            if (area.parent_id <= 0) {
                return;
            }

            area = _dbc_manager->area_table_dbc()->record(area.parent_id);
        }

        if (area.zone_music <= 0 || !_dbc_manager->zone_music_dbc()->has_record(area.zone_music)) {
            return;
        }

        const auto zone_music = _dbc_manager->zone_music_dbc()->record(area.zone_music);
        if (!_dbc_manager->sound_entries_dbc()->has_record(zone_music.day_music)) {
            return;
        }

        if (_last_played_entry == zone_music.day_music) {
            return;
        }

        _last_played_entry = zone_music.day_music;

        if (_cur_sound) {
            const auto snd = _cur_sound;
            _cur_sound = nullptr;
            snd->stop();
        }

        const auto sound_entry = _dbc_manager->sound_entries_dbc()->record(zone_music.day_music);
        _cur_sound_entry = sound_entry;
        _last_index = 0;

        _num_sounds = std::ranges::count_if(sound_entry.file_names, [](const auto &name) { return !name.empty(); });

        if (const auto file_name = select_next_sound();
            !file_name.empty()) {
            SPDLOG_INFO("Zone Music: {} ({} entries), Sound Emitter: {}", zone_music.name, _num_sounds, file_name);
            publish_sound_event(file_name);
            _cur_sound = _audio_manager->play_file(file_name);
        }
    }
}
