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

        struct render_cube_args
        {
            const vec3 &position = vec3(0.f);
            const quat &rotation = quat(0.f, 0.f, 0.f, 1.f);
            const vec3 &scale = vec3(1.f);
            const color &tint = colors::white;
        };

        std::function<void(const render_cube_args& args)> render_cube;

        struct render_model_args
        {
            const model& model;
            const transform& transform;
        };

        std::function<void(const render_model_args& args)> render_model;
    };
}
