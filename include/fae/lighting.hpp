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

    constexpr auto max_lights = 512;
    struct light_info
    {
        std::array<math::vec4, max_lights> colors;
        std::uint32_t count = 0;
        std::uint32_t padding0 = 0;
        std::uint32_t padding1 = 0;
        std::uint32_t padding2 = 0;

        auto clear() noexcept -> void
        {
            colors.fill({ 0.f, 0.f, 0.f, 0.f });
            count = 0;
        }
    };

    struct ambient_light_info
    {
        light_info lights;

        auto clear() noexcept -> void
        {
            lights.clear();
        }
    };
    static_assert(sizeof(ambient_light_info) % 16 == 0, "uniform buffer must be aligned on 16 bytes");

    struct directional_light_info
    {
        std::array<math::vec4, max_lights> directions;
        light_info lights;

        auto clear() noexcept -> void
        {
            directions.fill({ 0.f, 0.f, 0.f, 0.f });
            lights.clear();
        }
    };
    static_assert(sizeof(directional_light_info) % 16 == 0, "uniform buffer must be aligned on 16 bytes");

    struct lighting_plugin
    {
        auto init(application& app) const noexcept -> void;
    };

    auto update_lighting(const update_step& step) noexcept -> void;
}
