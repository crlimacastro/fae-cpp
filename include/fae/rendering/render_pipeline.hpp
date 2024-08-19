#pragma once
#include <cstddef>
#include <functional>

namespace fae
{
    struct window_resized;

    struct render_pipeline
    {
        std::function<std::size_t()> get_id;
        std::function<void(std::size_t)> prepare_render_pass;

        std::function<void(const fae::window_resized&)> on_window_resized;
    };
}
