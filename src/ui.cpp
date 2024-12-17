#include "fae/ui.hpp"

#include "fae/application/application.hpp"
#include "fae/imgui.hpp"
#include "fae/rendering/rendering.hpp"
#include "fae/windowing.hpp"

namespace fae
{
    auto ui_plugin::init(application& app) const noexcept -> void
    {
        app
            .add_plugin(imgui_plugin{})
            .set_global_component(ui_settings{})
            .add_system<render_step>(render_ui);
    }

    auto render_ui(const render_step& step) noexcept -> void
    {
        bool hide_ui = false;
        step.global_entity.use_component<ui_settings>([&](ui_settings& ui)
            { hide_ui = ui.hide_ui; });
        step.global_entity.use_component<primary_window>([&](primary_window& window)
            {
            if (!step.ecs_world.get_entity(window.window_entity).valid())
            {
                hide_ui = true;
            } });
        if (hide_ui)
        {
            return;
        }

        step.scheduler.invoke<ui_begin_step>(ui_begin_step{
            .global_entity = step.global_entity,
            .assets = step.assets,
            .scheduler = step.scheduler,
            .ecs_world = step.ecs_world,
        });
        step.scheduler.invoke<ui_render_step>(ui_render_step{
            .global_entity = step.global_entity,
            .assets = step.assets,
            .scheduler = step.scheduler,
            .ecs_world = step.ecs_world,
        });
        step.scheduler.invoke<ui_end_step>(ui_end_step{
            .global_entity = step.global_entity,
            .assets = step.assets,
            .scheduler = step.scheduler,
            .ecs_world = step.ecs_world,
        });
    }
}
