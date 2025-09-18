#ifndef WOW_UNIX_DI_H
#define WOW_UNIX_DI_H

#include <boost/di.hpp>
#include <utility>

#include "gl/window.h"
#undef Status
#include "web/web_core.h"
#include "io/mpq_manager.h"

namespace wow::utils {
    class application_module {
        std::shared_ptr<gl::window> _window{};
        std::shared_ptr<web::web_core> _web_core{};
        io::mpq_manager_ptr _mpq_manager{};

    public:
        explicit application_module(
            std::shared_ptr<gl::window> window,
            std::shared_ptr<web::web_core> web_core,
            io::mpq_manager_ptr mpq_manager
        ) : _window(std::move(window)),
            _web_core(std::move(web_core)),
            _mpq_manager(std::move(mpq_manager)) {
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
    };

    void initialize_di();

    extern std::shared_ptr<application_module> app_module;
}

#endif //WOW_UNIX_DI_H
