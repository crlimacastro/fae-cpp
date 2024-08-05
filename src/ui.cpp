#include "fae/ui.hpp"

#include "fae/application.hpp"
#include "fae/imgui.hpp"
#include "fae/rendering.hpp"

namespace fae
{
    auto ui_plugin::init(application& app) const noexcept -> void
    {
        app
            .add_plugin(imgui_plugin{})
            .add_system<render_step>(render_ui);
    }

    auto render_ui(const render_step& step) noexcept -> void
    {
        step.scheduler.invoke<ui_begin_step>(ui_begin_step{
            .resources = step.resources,
            .assets = step.assets,
            .scheduler = step.scheduler,
            .ecs_world = step.ecs_world,
        });
        step.scheduler.invoke<ui_render_step>(ui_render_step{
            .resources = step.resources,
            .assets = step.assets,
            .scheduler = step.scheduler,
            .ecs_world = step.ecs_world,
        });
        step.scheduler.invoke<ui_end_step>(ui_end_step{
            .resources = step.resources,
            .assets = step.assets,
            .scheduler = step.scheduler,
            .ecs_world = step.ecs_world,
        });
    }
}
