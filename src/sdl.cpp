module;
#include "fae/sdl.hpp"
#include <bitset>
#include <cstdint>
#include <format>

export module fae:sdl;

import :application;
import :logging;

export namespace fae
{
	struct sdl_input
	{
		auto press_key(const SDL_Keycode key) noexcept -> void
		{
			m_is_key_pressed->set(static_cast<std::size_t>(key), true);
		}

		auto press_key(const SDL_Scancode key) noexcept -> void
		{
			press_key(SDL_SCANCODE_TO_KEYCODE(key));
		}

		auto release_key(const SDL_Keycode key) noexcept -> void
		{
			m_is_key_pressed->set(static_cast<std::size_t>(key), false);
		}

		auto release_key(const SDL_Scancode key) noexcept -> void
		{
			release_key(SDL_SCANCODE_TO_KEYCODE(key));
		}

		[[nodiscard]] auto is_key_pressed(const SDL_Keycode key) const noexcept -> bool
		{
			return m_is_key_pressed->test(static_cast<std::size_t>(key));
		}

		[[nodiscard]] auto is_key_pressed(const SDL_Scancode key) const noexcept -> bool
		{
			return is_key_pressed(SDL_SCANCODE_TO_KEYCODE(key));
		}

		[[nodiscard]] auto is_key_released(const SDL_Keycode key) const noexcept -> bool
		{
			return !is_key_pressed(key);
		}

		[[nodiscard]] auto is_key_released(const SDL_Scancode key) const noexcept -> bool
		{
			return !is_key_pressed(key);
		}

		[[nodiscard]] auto is_key_just_pressed(const SDL_Keycode key) const noexcept -> bool
		{
			return is_key_pressed(key) && !was_key_pressed(key);
		}

		[[nodiscard]] auto is_key_just_pressed(const SDL_Scancode key) const noexcept -> bool
		{
			return is_key_pressed(key) && !was_key_pressed(key);
		}

		[[nodiscard]] auto is_key_just_released(const SDL_Keycode key) const noexcept -> bool
		{
			return !is_key_pressed(key) && was_key_pressed(key);
		}

		[[nodiscard]] auto is_key_just_released(const SDL_Scancode key) const noexcept -> bool
		{
			return !is_key_pressed(key) && was_key_pressed(key);
		}

		auto udpate() noexcept -> void
		{
			*m_was_key_pressed = *m_is_key_pressed;
		}

	private:
		[[nodiscard]] auto was_key_pressed(const SDL_Keycode key) const noexcept -> bool
		{
			return m_was_key_pressed->test(static_cast<std::size_t>(key));
		}

		[[nodiscard]] auto was_key_pressed(const SDL_Scancode key) const noexcept -> bool
		{
			return was_key_pressed(SDL_SCANCODE_TO_KEYCODE(key));
		}

		std::shared_ptr<std::bitset<static_cast<std::size_t>(SDLK_ENDCALL)>> m_is_key_pressed = std::make_shared<std::bitset<static_cast<std::size_t>(SDLK_ENDCALL)>>();
		std::shared_ptr<std::bitset<static_cast<std::size_t>(SDLK_ENDCALL)>> m_was_key_pressed = std::make_shared<std::bitset<static_cast<std::size_t>(SDLK_ENDCALL)>>();
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

	auto update_sdl(const update_step &step) noexcept -> void
	{
		step.resources.use_resource<sdl_input>([&](sdl_input &input)
			{ input.udpate(); });

		SDL_Event event;
		while (SDL_PollEvent(&event) != 0)
		{
			switch (event.type)
			{
			case SDL_EVENT_QUIT:
			{
				step.scheduler.invoke(application_quit{});
				break;
			}
			case SDL_EVENT_WINDOW_CLOSE_REQUESTED:
			{
				auto *window = SDL_GetWindowFromID(event.window.windowID);
				for (auto [entity, sdl_window] : step.ecs_world.query<sdl_window>())
				{
					if (sdl_window.raw == window)
					{
						sdl_window.should_close = true;
						break;
					}
				}
				break;
			}
			case SDL_EVENT_KEY_DOWN:
			{
				step.resources.use_resource<sdl_input>([&](sdl_input &input)
					{ input.press_key(event.key.keysym.sym); });
				break;
			}
			case SDL_EVENT_KEY_UP:
			{
				step.resources.use_resource<sdl_input>([&](sdl_input &input)
					{ input.release_key(event.key.keysym.sym); });
				break;
			}
			}
		}

		for (auto [entity, sdl_window] : step.ecs_world.query<sdl_window>())
		{
			if (sdl_window.should_close)
			{
				SDL_DestroyWindow(sdl_window.raw);
				entity.destroy();
			}
		}
	}
	auto deinit_sdl(const deinit_step &step) noexcept -> void
	{
		SDL_Quit();
	}

	struct sdl_plugin
	{
		bool init_timer = true;
		bool init_audio = true;
		bool init_video = true;
		bool init_haptic = false;
		bool init_gamepad = false;
		bool init_sensor = false;
		bool init_camera = false;

		auto init(application &app) const noexcept -> void
		{
			Uint32 flags = SDL_INIT_EVENTS;
			if (init_timer)
			{
				flags |= SDL_INIT_TIMER;
			}
			if (init_audio)
			{
				flags |= SDL_INIT_AUDIO;
			}
			if (init_video)
			{
				flags |= SDL_INIT_VIDEO;
			}
			if (init_haptic)
			{
				flags |= SDL_INIT_HAPTIC;
			}
			if (init_gamepad)
			{
				flags |= SDL_INIT_GAMEPAD;
			}
			if (init_sensor)
			{
				flags |= SDL_INIT_SENSOR;
			}
			if (init_camera)
			{
				flags |= SDL_INIT_CAMERA;
			}
			if (const auto error_code = SDL_Init(flags))
			{
				fae::log_error(std::format("could not initialize SDL: error code {}: error message: {}", error_code, SDL_GetError()));
				return;
			}
			app
				.emplace_resource<sdl_input>(sdl_input{})
				.add_system<update_step>(update_sdl)
				.add_system<deinit_step>(deinit_sdl);
		}
	};
} // namespace fae