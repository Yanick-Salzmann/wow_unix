#include "gl/window.h"
#include "spdlog/spdlog.h"
#include "utils/di.h"
#include "web/web_core.h"

int main(const int argc, char *argv[]) {
    spdlog::set_pattern("[%Y-%m-%d %H:%M:%S.%e] [%P:%t] [%^%l%$] [%s:%#] %v");
    spdlog::set_level(spdlog::level::debug);

    wow::utils::initialize_di();

    const auto window = wow::utils::app_module->window();
    const auto core = wow::utils::app_module->web_core();
    const auto world_frame = wow::utils::app_module->world_frame();

    core->initialize(argc, argv);

    while (window->process_events()) {
        window->begin_frame();

        world_frame->on_frame();
        core->render();

        window->end_frame();
    }

    world_frame->shutdown();

    core->shutdown();
    window->terminate();

    return 0;
}
