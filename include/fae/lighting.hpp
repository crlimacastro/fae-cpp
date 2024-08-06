#pragma once

#include "fae/math.hpp"
#include "fae/color.hpp"

namespace fae
{
    struct application;
    struct update_step;

    struct ambient_light
    {
        color color = colors::white;
    };

    struct directional_light
    {
        vec3 direction = { 0.f, -1.f, 0.f };
        color color = colors::white;
    };

    struct point_light
    {
        vec3 position = { 0.f, 0.f, 0.f };
        float intensity = 1.f;
        color color = colors::white;
    };

    constexpr auto MAX_LIGHTS = 32;
    struct ambient_light_info
    {
        std::array<math::vec4, MAX_LIGHTS> colors;

        auto clear() noexcept -> void
        {
            colors.fill({ 0.f, 0.f, 0.f, 0.f });
        }
    };
    static_assert(sizeof(ambient_light_info) % 16 == 0, "uniform buffer must be aligned on 16 bytes");

    struct directional_light_info
    {
        std::array<math::vec4, MAX_LIGHTS> directions;
        std::array<math::vec4, MAX_LIGHTS> colors;

        auto clear() noexcept -> void
        {
            directions.fill({ 0.f, 0.f, 0.f, 0.f });
            colors.fill({ 0.f, 0.f, 0.f, 0.f });
        }
    };
    static_assert(sizeof(directional_light_info) % 16 == 0, "uniform buffer must be aligned on 16 bytes");

    struct lighting_plugin
    {
        auto init(application& app) const noexcept -> void;
    };

    auto update_lighting(const update_step& step) noexcept -> void;
}
