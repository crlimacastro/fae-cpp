#include "fae/rendering/texture.hpp"
#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>

#include "fae/logging.hpp"

namespace fae
{
    auto texture::load(std::filesystem::path path) -> std::optional<texture>
    {
        int width, height, channels;
        auto *img_data = stbi_load(path.string().c_str(), &width, &height, &channels, 0);
        if (!img_data)
        {
            fae::log_error(std::format("Failed to load texture {}, {}", path.string(), stbi_failure_reason()));
            return std::nullopt;
        }

        auto data = std::vector<color>(width * height);
        for (int i = 0; i < width * height; ++i)
        {
            data[i] = color{
                .r = img_data[i * channels + 0],
                .g = img_data[i * channels + 1],
                .b = img_data[i * channels + 2],
                .a = channels == 4 ? img_data[i * channels + 3] : static_cast<std::uint8_t>(255),
            };
        }

        stbi_image_free(img_data);

        return texture{
            .width = static_cast<std::size_t>(width),
            .height = static_cast<std::size_t>(height),
            .data = std::move(data),
        };
    }
}
