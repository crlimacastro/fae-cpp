module;

#include "fae/sdl.hpp"
#include <atomic>
#include <bitset>
#include <cstddef>
#include <cstdint>
#include <expected>
#include <format>

export module fae:sdl;

import :core;
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

	struct sdl_error
	{
		int code;
		std::string message;
	};

	struct sdl
	{
		static std::atomic<std::size_t> ref_count;

		struct sdl_options
		{
			bool init_timer = true;
			bool init_audio = true;
			bool init_video = true;
			bool init_haptic = false;
			bool init_gamepad = false;
			bool init_sensor = false;
			bool init_camera = false;
		};

		static auto init(const sdl_options &options) noexcept -> std::expected<sdl, sdl_error>
		{
			if (ref_count == 0)
			{
				Uint32 flags = SDL_INIT_EVENTS;
				if (options.init_timer)
				{
					flags |= SDL_INIT_TIMER;
				}
				if (options.init_audio)
				{
					flags |= SDL_INIT_AUDIO;
				}
				if (options.init_video)
				{
					flags |= SDL_INIT_VIDEO;
				}
				if (options.init_haptic)
				{
					flags |= SDL_INIT_HAPTIC;
				}
				if (options.init_gamepad)
				{
					flags |= SDL_INIT_GAMEPAD;
				}
				if (options.init_sensor)
				{
					flags |= SDL_INIT_SENSOR;
				}
				if (options.init_camera)
				{
					flags |= SDL_INIT_CAMERA;
				}

				if (const auto error_code = SDL_Init(flags))
				{
					return std::unexpected(sdl_error{
						.code = error_code,
						.message = SDL_GetError()});
				}
			}
			return sdl{};
		}

		~sdl() noexcept
		{
			--ref_count;
			if (ref_count == 0)
			{
				SDL_Quit();
			}
		}

		sdl(const sdl &) noexcept
		{
			++ref_count;
		}
		auto operator=(const sdl &) noexcept -> sdl &
		{
			++ref_count;
			return *this;
		}
		sdl(sdl &&) noexcept
		{
			++ref_count;
		}
		auto operator=(sdl &&) noexcept -> sdl &
		{
			++ref_count;
			return *this;
		}

	private:
		sdl() noexcept
		{
			++ref_count;
		}
	};
	auto sdl::ref_count = std::atomic<std::size_t>{0};

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

	struct sdl_plugin
	{
		sdl::sdl_options options{};

		auto init(application &app) const noexcept -> void
		{
			auto maybe_sdl = sdl::init(options);
			if (!maybe_sdl)
			{
				fae::log_error(std::format("could not initialize SDL: error code {}: error message: {}", maybe_sdl.error().code, maybe_sdl.error().message));
				return;
			}
			auto &sdl = *maybe_sdl;
			app
				.insert_resource<fae::sdl>(std::move(sdl))
				.emplace_resource<sdl_input>(sdl_input{})
				.add_system<update_step>(update_sdl);
		}
	};
} // namespace fae
