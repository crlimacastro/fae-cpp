#pragma once

#include <functional>
#include <vector>

// #include "fae/rendering/render_pass.hpp"

namespace fae
{
    struct color_rgba;
    using color = color_rgba;

    struct render_pass;
    struct render_pipeline;

    struct renderer
    {
        std::function<const color&()> get_clear_color;
        std::function<void(const color &value)> set_clear_color;
        std::function<render_pass(const render_pipeline&)> begin;
        std::function<std::vector<render_pass>()> get_active_render_passes;
    };
}
