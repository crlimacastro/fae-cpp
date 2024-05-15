module;

#include <cstddef>
#include <functional>
#include <string>
#include <string_view>
#include <format>
#include "fae/sdl.hpp"

export module fae:windowing;

import :logging;
import :entity;
import :application;
import :sdl;

export namespace fae
{
	struct application;
	struct application_step;
	struct update_step;

	struct window
	{
		struct size
		{
			std::size_t width;
			std::size_t height;
		};

		std::function<std::string_view()> get_title;
		std::function<void(std::string_view)> set_title;
		std::function<size()> get_size;
		std::function<void(std::size_t width, std::size_t height)> set_size;
		std::function<void()> show;
		std::function<void()> hide;
		std::function<void()> update;
		std::function<bool()> should_close;
		std::function<void()> close;
		std::function<bool()> is_fullscreen;
		std::function<void(bool)> set_fullscreen;
		std::function<void()> toggle_fullscreen;
	};

	struct primary_window
	{
		fae::entity window_entity;

		[[nodiscard]] auto window() noexcept -> fae::window &
		{
			return *window_entity.get_component<fae::window>();
		}
	};

	struct first_render_end
	{
		resource_manager &resources;
		scheduler &scheduler;
		ecs_world &ecs_world;
	};

	auto show_primary_window_after_first_render(const fae::first_render_end &e) noexcept -> void
	{
		e.resources.use_resource<fae::primary_window>([](primary_window &primary)
													  { primary.window().show(); });
	}

	auto update_windows(const update_step &step) noexcept -> void
	{
		for (auto [entity, window] : step.ecs_world.query<fae::window>())
		{
			window.update();
		}
	}

	[[nodiscard]] auto make_sdl_window(fae::sdl_window &window) noexcept -> fae::window
	{
		return fae::window{
			.get_title = [&]()
			{ return SDL_GetWindowTitle(window.raw); },
			.set_title = [&](std::string_view value)
			{ SDL_SetWindowTitle(window.raw, value.data()); },
			.get_size = [&]()
															  {
																  int width{}, height{};
			SDL_GetWindowSize(window.raw, &width, &height);
			return fae::window::size
			{
				.width = static_cast<std::size_t>(width),
				.height = static_cast<std::size_t>(height),
			}; },
			.set_size = [&](std::size_t width, std::size_t height)
			{ SDL_SetWindowSize(window.raw, static_cast<int>(width), static_cast<int>(height)); },
			.show = [&]()
			{ SDL_ShowWindow(window.raw); },
			.hide = [&]()
			{ SDL_HideWindow(window.raw); },
			.update = [&]()
			{
				// do nothing
			},
			.should_close = [&]()
			{ return window.should_close; },
			.close = [&]()
			{
			window.should_close = true;
			SDL_DestroyWindow(window.raw); },
			.is_fullscreen = [&]()
			{ return SDL_GetWindowFlags(window.raw) & SDL_WINDOW_FULLSCREEN; },
			.set_fullscreen = [&](bool value)
			{ SDL_SetWindowFullscreen(window.raw, value ? SDL_TRUE : SDL_FALSE); },
			.toggle_fullscreen = [&]()
														   {
				const auto is_fullscreen = SDL_GetWindowFlags(window.raw) & SDL_WINDOW_FULLSCREEN;
				SDL_SetWindowFullscreen(window.raw, is_fullscreen ? SDL_FALSE : SDL_TRUE); },
		};
	}

	struct windowing_plugin
	{
		std::string window_title = "";
		std::size_t window_width = 1920;
		std::size_t window_height = 1080;
		bool is_window_resizable = true;
		bool should_hide_window_until_first_render = true;
		bool is_window_fullscreen = false;

		auto init(application &app) const noexcept -> void
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
			const auto maybe_sdl_window = SDL_CreateWindow(window_title.data(), static_cast<int>(window_width), static_cast<int>(window_height), flags);
			if (!maybe_sdl_window)
			{
				fae::log_error(std::format("could not create window: {}", SDL_GetError()));
				return;
			}
			auto sdl_window = fae::sdl_window{
				.raw = maybe_sdl_window,
			};
			auto window_entity = app.ecs_world.create_entity();
			auto &sdl_window_component = window_entity.set_and_get_component<fae::sdl_window>(std::move(sdl_window));
			auto window = make_sdl_window(sdl_window_component);
			window_entity.set_component<fae::window>(std::move(window));
			app.emplace_resource<primary_window>(primary_window{
				.window_entity = window_entity});
			if (should_hide_window_until_first_render)
			{
				app.add_system<first_render_end>(show_primary_window_after_first_render);
			}
			app
				.add_system<update_step>(update_windows);
		}
	};
}
