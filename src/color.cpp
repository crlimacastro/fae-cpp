module;
#include <cstdint>
#include <cmath>
#include <algorithm>

export module fae:color;

export namespace fae
{
    struct color_rgba
    {
        std::uint8_t r = 0;
        std::uint8_t g = 0;
        std::uint8_t b = 0;
        std::uint8_t a = 255;
    };
    using color = color_rgba;

    struct color_hsva
    {
        float h = 0.f;
        float s = 0.f;
        float v = 0.f;
        std::uint8_t a = 255;

        // TODO fix
        static inline constexpr auto from_rgba(const color_rgba &color) noexcept -> color_hsva
        {
            float fc_max = std::max(std::max(color.r, color.g), color.b);
            float fc_min = std::min(std::min(color.r, color.g), color.b);
            float f_delta = fc_max - fc_min;

            float h = 0.f, s = 0.f, v = 0.f;
            if (f_delta > 0)
            {
                if (fc_max == color.r)
                {
                    h = 60 * (std::fmod(((color.g - color.b) / f_delta), 6));
                }
                else if (fc_max == color.g)
                {
                    h = 60 * (((color.b - color.r) / f_delta) + 2);
                }
                else if (fc_max == color.b)
                {
                    h = 60 * (((color.r - color.g) / f_delta) + 4);
                }

                if (fc_max > 0)
                {
                    s = f_delta / fc_max;
                }
                else
                {
                    s = 0;
                }

                v = fc_max;
            }
            else
            {
                h = 0;
                s = 0;
                v = fc_max;
            }

            if (h < 0)
            {
                h = 360 + h;
            }

            return color_hsva{.h = h, .s = s, .v = v, .a = color.a};
        }

        // TODO fix
        inline constexpr auto to_rgba() const noexcept -> color_rgba
        {
            float fc = v * s;
            float h_prime = std::fmod(h / 60.0, 6);
            float fX = fc * (1 - std::fabs(std::fmod(h_prime, 2) - 1));
            float fM = v - fc;

            float r_norm = 0, g_norm = 0, b_norm = 0;
            if (0 <= h_prime && h_prime < 1)
            {
                r_norm = fc;
                g_norm = fX;
                b_norm = 0;
            }
            else if (1 <= h_prime && h_prime < 2)
            {
                r_norm = fX;
                g_norm = fc;
                b_norm = 0;
            }
            else if (2 <= h_prime && h_prime < 3)
            {
                r_norm = 0;
                g_norm = fc;
                b_norm = fX;
            }
            else if (3 <= h_prime && h_prime < 4)
            {
                r_norm = 0;
                g_norm = fX;
                b_norm = fc;
            }
            else if (4 <= h_prime && h_prime < 5)
            {
                r_norm = fX;
                g_norm = 0;
                b_norm = fc;
            }
            else if (5 <= h_prime && h_prime < 6)
            {
                r_norm = fc;
                g_norm = 0;
                b_norm = fX;
            }
            else
            {
                r_norm = 0;
                g_norm = 0;
                b_norm = 0;
            }

            r_norm += fM;
            g_norm += fM;
            b_norm += fM;

            auto r = static_cast<std::uint8_t>(r_norm * 255.f);
            auto g = static_cast<std::uint8_t>(g_norm * 255.f);
            auto b = static_cast<std::uint8_t>(b_norm * 255.f);

            return color_rgba{
                .r = r,
                .g = g,
                .b = b,
                .a = a};
        }
    };

    namespace colors
    {
        constexpr auto black = color{0, 0, 0, 255};
        constexpr auto white = color{255, 255, 255, 255};
        constexpr auto red = color{255, 0, 0, 255};
        constexpr auto green = color{0, 255, 0, 255};
        constexpr auto blue = color{0, 0, 255, 255};
        constexpr auto cornflower_blue = color{100, 149, 237, 255};
    }
}
