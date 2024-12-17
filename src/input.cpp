#include "fae/input.hpp"

#include "fae/application/application.hpp"
#include "fae/sdl.hpp"
#include "fae/windowing.hpp"

namespace fae
{
    auto quit_on_esc(const update_step& step) noexcept -> void
    {
        step.global_entity.use_component<input>([&](input& input)
            {
			if (input.is_key_just_pressed(key::escape))
			{
				step.scheduler.invoke(application_quit{});
			} });
    }

    auto toggle_fullscreen_on_alt_enter(const update_step& step) noexcept -> void
    {
        step.global_entity.use_component<input>([&](input& input)
            {
			if (input.is_key_pressed(key::lalt) && input.is_key_just_pressed(key::enter))
			{
				step.global_entity.use_component<fae::primary_window>([&](fae::primary_window& primary)
					{
						entity_commands{.id=primary.window_entity, .registry=step.ecs_world.registry}.get_component<fae::window>()->toggle_fullscreen();
					});
			} });
    }

    auto input_plugin::init(application& app) const noexcept -> void
    {
        app.add_plugin(sdl_plugin{});
        app
            .set_global_component<fae::input>(std::move(input_from_sdl()))
            .add_system<update_step>(toggle_fullscreen_on_alt_enter);
    }
}
