#pragma once

#include <cstddef>
#include <functional>

#include "fae/math.hpp"
#include "fae/color.hpp"

namespace fae
{
    struct model;

    struct render_pass
    {
        std::function<std::size_t()> get_id;
        std::function<const render_pipeline&()> get_render_pipeline;
        std::function<void(const color&)> clear;
        std::function<void()> end;

        struct render_model_args
        {
            const model& model;
            const transform& transform;
        };
        std::function<void(const render_model_args& args)> render_model;
    };
}
