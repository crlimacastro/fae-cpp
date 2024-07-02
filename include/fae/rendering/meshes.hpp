#pragma once

#include <vector>
#include <filesystem>

#include "fae/math.hpp"

namespace fae
{
    struct vertex
    {
        vec3 position;
        vec4 color;
        vec3 normal;
        vec2 uv;
    };

    struct mesh
    {
        std::vector<vertex> vertices;

        static auto load(std::filesystem::path path) -> std::optional<mesh>;
    };

    auto cube(float size) -> mesh;
}
