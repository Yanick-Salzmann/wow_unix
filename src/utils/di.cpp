#include "di.h"

#include "gl/window.h"
#include "io/mpq_manager.h"
#include "io/dbc/dbc_manager.h"
#include "web/web_core.h"
#include "web/event/event_manager.h"
#include "web/event/ui_event_system.h"

namespace wow::utils {
    std::shared_ptr<application_module> app_module{};

    void initialize_di() {
        app_module = boost::di::make_injector(
            boost::di::bind<gl::window>().in(boost::di::singleton),
            boost::di::bind<web::web_core>().in(boost::di::singleton),
            boost::di::bind<web::event::event_manager>().in(boost::di::singleton),
            boost::di::bind<io::mpq_manager>().in(boost::di::singleton),
            boost::di::bind<io::dbc::dbc_manager>().in(boost::di::singleton),
            boost::di::bind<web::event::ui_event_system>().in(boost::di::singleton)
        ).create<std::shared_ptr<application_module> >();
    }
}
