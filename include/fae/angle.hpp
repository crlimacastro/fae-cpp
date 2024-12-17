#pragma once

#include <numbers>

namespace fae
{
    struct angle
    {
        [[nodiscard]] inline constexpr auto radians() const noexcept -> float
        {
            return m_radians;
        }

        [[nodiscard]] inline constexpr auto degrees() const noexcept -> float
        {
            return m_radians * 180.0f / std::numbers::pi;
        }

        [[nodiscard]] static inline constexpr auto from_radians(const float radians) noexcept -> angle
        {
            return angle(radians);
        }

        [[nodiscard]] static inline constexpr auto from_degrees(const float degrees) noexcept -> angle
        {
            return angle(degrees * std::numbers::pi / 180.0f);
        }

        [[nodiscard]] static inline constexpr auto zero() noexcept -> angle
        {
            return angle(0.0f);
        }

        [[nodiscard]] static inline constexpr auto quarter() noexcept -> angle
        {
            return angle(std::numbers::pi / 2.0f);
        }

        [[nodiscard]] static inline constexpr auto half() noexcept -> angle
        {
            return angle(std::numbers::pi);
        }

        [[nodiscard]] static inline constexpr auto full() noexcept -> angle
        {
            return angle(2.0f * std::numbers::pi);
        }

        [[nodiscard]] inline constexpr auto operator-() const noexcept -> angle
        {
            return angle(-m_radians);
        }

        [[nodiscard]] inline constexpr auto operator+(const angle other) const noexcept -> angle
        {
            return angle(m_radians + other.m_radians);
        }

        [[nodiscard]] inline constexpr auto operator-(const angle other) const noexcept -> angle
        {
            return angle(m_radians - other.m_radians);
        }

        [[nodiscard]] inline constexpr auto operator*(const float scalar) const noexcept -> angle
        {
            return angle(m_radians * scalar);
        }

        [[nodiscard]] inline constexpr auto operator/(const float scalar) const noexcept -> angle
        {
            return angle(m_radians / scalar);
        }

        [[nodiscard]] inline constexpr auto operator+=(const angle other) noexcept -> angle&
        {
            m_radians += other.m_radians;
            return *this;
        }

        [[nodiscard]] inline constexpr auto operator-=(const angle other) noexcept -> angle&
        {
            m_radians -= other.m_radians;
            return *this;
        }

        [[nodiscard]] inline constexpr auto operator*=(const float scalar) noexcept -> angle&
        {
            m_radians *= scalar;
            return *this;
        }

        [[nodiscard]] inline constexpr auto operator/=(const float scalar) noexcept -> angle&
        {
            m_radians /= scalar;
            return *this;
        }

        [[nodiscard]] inline constexpr auto operator==(const angle other) const noexcept -> bool
        {
            return m_radians == other.m_radians;
        }

        [[nodiscard]] inline constexpr auto operator!=(const angle other) const noexcept -> bool
        {
            return m_radians != other.m_radians;
        }

        [[nodiscard]] inline constexpr auto operator<=>(const angle other) const noexcept
        {
            return m_radians <=> other.m_radians;
        }

      protected:
        constexpr angle(float radians) noexcept : m_radians(radians) {}

        float m_radians;
    };

    struct unit_angle : angle
    {
        constexpr unit_angle(angle angle) noexcept : fae::angle(angle.radians())
        {
            auto radians = angle.radians();
            while (radians < 0.0f)
            {
                radians += 2.0f * std::numbers::pi;
            }
            while (radians >= 2.0f * std::numbers::pi)
            {
                radians -= 2.0f * std::numbers::pi;
            }
            m_radians = radians;
        }

        [[nodiscard]] static inline constexpr auto from_radians(const float radians) noexcept -> unit_angle
        {
            auto angle = angle::from_radians(radians);
            return unit_angle(angle);
        }

        [[nodiscard]] static inline constexpr auto from_degrees(const float degrees) noexcept -> unit_angle
        {
            auto angle = angle::from_degrees(degrees);
            return unit_angle(angle);
        }
    };
    using uangle = unit_angle;
    using norm_angle = unit_angle;
}
