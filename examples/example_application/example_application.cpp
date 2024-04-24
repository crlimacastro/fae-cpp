export module example_application;

import fae;

auto start(const fae::start_step &step) noexcept -> void
{
	auto renderer = step.commands.resources.use_resource<fae::renderer>([](fae::renderer &renderer)
																		{ renderer.set_clear_color(fae::colors::cornflower_blue); });
}

auto fae_main(int argc, char *argv[]) -> int
{
	fae::application{}
		.add_plugin(fae::default_plugins{
			.windowing_plugin = fae::windowing_plugin{
				.should_hide_window_until_first_render = false,
			},
		})
		.add_system<fae::start_step>(start)
		.add_system<fae::update_step>(fae::quit_on_esc)
		.run();
	return fae::exit_success;
}
