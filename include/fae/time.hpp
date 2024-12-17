#pragma once

#include <limits>

#include "fae/duration.hpp"

namespace fae
{
    struct application;
    struct update_step;

    struct time
    {
        duration unscaled_delta{};
        duration unscaled_elapsed{};
        float scale = 1.f;

        [[nodiscard]] inline constexpr auto delta() const noexcept -> duration
        {
            return unscaled_delta * scale;
        }

        [[nodiscard]] inline constexpr auto elapsed() const noexcept -> duration
        {
            return unscaled_elapsed * scale;
        }

        [[nodiscard]] inline constexpr auto fps() const noexcept -> float
        {
            auto dt = unscaled_delta.seconds_f32();
            if (dt == 0.f)
            {
                return std::numeric_limits<float>::infinity();
            }
            return 1.f / dt;
        }
    };

    auto update_time(const update_step& step) noexcept -> void;

    struct time_plugin
    {
        auto init(application& app) const noexcept -> void;
    };
}
