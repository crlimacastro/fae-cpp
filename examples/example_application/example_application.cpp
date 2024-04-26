module;
#include <SDL3/SDL.h>
#include <type_traits>
#include <format>

export module example_application;

import fae;

auto start(const fae::start_step &step) noexcept -> void
{
	auto renderer = step.resources.use_resource<fae::renderer>([](fae::renderer &renderer)
															   { renderer.set_clear_color(fae::colors::cornflower_blue); });
}

auto hue_shift_clear_color(const fae::update_step &step) noexcept -> void
{
	auto renderer = step.resources.use_resource<fae::renderer>([](fae::renderer &renderer)
															   {
																			const auto speed = 0.0000001f;
																			auto clear_color_hsva = fae::color_hsva::from_rgba(renderer.get_clear_color());
																			clear_color_hsva.h = static_cast<int>((clear_color_hsva.h + speed)) % 360;
																			auto new_clear_color = clear_color_hsva.to_rgba();
																			renderer.set_clear_color(new_clear_color); });
}

auto update(const fae::update_step &step) noexcept -> void
{
}

auto fae_main(int argc, char *argv[]) -> int
{
	fae::application{}
		.add_plugin(fae::default_plugins{})
		.add_system<fae::start_step>(start)
		.add_system<fae::update_step>(fae::quit_on_esc)
		// .add_system<fae::update_step>(hue_shift_clear_color)
		.add_system<fae::update_step>(update)
		.run();
	return fae::exit_success;
}
