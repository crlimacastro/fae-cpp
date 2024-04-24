module;

#include <cstdint>
#include <functional>
#include <optional>
#include <string_view>
#include <variant>
#include <format>

#include <SDL3/SDL.h>

export module fae:rendering;

import :core;
import :logging;
import :application;
import :sdl;
import :webgpu;
import :windowing;

export namespace fae
{
	struct color
	{
		std::uint8_t r = 0;
		std::uint8_t g = 0;
		std::uint8_t b = 0;
		std::uint8_t a = 255;
	};

	namespace colors
	{
		constexpr auto black = color{0, 0, 0, 255};
		constexpr auto white = color{255, 255, 255, 255};
		constexpr auto red = color{255, 0, 0, 255};
		constexpr auto green = color{0, 255, 0, 255};
		constexpr auto blue = color{0, 0, 255, 255};
		constexpr auto cornflower_blue = color{100, 149, 237, 255};
	}

	struct renderer
	{
		std::function<color()> get_clear_color;
		std::function<void(color)> set_clear_color;
		std::function<void()> clear;
		std::function<void()> begin;
		std::function<void()> end;
	};

	struct render_step
	{
		application_commands commands;
	};

	auto update_rendering(const update_step &step) noexcept -> void
	{
		static bool first_render_happened = false;
		step.commands.resources.use_resource<fae::renderer>([&](fae::renderer &renderer)
															{
			renderer.begin();
			renderer.clear();
			step.commands.scheduler.invoke<render_step>(render_step{ .commands = step.commands });
			renderer.end();
			if (!first_render_happened)
			{
				step.commands.scheduler.invoke(fae::first_render_end{
					.commands = step.commands,
					});
				first_render_happened = true;
			} });
	}

	auto deinit_rendering(const deinit_step &step) noexcept -> void
	{
		step.commands.resources.erase<fae::sdl_renderer>();
		step.commands.resources.erase<fae::renderer>();
	}

	auto renderer_from(fae::sdl_renderer &renderer) noexcept -> fae::renderer
	{
		return fae::renderer{
			.get_clear_color = [&]() -> fae::color
			{
				Uint8 r{}, g{}, b{}, a{};
				SDL_GetRenderDrawColor(renderer.raw, &r, &g, &b, &a);
				return fae::color(r, g, b, a);
			},
			.set_clear_color = [&](fae::color value)
			{ SDL_SetRenderDrawColor(renderer.raw, value.r, value.g, value.b, value.a); },
			.clear = [&]()
			{ SDL_RenderClear(renderer.raw); },
			.begin = [&]()
			{
				// do nothing
			},
			.end = [&]()
			{ SDL_RenderPresent(renderer.raw); },
		};
	}

	struct rendering_plugin
	{
		struct sdl_renderer_config
		{
			std::optional<std::string_view> rendering_driver_name = std::nullopt;
			enum class sdl_renderer_type
			{
				software,
				hardware,
			};
			sdl_renderer_type type = sdl_renderer_type::hardware;
			bool vsync = true;
		};

		struct webgpu_renderer_config
		{
			// TODO
		};

		std::variant<
			sdl_renderer_config, webgpu_renderer_config>
			renderer_config = webgpu_renderer_config{};

		auto init(application &app) const noexcept -> void
		{
			app.add_plugin(windowing_plugin{});

			fae::match(
				renderer_config,
				[&](sdl_renderer_config config)
				{
					app.add_plugin(sdl_plugin{});
					auto maybe_primary = app.resources.get<primary_window>();
					if (!maybe_primary)
					{
						fae::log_error("no primary window found");
						return;
					}
					auto &primary = *maybe_primary;
					auto maybe_sdl_window = primary.window_entity.get_component<fae::sdl_window>();
					if (!maybe_sdl_window)
					{
						fae::log_error("primary window is not an sdl_window");
						return;
					}
					auto &sdl_window = *maybe_sdl_window;

					Uint32 flags = 0;

					switch (config.type)
					{
					case sdl_renderer_config::sdl_renderer_type::software:
					{
						flags |= SDL_RENDERER_SOFTWARE;
						break;
					}
					case sdl_renderer_config::sdl_renderer_type::hardware:
					{
						flags |= SDL_RENDERER_ACCELERATED;
						break;
					}
					default:
					{
						fae::log_error("unknown sdl renderer type");
						return;
					}
					}
					if (config.vsync)
					{
						flags |= SDL_RENDERER_PRESENTVSYNC;
					}
					const auto rendering_driver_name = config.rendering_driver_name.has_value() ? config.rendering_driver_name.value().data() : nullptr;
					const auto maybe_renderer = SDL_CreateRenderer(sdl_window.raw, rendering_driver_name, flags);
					if (!maybe_renderer)
					{
						fae::log_error(std::format("could not create renderer: {}", SDL_GetError()));
						return;
					}
					auto &sdl_renderer = app.resources.emplace_and_get<fae::sdl_renderer>(fae::sdl_renderer{
						.raw = maybe_renderer,
					});
					app.emplace_resource<fae::renderer>(renderer_from(sdl_renderer));
				},
				[&](webgpu_renderer_config config)
				{
					app.add_plugin(webgpu_plugin{});

					auto maybe_primary = app.resources.get<primary_window>();
					if (!maybe_primary)
					{
						fae::log_error("no primary window found");
						return;
					}
					auto &primary = *maybe_primary;
					auto maybe_sdl_window = primary.window_entity.get_component<fae::sdl_window>();
					if (!maybe_sdl_window)
					{
						fae::log_error("primary window is not an sdl_window");
						return;
					}
					auto &sdl_window = *maybe_sdl_window;
				},
				[&]([[maybe_unused]] auto other)
				{
					fae::log_error("unknown renderer type");
				});

			app
				.add_system<update_step>(update_rendering)
				.add_system<deinit_step>(deinit_rendering);
		}
	};
}
