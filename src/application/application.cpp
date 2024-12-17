#include "fae/application/application.hpp"

#ifdef FAE_PLATFORM_WEB
#include <emscripten/emscripten.h>
#endif

namespace fae
{
    auto application::step() -> void
    {
        scheduler.invoke(pre_update_step{
            .global_entity = global_entity,
            .assets = assets,
            .scheduler = scheduler,
            .ecs_world = ecs_world,
        });
        scheduler.invoke(update_step{
            .global_entity = global_entity,
            .assets = assets,
            .scheduler = scheduler,
            .ecs_world = ecs_world,
        });
        scheduler.invoke(post_update_step{
            .global_entity = global_entity,
            .assets = assets,
            .scheduler = scheduler,
            .ecs_world = ecs_world,
        });

        if (!is_running)
        {
            scheduler.invoke(stop_step{
                .global_entity = global_entity,
                .assets = assets,
                .scheduler = scheduler,
                .ecs_world = ecs_world,
            });
            scheduler.invoke(deinit_step{
                .global_entity = global_entity,
                .assets = assets,
                .scheduler = scheduler,
                .ecs_world = ecs_world,
            });
#ifdef FAE_PLATFORM_WEB
            emscripten_cancel_main_loop();
#endif
        }
    }

    auto application::run() -> void
    {
        is_running = true;
        scheduler.add_system<application_quit>(
            [&]([[maybe_unused]] const application_quit& event)
            {
                is_running = false;
            });
        scheduler.invoke(init_step{
            .global_entity = global_entity,
            .assets = assets,
            .scheduler = scheduler,
            .ecs_world = ecs_world,
        });
        scheduler.invoke(start_step{
            .global_entity = global_entity,
            .assets = assets,
            .scheduler = scheduler,
            .ecs_world = ecs_world,
        });
#ifdef FAE_PLATFORM_WEB
        emscripten_set_main_loop_arg([](void* arg) -> void
            { static_cast<application*>(arg)->step(); }, reinterpret_cast<void*>(this), 0, 1);
#else
        do
        {
            step();
        } while (is_running);
#endif
    }
}
