#pragma once

#include "fae/input.hpp"
#include "fae/windowing.hpp"

struct SDL_Window;

namespace fae
{
    struct application;
    struct update_step;
    struct deinit_step;

    [[nodiscard]] auto input_from_sdl() noexcept -> fae::input;
    [[nodiscard]] auto window_from_sdl_window(SDL_Window* sdl_window) noexcept -> fae::window;

    struct sdl_plugin
    {
        bool init_audio = true;
        bool init_video = true;
        bool init_haptic = false;
        bool init_sensor = false;

        auto init(application& app) const noexcept -> void;
    };

    auto update_sdl(const update_step& step) noexcept -> void;
    auto deinit_sdl(const deinit_step& step) noexcept -> void;
}
