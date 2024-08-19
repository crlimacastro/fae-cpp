#include "fae/rendering.hpp"

#include <cstdint>
#include <format>
#include <functional>
#include <numbers>
#include <optional>
#include <string_view>
#include <variant>

#include "fae/application/application.hpp"
#include "fae/color.hpp"
#include "fae/logging.hpp"
#include "fae/math.hpp"
#include "fae/time.hpp"
#include "fae/webgpu.hpp"
#include "fae/windowing.hpp"
#include "fae/rendering/renderer.hpp"
#include "fae/rendering/render_pipeline.hpp"
#include "fae/rendering/render_pass.hpp"
#include "fae/webgpu/default_render_pipeline.hpp"

namespace fae
{
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
            app
                .emplace_resource<default_render_pipeline>(default_render_pipeline{
                    .render_pipeline = create_default_render_pipeline(app.resources, app.assets),
                })
                .emplace_resource<renderer>(
                    make_webgpu_renderer(app.resources));
        }

        app.add_system<update_step>(update_rendering)
            .add_system<render_step>(render_models)
            .add_system<window_resized>(resize_active_render_passes);
    }

    auto update_rendering(const update_step& step) noexcept -> void
    {
        static bool first_render_happened = false;
        step.resources.use_resource<fae::default_render_pipeline>([&](fae::default_render_pipeline& default_render_pipeline)
            { step.resources.use_resource<fae::renderer>(
                  [&](fae::renderer& renderer)
                  {
                      auto render_pass = renderer.begin(default_render_pipeline.render_pipeline);
                      render_pass.clear(renderer.get_clear_color());
                      step.scheduler.invoke<render_step>(render_step{
                          .resources = step.resources,
                          .assets = step.assets,
                          .scheduler = step.scheduler,
                          .ecs_world = step.ecs_world,
                          .render_pass = render_pass,
                      });
                      render_pass.end();
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
                  }); });
    }

    auto render_models(const render_step& step) noexcept -> void
    {
        for (auto& [entity, model] : step.ecs_world.query<model>())
        {
            bool should_render = true;
            entity.use_component<const visibility>([&](const fae::visibility& visibility)
                { should_render = visibility.visible; });
            if (!should_render)
                continue;

            auto transform = fae::transform{};
            entity.use_component<const fae::transform>([&](const fae::transform& t)
                { transform = t; });

            step.render_pass.render_model(render_pass::render_model_args{ .model = model, .transform = transform });
        }
    }

    auto resize_active_render_passes(const window_resized& e) noexcept -> void
    {
        e.resources.use_resource<fae::renderer>([&](fae::renderer& renderer)
            {
            for (auto& render_pass : renderer.get_active_render_passes())
            {
                render_pass.get_render_pipeline().on_window_resized(e);
            } });
    }
}
