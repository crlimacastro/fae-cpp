#include "fae/ui.hpp"

#include "fae/application.hpp"
#include "fae/imgui.hpp"
#include "fae/rendering.hpp"
#include "fae/windowing.hpp"

namespace fae
{
    auto ui_plugin::init(application& app) const noexcept -> void
    {
        app
            .add_plugin(imgui_plugin{})
            .insert_resource(ui_settings{})
            .add_system<render_step>(render_ui);
    }

    auto render_ui(const render_step& step) noexcept -> void
    {
        bool hide_ui = false;
        step.resources.use_resource<ui_settings>([&](ui_settings& ui)
            { hide_ui = ui.hide_ui; });
        step.resources.use_resource<primary_window>([&](primary_window& window)
            {
            if (!window.window_entity.valid())
            {
                hide_ui = true;
            } });
        if (hide_ui)
        {
            return;
        }

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
