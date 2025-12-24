#ifndef WOW_UNIX_DI_H
#define WOW_UNIX_DI_H

#include <boost/di.hpp>
#include <utility>

#include "gl/window.h"
#undef Status
#include "audio/audio_manager.hpp"
#include "web/web_core.h"
#include "io/mpq_manager.h"
#include "io/minimap/minimap_provider.h"
#include "scene/world_frame.h"
#include "web/event/ui_event_system.h"
#include "config/config_manager.h"
#include "scene/camera.h"

namespace wow::utils {
    class application_module {
        std::shared_ptr<gl::window> _window{};
        std::shared_ptr<web::web_core> _web_core{};
        io::mpq_manager_ptr _mpq_manager{};
        web::event::ui_event_system_ptr _ui_event_system{};
        io::minimap::minimap_provider_ptr _minimap_provider{};
        scene::world_frame_ptr _world_frame{};
        config::config_manager_ptr _config_manager{};
        scene::gpu_dispatcher_ptr _gpu_dispatcher{};
        scene::camera_ptr _camera{};
        scene::map_manager_ptr _map_manager{};
        audio::audio_manager_ptr _audio_manager{};

    public:
        explicit application_module(
            std::shared_ptr<gl::window> window,
            std::shared_ptr<web::web_core> web_core,
            io::mpq_manager_ptr mpq_manager,
            web::event::ui_event_system_ptr ui_event_system,
            io::minimap::minimap_provider_ptr minimap_provider,
            scene::world_frame_ptr world_frame,
            config::config_manager_ptr config_manager,
            scene::gpu_dispatcher_ptr gpu_dispatcher,
            scene::camera_ptr camera,
            scene::map_manager_ptr map_manager,
            audio::audio_manager_ptr audio_manager
        ) : _window(std::move(window)),
            _web_core(std::move(web_core)),
            _mpq_manager(std::move(mpq_manager)),
            _ui_event_system(std::move(ui_event_system)),
            _minimap_provider(std::move(minimap_provider)),
            _world_frame(std::move(world_frame)),
            _config_manager(std::move(config_manager)),
            _gpu_dispatcher(std::move(gpu_dispatcher)),
            _camera(std::move(camera)),
            _map_manager(std::move(map_manager)) {
        }

        const std::shared_ptr<gl::window> &window() {
            return _window;
        }

        const std::shared_ptr<web::web_core> &web_core() {
            return _web_core;
        }

        const io::mpq_manager_ptr &mpq_manager() {
            return _mpq_manager;
        }

        const web::event::ui_event_system_ptr &ui_event_system() {
            return _ui_event_system;
        }

        const io::minimap::minimap_provider_ptr &minimap_provider() {
            return _minimap_provider;
        }

        const scene::world_frame_ptr &world_frame() {
            return _world_frame;
        }

        const config::config_manager_ptr &config_manager() {
            return _config_manager;
        }

        const scene::gpu_dispatcher_ptr &gpu_dispatcher() {
            return _gpu_dispatcher;
        }

        const scene::camera_ptr &camera() {
            return _camera;
        }

        const scene::map_manager_ptr &map_manager() {
            return _map_manager;
        }
    };

    void initialize_di();

    extern std::shared_ptr<application_module> app_module;
}

#endif //WOW_UNIX_DI_H
