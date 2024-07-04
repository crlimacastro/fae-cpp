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
        std::vector<std::size_t> indices;

        static auto load(std::filesystem::path path) -> std::optional<mesh>;

        constexpr auto has_indices() const noexcept -> bool
        {
            return !indices.empty();
        }
    };

    auto cube(float size) -> mesh;
}
