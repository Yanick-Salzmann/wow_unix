#include "gl/window.h"
#include "spdlog/spdlog.h"
#include "utils/di.h"
#include "web/web_core.h"

int main(const int argc, char *argv[]) {
    spdlog::set_level(spdlog::level::debug);

    wow::utils::initialize_di();

    const auto window = wow::utils::create<wow::gl::window>();
    const auto core = wow::utils::create<wow::web::web_core>();

    core->initialize(argc, argv);

    while (window->process_events()) {
        window->begin_frame();
        core->render();
        window->end_frame();
    }

    core->shutdown();
    window->terminate();


    return 0;
}
