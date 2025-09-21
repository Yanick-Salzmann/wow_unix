#include "di.h"

#include "gl/window.h"
#include "io/mpq_manager.h"
#include "io/dbc/dbc_manager.h"
#include "io/minimap/minimap_provider.h"
#include "web/web_core.h"
#include "web/event/event_manager.h"
#include "web/event/ui_event_system.h"

namespace wow::utils {
    std::shared_ptr<application_module> app_module{};

    void initialize_di() {
        using namespace boost::di;

        app_module = make_injector(
            bind<gl::window>().in(singleton),
            bind<web::web_core>().in(singleton),
            bind<web::event::event_manager>().in(singleton),
            bind<io::mpq_manager>().in(singleton),
            bind<io::dbc::dbc_manager>().in(singleton),
            bind<web::event::ui_event_system>().in(singleton),
            bind<io::minimap::minimap_provider>().in(singleton)
        ).create<std::shared_ptr<application_module> >();
    }
}
