#pragma once

#include <atomic>
#include <bitset>
#include <cstddef>
#include <cstdint>
#include <format>
#include <functional>
#include <memory>
#include <optional>

#include "SDL3/SDL.h"

#ifdef FAE_PLATFORM_WEB
#include <emscripten/emscripten.h>
#endif

#include "fae/application.hpp"
#include "fae/core.hpp"
#include "fae/logging.hpp"

namespace fae
{
    using unique_sdl_window_ptr = std::unique_ptr<SDL_Window, std::function<void(SDL_Window*)>>;

    struct sdl_window
    {
        unique_sdl_window_ptr raw;
        bool should_close = false;

        struct options
        {
            std::string title = "";
            std::size_t width = 1920;
            std::size_t height = 1080;
            bool is_resizable = true;
            bool is_hidden = false;
            bool is_fullscreen = false;
        };

        static auto create(const options& options) noexcept -> std::optional<sdl_window>
        {
            Uint32 flags = 0;
            if (options.is_resizable)
            {
                flags |= SDL_WINDOW_RESIZABLE;
            }
            if (options.is_hidden)
            {
                flags |= SDL_WINDOW_HIDDEN;
            }
            if (options.is_fullscreen)
            {
                flags |= SDL_WINDOW_FULLSCREEN;
            }
            if (auto maybe_window = SDL_CreateWindow(options.title.data(), static_cast<int>(options.width), static_cast<int>(options.height), flags); maybe_window)
            {
                return sdl_window{
                    .raw = unique_sdl_window_ptr(maybe_window, SDL_DestroyWindow)};
            }
            return std::nullopt;
        }
    };

    struct sdl_renderer
    {
        std::shared_ptr<SDL_Renderer> raw = nullptr;

        struct options
        {
            std::optional<std::string_view> rendering_driver_name = std::nullopt;
            bool vsync = true;

            [[nodiscard]] auto get_rendering_driver_name() const noexcept -> const char*
            {
                return rendering_driver_name
                           ? (*rendering_driver_name).data()
                           : nullptr;
            };
        };

        static auto create(const sdl_window& window, const sdl_renderer::options& options) noexcept -> std::optional<sdl_renderer>
        {
            Uint32 flags = 0;

            if (options.vsync)
            {
                flags |= SDL_RENDERER_PRESENTVSYNC;
            }

            if (auto maybe_renderer = SDL_CreateRenderer(window.raw.get(), options.get_rendering_driver_name(), flags))
            {
                return sdl_renderer{
                    .raw = std::shared_ptr<SDL_Renderer>(maybe_renderer, SDL_DestroyRenderer),
                };
            }

            return std::nullopt;
        }
    };

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

    struct sdl
    {
        static std::atomic<std::size_t> ref_count;

        struct options
        {
            bool init_timer = true;
            bool init_audio = true;
            bool init_video = true;
            bool init_haptic = false;
            bool init_sensor = false;
        };

        static auto init(const options& options) noexcept -> std::optional<sdl>
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
                if (options.init_sensor)
                {
                    flags |= SDL_INIT_SENSOR;
                }

                if (const auto error_code = SDL_Init(flags))
                {
                    return std::nullopt;
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

        sdl(const sdl& other) noexcept
        {
            ++ref_count;
        }
        auto operator=(const sdl& other) noexcept -> sdl&
        {
            ++ref_count;
            return *this;
        }
        sdl(sdl&& other) noexcept
        {
            ++ref_count;
        }
        auto operator=(sdl&& other) noexcept -> sdl&
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

    auto update_sdl(const update_step& step) noexcept -> void
    {
        step.resources.use_resource<sdl_input>([&](sdl_input& input)
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
                auto* window = SDL_GetWindowFromID(event.window.windowID);
                for (auto [entity, sdl_window] : step.ecs_world.query<sdl_window>())
                {
                    if (sdl_window.raw.get() == window)
                    {
                        sdl_window.should_close = true;
                        break;
                    }
                }
                break;
            }
            case SDL_EVENT_KEY_DOWN:
            {
                step.resources.use_resource<sdl_input>([&](sdl_input& input)
                    { input.press_key(event.key.keysym.sym); });
                break;
            }
            case SDL_EVENT_KEY_UP:
            {
                step.resources.use_resource<sdl_input>([&](sdl_input& input)
                    { input.release_key(event.key.keysym.sym); });
                break;
            }
            }
        }
    }

    auto destroy_sdl_window_entities_that_should_close(const update_step& step) noexcept -> void
    {
        for (auto [entity, sdl_window] : step.ecs_world.query<sdl_window>())
        {
            if (sdl_window.should_close)
            {
                entity.destroy();
            }
        }
    }

    struct sdl_plugin
    {
        sdl::options options{};

        auto init(application& app) const noexcept -> void
        {
            auto maybe_sdl = sdl::init(options);
            if (!maybe_sdl)
            {
                fae::log_error(std::format("could not initialize SDL: {}", SDL_GetError()));
                return;
            }
            auto& sdl = *maybe_sdl;
            app
                .insert_resource<fae::sdl>(std::move(sdl))
                .insert_resource<sdl_input>(std::move(sdl_input{}))
                .add_system<update_step>(update_sdl);
        }
    };
} // namespace fae
