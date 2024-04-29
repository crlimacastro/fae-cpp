export module fae:default_plugins;

import :application;
import :windowing;
import :input;
import :rendering;

export namespace fae
{
	struct application;

	struct default_plugins
	{
		windowing_plugin windowing_plugin;
		input_plugin input_plugin;
		rendering_plugin rendering_plugin;

		auto init(application &app) const noexcept -> void
		{
			app
				.add_plugin(windowing_plugin)
				.add_plugin(input_plugin)
				.add_plugin(rendering_plugin);
		}
	};
} // namespace fae
