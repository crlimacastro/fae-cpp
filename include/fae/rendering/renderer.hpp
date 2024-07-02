#pragma once

#include <functional>

#include "fae/math.hpp"
#include "fae/color.hpp"
#include "fae/rendering/model.hpp"

namespace fae
{
    struct color_rgba;
    using color = color_rgba;

    struct renderer
    {
        std::function<color()> get_clear_color;
        std::function<void(color value)> set_clear_color;
        std::function<void()> clear;
        std::function<void()> begin;
        std::function<void()> end;

        struct draw_cube_args
        {
            vec3 position = vec3(0.f);
            quat rotation = quat(0.f, 0.f, 0.f, 1.f);
            vec3 scale = vec3(1.f);
            color tint = colors::white;
        };

        std::function<void(const draw_cube_args& args)> render_cube;
        std::function<void(const fae::model& model)> render_model;
    };
}
