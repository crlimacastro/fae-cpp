#include "fae/windowing.hpp"

#include <SDL3/SDL.h>

#include "fae/application/application.hpp"
#include "fae/logging.hpp"
#include "fae/sdl.hpp"

namespace fae
{
    auto show_primary_window_after_first_render(const fae::first_render_end& e) noexcept -> void
    {
        e.global_entity.use_component<fae::primary_window>([&](primary_window& primary)
            { entity_commands{ .id=primary.window_entity, .registry=e.ecs_world.registry }.get_component<fae::window>()->show(); });
    }

    auto update_windows(const update_step& step) noexcept -> void
    {
        for (auto [entity, window] : step.ecs_world.query<fae::window>())
        {
            window.update();
        }
    }

    auto windowing_plugin::init(application& app) const noexcept -> void
    {
        app.add_plugin(sdl_plugin{});

        Uint32 flags = 0;
        if (is_window_resizable)
        {
            flags |= SDL_WINDOW_RESIZABLE;
        }
        if (should_hide_window_until_first_render)
        {
            flags |= SDL_WINDOW_HIDDEN;
        }
        if (is_window_fullscreen)
        {
            flags |= SDL_WINDOW_FULLSCREEN;
        }

        auto window = SDL_CreateWindow(window_title.c_str(), static_cast<int>(window_width), static_cast<int>(window_height), flags);
        if (!window)
        {
            fae::log_error(std::format("could not create window: {}", SDL_GetError()));
            return;
        }
        auto window_entity = app.ecs_world.create_entity();
        auto window_component = window_entity.get_or_set_component<SDL_Window*>(std::move(window));
        window_entity
            .set_component<name>(name{ .value = "primary window" })
            .set_component<fae::window>(std::move(window_from_sdl_window(window_component)));
        app.set_global_component<primary_window>(primary_window{
            .window_entity = window_entity.id,
        });
        if (should_hide_window_until_first_render)
        {
            app.add_system<first_render_end>(show_primary_window_after_first_render);
        }
        app
            .add_system<update_step>(update_windows);
    }
}
