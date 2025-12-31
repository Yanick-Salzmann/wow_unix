#include "di.h"

#include "gl/window.h"
#include "io/mpq_manager.h"
#include "io/dbc/dbc_manager.h"
#include "io/minimap/minimap_provider.h"
#include "web/web_core.h"
#include "web/event/event_manager.h"
#include "web/event/ui_event_system.h"

namespace wow::audio {
    class zone_music_manager;
}

namespace wow::utils {
    std::shared_ptr<application_module> app_module{};

    void initialize_di() {
        using namespace boost;

        app_module = di::make_injector(
            di::bind<gl::window>().in(di::singleton),
            di::bind<web::web_core>().in(di::singleton),
            di::bind<web::event::event_manager>().in(di::singleton),
            di::bind<io::mpq_manager>().in(di::singleton),
            di::bind<io::dbc::dbc_manager>().in(di::singleton),
            di::bind<web::event::ui_event_system>().in(di::singleton),
            di::bind<io::minimap::minimap_provider>().in(di::singleton),
            di::bind<scene::world_frame>().in(di::singleton),
            di::bind<config::config_manager>().in(di::singleton),
            di::bind<scene::gpu_dispatcher>().in(di::singleton),
            di::bind<scene::camera>().in(di::singleton),
            di::bind<audio::audio_manager>().in(di::singleton),
            di::bind<audio::zone_music_manager>().in(di::singleton)
        ).create<std::shared_ptr<application_module> >();
    }
}
