#pragma once
#include "fae/rendering/texture.hpp"

namespace fae
{
    struct material
    {
        texture diffuse = texture
        {
            .width = 1,
            .height = 1,
            .data = { colors::white },
        };
        // texture normal;
        // texture metallic;
        // texture roughness;
        // texture ambient_occlusion;
        // texture emissive;
    };
}
