#include "fae/sdl.hpp"

#include "fae/input.hpp"
#include "fae/windowing.hpp"

namespace fae
{
    auto sdl::ref_count = std::atomic<std::size_t>{ 0 };

    auto sdl::init(const options& options) noexcept -> std::optional<sdl>
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

    sdl::~sdl() noexcept
    {
        --ref_count;
        if (ref_count == 0)
        {
            SDL_Quit();
        }
    }

    sdl::sdl(const sdl& other) noexcept
    {
        ++ref_count;
    }
    auto sdl::operator=(const sdl& other) noexcept -> sdl&
    {
        ++ref_count;
        return *this;
    }
    sdl::sdl(sdl&& other) noexcept
    {
        ++ref_count;
    }
    auto sdl::operator=(sdl&& other) noexcept -> sdl&
    {
        ++ref_count;
        return *this;
    }

    sdl::sdl() noexcept
    {
        ++ref_count;
    }

    auto update_sdl(const update_step& step) noexcept -> void
    {
        step.resources.use_resource<sdl_input>([&](sdl_input& input)
            { *input.m_was_key_pressed = *input.m_is_key_pressed; });

        for (SDL_Event event; SDL_PollEvent(&event);)
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
            case SDL_EVENT_WINDOW_RESIZED:
            {
                auto width = static_cast<std::size_t>(event.window.data1);
                auto height = static_cast<std::size_t>(event.window.data2);
                step.scheduler.invoke(window_resized{
                    .width = width,
                    .height = height,
                    .resources = step.resources,
                    .assets = step.assets,
                    .scheduler = step.scheduler,
                    .ecs_world = step.ecs_world,
                });
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
}
