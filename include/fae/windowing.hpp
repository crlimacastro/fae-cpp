#pragma once

#include <cstddef>
#include <format>
#include <functional>
#include <string>
#include <string_view>

#include "fae/logging.hpp"
#include "fae/entity.hpp"
#include "fae/application.hpp"
#include "fae/sdl.hpp"

namespace fae
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

	auto show_primary_window_after_first_render(const fae::first_render_end &e) noexcept -> void;
	auto update_windows(const update_step &step) noexcept -> void;
	[[nodiscard]] auto make_sdl_window(fae::sdl_window &window) noexcept -> fae::window;

	struct windowing_plugin
	{
		std::string window_title = "";
		std::size_t window_width = 1920;
		std::size_t window_height = 1080;
		bool is_window_resizable = true;
		bool should_hide_window_until_first_render = true;
		bool is_window_fullscreen = false;

		auto init(application &app) const noexcept -> void;
	};
}
