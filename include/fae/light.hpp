#pragma once

#include "fae/math.hpp"
#include "fae/color.hpp"

namespace fae
{
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
}
