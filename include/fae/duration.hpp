#pragma once

#include <chrono>
#include <format>
#include <iostream>
#include <limits>

namespace fae
{
    struct duration
    {
        static constexpr auto nanoseconds_to_seconds = 1e-9f;

        constexpr duration() = default;
        constexpr duration(const std::chrono::nanoseconds& nanosecs) : nanosecs(nanosecs) {}

        /* in seconds */
        [[nodiscard]] inline constexpr operator float() const noexcept
        {
            return nanosecs.count() * nanoseconds_to_seconds;
        }

        [[nodiscard]] inline constexpr auto nanoseconds() const noexcept -> std::chrono::nanoseconds
        {
            return nanosecs;
        }

        [[nodiscard]] inline constexpr auto milliseconds() const noexcept -> std::chrono::milliseconds
        {
            return std::chrono::duration_cast<std::chrono::milliseconds>(nanosecs);
        }

        [[nodiscard]] inline constexpr auto seconds() const noexcept -> std::chrono::seconds
        {
            return std::chrono::duration_cast<std::chrono::seconds>(nanosecs);
        }

        [[nodiscard]] inline constexpr auto seconds_f32() const noexcept -> float
        {
            return nanosecs.count() * nanoseconds_to_seconds;
        }

        [[nodiscard]] inline constexpr auto operator+(const duration& rhs) const noexcept -> duration
        {
            return duration{nanosecs + rhs.nanosecs};
        }

        [[nodiscard]] inline constexpr auto operator*(float scalar) const noexcept -> duration
        {
            return duration{std::chrono::nanoseconds{static_cast<std::int64_t>(nanosecs.count() * scalar)}};
        }

        [[nodiscard]] inline constexpr auto operator/(float scalar) const noexcept -> duration
        {
            return duration{std::chrono::nanoseconds{static_cast<std::int64_t>(nanosecs.count() / scalar)}};
        }

        [[nodiscard]] inline constexpr auto operator-(const duration& rhs) const noexcept -> duration
        {
            return duration{nanosecs - rhs.nanosecs};
        }

        [[maybe_unused]] inline constexpr auto operator+=(const duration& rhs) noexcept -> duration&
        {
            nanosecs += rhs.nanosecs;
            return *this;
        }

        [[maybe_unused]] inline constexpr auto operator-=(const duration& rhs) noexcept -> duration&
        {
            nanosecs -= rhs.nanosecs;
            return *this;
        }

        [[maybe_unused]] inline constexpr auto operator*=(float scalar) noexcept -> duration&
        {
            nanosecs = std::chrono::nanoseconds{static_cast<std::int64_t>(nanosecs.count() * scalar)};
            return *this;
        }

        [[maybe_unused]] inline constexpr auto operator/=(float scalar) noexcept -> duration&
        {
            nanosecs = std::chrono::nanoseconds{static_cast<std::int64_t>(nanosecs.count() / scalar)};
            return *this;
        }

        [[nodiscard]] inline constexpr auto operator==(const duration& rhs) const noexcept -> bool
        {
            return nanosecs == rhs.nanosecs;
        }

        [[nodiscard]] inline constexpr auto operator!=(const duration& rhs) const noexcept -> bool
        {
            return nanosecs != rhs.nanosecs;
        }

        [[nodiscard]] inline constexpr auto operator<(const duration& rhs) const noexcept -> bool
        {
            return nanosecs < rhs.nanosecs;
        }

        [[nodiscard]] inline constexpr auto operator<=(const duration& rhs) const noexcept -> bool
        {
            return nanosecs <= rhs.nanosecs;
        }

        [[nodiscard]] inline constexpr auto operator>(const duration& rhs) const noexcept -> bool
        {
            return nanosecs > rhs.nanosecs;
        }

        [[nodiscard]] inline constexpr auto operator>=(const duration& rhs) const noexcept -> bool
        {
            return nanosecs >= rhs.nanosecs;
        }

      private:
        std::chrono::nanoseconds nanosecs{};
    };
}

template <>
struct std::formatter<fae::duration> : std::formatter<std::string>
{
    auto format(fae::duration value, std::format_context& ctx) const
    {
        return std::formatter<std::string>::format(
            std::format("{}", value.seconds_f32()), ctx);
    }
};
