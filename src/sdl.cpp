#include "fae/sdl.hpp"

#include <imgui.h>
#include <backends/imgui_impl_sdl3.h>

#include "fae/input.hpp"
#include "fae/windowing.hpp"
#include "fae/application/application.hpp"

auto fae::sdl::ref_count = std::atomic<std::size_t>{ 0 };

auto fae::sdl::init(const options& options) noexcept -> std::optional<fae::sdl>
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

fae::sdl::~sdl() noexcept
{
    --ref_count;
    if (ref_count == 0)
    {
        SDL_Quit();
    }
}

fae::sdl::sdl(const sdl& other) noexcept
{
    ++ref_count;
}
auto fae::sdl::operator=(const sdl& other) noexcept -> sdl&
{
    ++ref_count;
    return *this;
}
fae::sdl::sdl(sdl&& other) noexcept
{
    ++ref_count;
}
auto fae::sdl::operator=(sdl&& other) noexcept -> sdl&
{
    ++ref_count;
    return *this;
}

fae::sdl::sdl() noexcept
{
    ++ref_count;
}

auto fae::sdl_plugin::init(application& app) const noexcept -> void
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
        .add_system<update_step>(update_sdl)
        .add_system<update_step>(destroy_sdl_window_entities_that_should_close);
}

auto fae::update_sdl(const fae::update_step& step) noexcept -> void
{
    step.resources.use_resource<fae::sdl_input>([&](fae::sdl_input& input)
        { input.update(); });

    for (SDL_Event event; SDL_PollEvent(&event);)
    {
        switch (event.type)
        {
        case SDL_EVENT_QUIT:
        {
            step.scheduler.invoke(fae::application_quit{});
            break;
        }
        case SDL_EVENT_WINDOW_CLOSE_REQUESTED:
        {
            auto* window = SDL_GetWindowFromID(event.window.windowID);
            for (auto [entity, sdl_window] : step.ecs_world.query<fae::sdl_window>())
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
            step.resources.use_resource<fae::sdl_input>([&](fae::sdl_input& input)
                { input.press_key(event.key.key); });
            break;
        }
        case SDL_EVENT_KEY_UP:
        {
            step.resources.use_resource<fae::sdl_input>([&](fae::sdl_input& input)
                { input.release_key(event.key.key); });
            break;
        }
        case SDL_EVENT_WINDOW_RESIZED:
        {
            auto width = static_cast<std::size_t>(event.window.data1);
            auto height = static_cast<std::size_t>(event.window.data2);
            step.scheduler.invoke(fae::window_resized{
                .width = width,
                .height = height,
                .resources = step.resources,
                .assets = step.assets,
                .scheduler = step.scheduler,
                .ecs_world = step.ecs_world,
            });
        }
        case SDL_EVENT_WINDOW_FOCUS_GAINED:
        {
            auto* window = SDL_GetWindowFromID(event.window.windowID);
            for (auto [entity, sdl_window] : step.ecs_world.query<fae::sdl_window>())
            {
                if (sdl_window.raw.get() == window)
                {
                    sdl_window.is_focused = true;
                    break;
                }
            }
            break;
        }
        case SDL_EVENT_WINDOW_FOCUS_LOST:
        {
            auto* window = SDL_GetWindowFromID(event.window.windowID);
            for (auto [entity, sdl_window] : step.ecs_world.query<fae::sdl_window>())
            {
                if (sdl_window.raw.get() == window)
                {
                    sdl_window.is_focused = false;
                    break;
                }
            }
            break;
        }
        }
        ImGui_ImplSDL3_ProcessEvent(&event);
    }
}

auto fae::destroy_sdl_window_entities_that_should_close(const fae::update_step& step) noexcept -> void
{
    for (auto [entity, sdl_window] : step.ecs_world.query<fae::sdl_window>())
    {
        if (sdl_window.should_close)
        {
            entity.destroy();
        }
    }
}
