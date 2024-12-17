#include "fae/color.hpp"

#include <cmath>

namespace fae
{
    auto color_rgba::from_hsva(const color_hsva& color) noexcept -> color_rgba
    {
        return color.to_rgba();
    }

    auto color_rgba::to_hsva() const noexcept -> color_hsva
    {
        return color_hsva::from_rgba(*this);
    }

    auto color_hsva::from_rgba(const color_rgba& color) noexcept -> color_hsva
    {
        float r_norm = color.r / 255.f;
        float g_norm = color.g / 255.f;
        float b_norm = color.b / 255.f;

        float max = std::max(std::max(r_norm, g_norm), b_norm);
        float min = std::min(std::min(r_norm, g_norm), b_norm);
        float delta = max - min;

        float h = 0.f, s = 0.f, v = 0.f;
        if (delta > 0)
        {
            if (max == r_norm)
            {
                h = 60 * (std::fmod(((g_norm - b_norm) / delta), 6));
            }
            else if (max == g_norm)
            {
                h = 60 * (((b_norm - r_norm) / delta) + 2);
            }
            else if (max == b_norm)
            {
                h = 60 * (((r_norm - g_norm) / delta) + 4);
            }

            if (max > 0)
            {
                s = delta / max;
            }
            else
            {
                s = 0;
            }

            v = max;
        }
        else
        {
            h = 0;
            s = 0;
            v = max;
        }

        if (h < 0)
        {
            h = 360 + h;
        }

        return color_hsva{ .h = h, .s = s, .v = v, .a = color.a };
    }

    auto color_hsva::to_rgba() const noexcept -> color_rgba
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

        return color_rgba{ .r = r, .g = g, .b = b, .a = a };
    }
}
