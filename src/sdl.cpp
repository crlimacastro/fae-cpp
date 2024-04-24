module;
#include <memory>
#include <unordered_map>
#include <format>
#include <SDL3/SDL.h>

export module fae:sdl;

import :application;
import :logging;

export namespace fae
{
	struct sdl_input
	{
		void press_key(const SDL_Keycode key) noexcept
		{
			m_is_key_pressed[key] = true;
		}

		void press_key(const SDL_Scancode key) noexcept
		{
			press_key(SDL_SCANCODE_TO_KEYCODE(key));
		}

		void release_key(const SDL_Keycode key) noexcept
		{
			m_is_key_pressed[key] = false;
		}

		void release_key(const SDL_Scancode key) noexcept
		{
			release_key(SDL_SCANCODE_TO_KEYCODE(key));
		}

		auto is_key_pressed(const SDL_Keycode key) const noexcept -> bool
		{
			const auto it = m_is_key_pressed.find(key);
			return (it != m_is_key_pressed.end()) ? it->second : false;
		}

		auto is_key_pressed(const SDL_Scancode key) const noexcept -> bool
		{
			return is_key_pressed(SDL_SCANCODE_TO_KEYCODE(key));
		}

		auto is_key_released(const SDL_Keycode key) const noexcept -> bool
		{
			return !is_key_pressed(key);
		}

		auto is_key_released(const SDL_Scancode key) const noexcept -> bool
		{
			return !is_key_pressed(key);
		}

		auto is_key_just_pressed(const SDL_Keycode key) const noexcept -> bool
		{
			return is_key_pressed(key) && !was_key_pressed(key);
		}

		auto is_key_just_pressed(const SDL_Scancode key) const noexcept -> bool
		{
			return is_key_pressed(key) && !was_key_pressed(key);
		}

		auto is_key_just_released(const SDL_Keycode key) const noexcept -> bool
		{
			return !is_key_pressed(key) && was_key_pressed(key);
		}

		auto is_key_just_released(const SDL_Scancode key) const noexcept -> bool
		{
			return !is_key_pressed(key) && was_key_pressed(key);
		}

		auto udpate() noexcept -> void
		{
			m_was_key_pressed = m_is_key_pressed;
		}

	private:
		auto was_key_pressed(const SDL_Keycode key) const -> bool
		{
			const auto it = m_was_key_pressed.find(key);
			return (it != m_was_key_pressed.end()) ? it->second : false;
		}

		auto was_key_pressed(const SDL_Scancode key) const -> bool
		{
			const auto it = m_was_key_pressed.find(SDL_SCANCODE_TO_KEYCODE(key));
			return (it != m_was_key_pressed.end()) ? it->second : false;
		}

		std::unordered_map<SDL_Keycode, bool> m_is_key_pressed{};
		std::unordered_map<SDL_Keycode, bool> m_was_key_pressed{};
	};

	struct sdl_window
	{
		SDL_Window *raw;
		bool should_close = false;
	};

	struct sdl_renderer
	{
		SDL_Renderer *raw;
	};

	struct sdl_settings
	{
		bool should_quit_app_on_sdl_quit = true;
	};

	auto update_sdl(const update_step &step) noexcept -> void
	{
		step.commands.resources.use_resource<sdl_input>([&](sdl_input &input)
														{ input.udpate(); });

		SDL_Event event;
		while (SDL_PollEvent(&event) != 0)
		{
			switch (event.type)
			{
			case SDL_EVENT_QUIT:
			{
				step.commands.resources.use_resource<sdl_settings>([&](sdl_settings &settings)
																   {
					if (settings.should_quit_app_on_sdl_quit)
					{
						step.commands.scheduler.invoke(application_quit{});
					} });
				break;
			}
			case SDL_EVENT_WINDOW_CLOSE_REQUESTED:
			{
				auto window = SDL_GetWindowFromID(event.window.windowID);
				SDL_DestroyWindow(window);
				break;
			}
			case SDL_EVENT_KEY_DOWN:
			{
				step.commands.resources.use_resource<sdl_input>([&](sdl_input &input)
																{ input.press_key(event.key.keysym.sym); });
				break;
			}
			case SDL_EVENT_KEY_UP:
			{
				step.commands.resources.use_resource<sdl_input>([&](sdl_input &input)
																{ input.release_key(event.key.keysym.sym); });
				break;
			}
			}
		}
	}
	auto deinit_sdl(const deinit_step &step) noexcept -> void
	{
		step.commands.resources.use_resource<sdl_renderer>([&](const sdl_renderer &renderer)
														   { SDL_DestroyRenderer(renderer.raw); });
		step.commands.resources.use_resource<sdl_window>([&](const sdl_window &window)
														 { SDL_DestroyWindow(window.raw); });
		SDL_Quit();
	}

	struct sdl_plugin
	{
		bool init_video = true;
		bool init_audio = true;
		sdl_settings sdl_settings{};

		auto init(application &app) const noexcept -> void
		{
			Uint32 flags = 0;
			if (init_video)
			{
				flags |= SDL_INIT_VIDEO;
			}
			if (init_audio)
			{
				flags |= SDL_INIT_AUDIO;
			}
			if (const auto error_code = SDL_Init(flags))
			{
				fae::log_error(std::format("could not initialize SDL: {}-{}", error_code, SDL_GetError()));
				return;
			}
			app
				.emplace_resource<fae::sdl_settings>(sdl_settings)
				.add_system<update_step>(update_sdl)
				.add_system<deinit_step>(deinit_sdl);
		}
	};
}