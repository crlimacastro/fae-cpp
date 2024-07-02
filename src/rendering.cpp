#include "fae/rendering.hpp"

#include <cstdint>
#include <format>
#include <functional>
#include <numbers>
#include <optional>
#include <string_view>
#include <variant>

#include "fae/application.hpp"
#include "fae/color.hpp"
#include "fae/core.hpp"
#include "fae/logging.hpp"
#include "fae/math.hpp"
#include "fae/time.hpp"
#include "fae/webgpu.hpp"
#include "fae/windowing.hpp"

namespace fae
{
    auto update_rendering(const update_step& step) noexcept -> void
    {
        static bool first_render_happened = false;
        step.resources.use_resource<fae::renderer>(
            [&](fae::renderer& renderer)
            {
                renderer.begin();
                renderer.clear();
                step.scheduler.invoke<render_step>(render_step{
                    .resources = step.resources,
                    .assets = step.assets,
                    .scheduler = step.scheduler,
                    .ecs_world = step.ecs_world,
                });
                renderer.end();
                if (!first_render_happened)
                {
                    step.scheduler.invoke(fae::first_render_end{
                        .resources = step.resources,
                        .assets = step.assets,
                        .scheduler = step.scheduler,
                        .ecs_world = step.ecs_world,
                    });
                    first_render_happened = true;
                }
            });
    }

    auto render_models(const render_step& step) noexcept -> void
    {
        for (auto& [entity, model] : step.ecs_world.query<model>())
        {
            bool should_render = true;
            entity.use_component<const visibility>([&](const fae::visibility& visibility) {
                should_render = visibility.visible;
            });
            if (!should_render) continue;
            
            step.resources.use_resource<fae::renderer>(
                [&](fae::renderer& renderer)
                {
                    renderer.render_model(model);
                });
        }
    }

    auto rendering_plugin::init(application& app) const noexcept -> void
    {
        if (!app.resources.get<renderer>())
        {
            app.add_plugin(webgpu_plugin{});
            const auto maybe_webgpu_renderer =
                app.resources.get<webgpu>();
            if (!maybe_webgpu_renderer)
            {
                fae::log_error("webgpu renderer not found");
                return;
            }
            auto webgpu_renderer = *maybe_webgpu_renderer;
            app.emplace_resource<renderer>(
                make_webgpu_renderer(app.resources));
        }

        app.add_system<update_step>(update_rendering)
        .add_system<render_step>(render_models)
        ;
    }
}
