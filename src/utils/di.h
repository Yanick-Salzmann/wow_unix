#ifndef WOW_UNIX_DI_H
#define WOW_UNIX_DI_H

#include <boost/di.hpp>
#include <utility>

#include "gl/window.h"
#undef Status
#include "web/web_core.h"
#include "io/mpq_manager.h"
#include "io/minimap/minimap_provider.h"
#include "web/event/ui_event_system.h"

namespace wow::utils {
    class application_module {
        std::shared_ptr<gl::window> _window{};
        std::shared_ptr<web::web_core> _web_core{};
        io::mpq_manager_ptr _mpq_manager{};
        web::event::ui_event_system_ptr _ui_event_system{};
        io::minimap::minimap_provider_ptr _minimap_provider{};

    public:
        explicit application_module(
            std::shared_ptr<gl::window> window,
            std::shared_ptr<web::web_core> web_core,
            io::mpq_manager_ptr mpq_manager,
            web::event::ui_event_system_ptr ui_event_system,
            io::minimap::minimap_provider_ptr minimap_provider
        ) : _window(std::move(window)),
            _web_core(std::move(web_core)),
            _mpq_manager(std::move(mpq_manager)),
            _ui_event_system(std::move(ui_event_system)),
            _minimap_provider(std::move(minimap_provider)) {
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
    };

    void initialize_di();

    extern std::shared_ptr<application_module> app_module;
}

#endif //WOW_UNIX_DI_H
