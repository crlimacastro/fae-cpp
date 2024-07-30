#pragma once

#include "fae/rendering/mesh.hpp"
#include "fae/rendering/material.hpp"

namespace fae
{
    struct model
    {
        fae::mesh mesh;
        fae::material material = fae::material
        {
            .diffuse = fae::texture
            {
                .width = 1,
                .height = 1,
                .data = { fae::color{ 255, 255, 255, 255 } },
            },
        };
    };
}
