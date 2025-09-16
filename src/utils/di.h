#ifndef WOW_UNIX_DI_H
#define WOW_UNIX_DI_H

#include <boost/di.hpp>

#include "gl/window.h"
#undef Status
#include "web/web_core.h"

namespace wow::utils {
    class application_module {
        std::shared_ptr<gl::window> _window{};
        std::shared_ptr<web::web_core> _web_core{};

    public:
        explicit application_module(
            std::shared_ptr<gl::window> window,
            std::shared_ptr<web::web_core> web_core
        ) : _window(std::move(window)),
            _web_core(std::move(web_core)) {
        }

        const std::shared_ptr<gl::window> &window() {
            return _window;
        }

        const std::shared_ptr<web::web_core> &web_core() {
            return _web_core;
        }
    };

    void initialize_di();

    extern std::shared_ptr<application_module> app_module;
}

#endif //WOW_UNIX_DI_H
