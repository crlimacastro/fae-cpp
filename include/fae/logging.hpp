#pragma once

#include <format>
#include <source_location>
#ifdef FAE_PLATFORM_WEB
#include <emscripten/emscripten.h>
#else
#include <chrono>
#include <print>
#include <stacktrace>
#include <string>
#endif

#include "fae/config.hpp"
#include "fae/core.hpp"

namespace fae
{
	enum struct log_level
	{
		debug,
		info,
		warning,
		error,
		fatal,
	};
}

template <>
struct std::formatter<fae::log_level> : std::formatter<std::string>
{
	auto format(fae::log_level value, std::format_context &ctx) const
	{
		std::string log_level_name = "";
		switch (value)
		{
		case fae::log_level::debug:
			log_level_name += "debug";
			break;
		case fae::log_level::info:
			log_level_name += "info";
			break;
		case fae::log_level::warning:
			log_level_name += "warning";
			break;
		case fae::log_level::error:
			log_level_name += "error";
			break;
		case fae::log_level::fatal:
			log_level_name += "fatal";
			break;
		default:
			log_level_name += "unknown";
			break;
		}
		return std::formatter<std::string>::format(
			std::format("{}", log_level_name), ctx);
	}
};

namespace fae
{
	struct log_options
	{
		bool show_time = false;
		bool show_source_location = false;
		std::source_location source_location = std::source_location::current();
		bool show_level = true;
		log_level level = log_level::debug;
		bool show_stacktrace = false;
#ifndef FAE_PLATFORM_WEB
		std::stacktrace stacktrace = std::stacktrace::current();
#endif
	};

	template <std::formattable<char> t_log_arg>
#ifndef FAE_PLATFORM_WEB
	constexpr
#endif
		auto
		log(const t_log_arg &msg, const log_options &options = {}) noexcept -> void
	{
#ifdef FAE_PLATFORM_WEB
		EM_ASM(console.log(msg));
		return;
#else
		if constexpr (config::is_release_build)
		{
			if (options.level == log_level::fatal)
			{
				std::exit(exit_failure);
			}
			return;
		}
		std::ostringstream result_msg;
		if (options.show_time)
		{
			result_msg << std::format("[{}] ", std::chrono::system_clock::now());
		}
		if (options.show_source_location)
		{
			result_msg << std::format("[{}:{}] ", options.source_location.file_name(), options.source_location.line());
		}
		if (options.show_level)
		{
			result_msg << std::format("[{}] ", options.level);
		}

		result_msg << std::format("{}", msg) << std::endl;

#ifndef FAE_PLATFORM_WEB
		if (options.show_stacktrace)
		{
			result_msg << std::format("{}", options.stacktrace) << std::endl;
		}
#endif

		std::print("{}", result_msg.str());

		if (options.level == log_level::fatal)
		{
			std::exit(EXIT_FAILURE);
		}
#endif
	}

	template <typename t_log_arg>
	constexpr auto log_debug(const t_log_arg &msg, const log_options &options = {}) noexcept -> void
	{
		auto debug_options = options;
		debug_options.level = log_level::debug;
		log(msg, debug_options);
	}

	template <typename t_log_arg>
	constexpr auto log_info(const t_log_arg &msg, const log_options &options = {}) noexcept -> void
	{
		auto info_options = options;
		info_options.level = log_level::info;
		log(msg, info_options);
	}

	template <typename t_log_arg>
	constexpr auto log_warning(const t_log_arg &msg, const log_options &options = {}) noexcept -> void
	{
		auto warning_options = options;
		warning_options.level = log_level::warning;
		log(msg, warning_options);
	}

	template <typename t_log_arg>
	constexpr auto log_error(const t_log_arg &msg, const log_options &options = {}) noexcept -> void
	{
		auto error_options = options;
		error_options.level = log_level::error;
		log(msg, error_options);
	}

	template <typename t_log_arg>
	constexpr auto log_fatal(const t_log_arg &msg, const log_options &options = {}) noexcept -> void
	{
		auto error_options = options;
		error_options.level = log_level::fatal;
		log(msg, error_options);
	}
} // namespace fae
