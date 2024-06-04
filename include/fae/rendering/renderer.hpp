#pragma once

#include <functional>

namespace fae
{
    struct color_rgba;
    using color = color_rgba;

    struct renderer
    {
        std::function<color()> get_clear_color;
        std::function<void(color)> set_clear_color;
        std::function<void()> clear;
        std::function<void()> begin;
        std::function<void()> end;
    };
}
