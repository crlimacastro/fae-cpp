#pragma once

#include "fae/application.hpp"
#include "fae/input.hpp"
#include "fae/time.hpp"
#include "fae/windowing.hpp"
#include "fae/rendering.hpp"

namespace fae
{
    struct application;

    struct default_plugins
    {
        time_plugin time_plugin{};
        windowing_plugin windowing_plugin{};
        input_plugin input_plugin{};
        rendering_plugin rendering_plugin{};

        auto init(application& app) const noexcept -> void
        {
            app
                .add_plugin(time_plugin)
                .add_plugin(windowing_plugin)
                .add_plugin(input_plugin)
                .add_plugin(rendering_plugin)
                ;
        }
    };
} // namespace fae
