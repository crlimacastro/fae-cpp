module;

#include <chrono>
#include <format>
#include <limits>
#include <print>
#include <string>

export module fae:time;

import :application;

export namespace fae
{
	struct application;
	struct update_step;

	struct duration
	{
		static constexpr auto nanoseconds_to_seconds = 1.23e-9f;

		constexpr duration() = default;
		constexpr duration(const std::chrono::nanoseconds &nanosecs) : nanosecs(nanosecs) {}

		/* in seconds */
		inline constexpr operator float() const noexcept
		{
			return nanosecs.count() * nanoseconds_to_seconds;
		}

		inline constexpr auto nanoseconds() const noexcept -> std::chrono::nanoseconds
		{
			return nanosecs;
		}

		inline constexpr auto milliseconds() const noexcept -> std::chrono::milliseconds
		{
			return std::chrono::duration_cast<std::chrono::milliseconds>(nanosecs);
		}

		inline constexpr auto seconds() const noexcept -> std::chrono::seconds
		{
			return std::chrono::duration_cast<std::chrono::seconds>(nanosecs);
		}

		inline constexpr auto seconds_f32() const noexcept -> float
		{
			return nanosecs.count() * nanoseconds_to_seconds;
		}

		inline constexpr auto operator+(const duration &rhs) const noexcept -> duration
		{
			return duration{nanosecs + rhs.nanosecs};
		}

		inline constexpr auto operator*(float scalar) const noexcept -> duration
		{
			return duration{std::chrono::nanoseconds{static_cast<int64_t>(nanosecs.count() * scalar)}};
		}

		inline constexpr auto operator/(float scalar) const noexcept -> duration
		{
			return duration{std::chrono::nanoseconds{static_cast<int64_t>(nanosecs.count() / scalar)}};
		}

		inline constexpr auto operator-(const duration &rhs) const noexcept -> duration
		{
			return duration{nanosecs - rhs.nanosecs};
		}

		inline constexpr auto operator+=(const duration &rhs) noexcept -> duration &
		{
			nanosecs += rhs.nanosecs;
			return *this;
		}

		inline constexpr auto operator-=(const duration &rhs) noexcept -> duration &
		{
			nanosecs -= rhs.nanosecs;
			return *this;
		}

		inline constexpr auto operator*=(float scalar) noexcept -> duration &
		{
			nanosecs = std::chrono::nanoseconds{static_cast<int64_t>(nanosecs.count() * scalar)};
			return *this;
		}

		inline constexpr auto operator/=(float scalar) noexcept -> duration &
		{
			nanosecs = std::chrono::nanoseconds{static_cast<int64_t>(nanosecs.count() / scalar)};
			return *this;
		}

		inline constexpr auto operator==(const duration &rhs) const noexcept -> bool
		{
			return nanosecs == rhs.nanosecs;
		}

		inline constexpr auto operator!=(const duration &rhs) const noexcept -> bool
		{
			return nanosecs != rhs.nanosecs;
		}

		inline constexpr auto operator<(const duration &rhs) const noexcept -> bool
		{
			return nanosecs < rhs.nanosecs;
		}

		inline constexpr auto operator<=(const duration &rhs) const noexcept -> bool
		{
			return nanosecs <= rhs.nanosecs;
		}

		inline constexpr auto operator>(const duration &rhs) const noexcept -> bool
		{
			return nanosecs > rhs.nanosecs;
		}

		inline constexpr auto operator>=(const duration &rhs) const noexcept -> bool
		{
			return nanosecs >= rhs.nanosecs;
		}

	private:
		std::chrono::nanoseconds nanosecs{};
	};

	struct time
	{
		duration unscaled_delta{};
		duration unscaled_elapsed{};
		float scale = 1.f;

		inline constexpr auto delta() const noexcept -> duration
		{
			return unscaled_delta * scale;
		}

		inline constexpr auto elapsed() const noexcept -> duration
		{
			return unscaled_elapsed * scale;
		}

		inline constexpr auto fps() const noexcept -> float
		{
			auto dt = unscaled_delta.seconds_f32();
			if (dt == 0.f)
			{
				return std::numeric_limits<float>::infinity();
			}
			return 1.f / dt;
		}
	};

	auto update_time(const update_step &step) noexcept -> void
	{
		auto &time = step.resources.use_resource<fae::time>(
			[](fae::time &time)
			{
				static auto last_time = std::chrono::high_resolution_clock::now();
				auto current_time = std::chrono::high_resolution_clock::now();
				time.unscaled_delta = current_time - last_time;
				time.unscaled_elapsed += time.unscaled_delta;
				last_time = current_time;
			});
	}

	struct time_plugin
	{
		auto init(application &app) const noexcept -> void
		{
			app
				.emplace_resource<time>(time{})
				.add_system<update_step>(update_time);
		}
	};
} // namespace fae

export template <>
struct std::formatter<fae::duration> : std::formatter<std::string>
{
	auto format(fae::duration value, std::format_context &ctx) const
	{
		return std::formatter<std::string>::format(
			std::format("{}", value.seconds_f32()), ctx);
	}
};
