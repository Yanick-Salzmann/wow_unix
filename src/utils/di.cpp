#include "di.h"

#include "gl/window.h"
#include "web/web_core.h"

namespace wow::utils {
    namespace internal {
        std::shared_ptr<injector_holder> _injector{};
    }

    void initialize_di() {
        auto window = gl::make_window();
        auto core = web::make_web_core(window);

        internal::_injector = std::make_shared<internal::injector_holder>(boost::di::make_injector(
            boost::di::bind<gl::window>().to(window),
            boost::di::bind<web::web_core>().to(core)
        ));
    }
}
