#pragma once
#include <cstdint>
#include <vector>
#include <filesystem>

#include "fae/color.hpp"

namespace fae
{
    struct texture
    {
        std::size_t width;
        std::size_t height;
        std::vector<color> data;

        static auto load(std::filesystem::path path) -> std::optional<texture>;
    };
}
