#pragma once

#include <chrono>
#include <format>
#include <iostream>
#include <print>
#include <source_location>
#include <string>
#include <string_view>

#ifndef FAE_PLATFORM_WEB
#include <stacktrace>
#else
#include <emscripten/emscripten.h>
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
struct std::formatter<fae::log_level>
{
    bool short_circuit = false;

    constexpr auto parse(auto& ctx)
    {
        auto it = ctx.begin();
        if (it == ctx.end())
            return it;
        if (*it == 's')
        {
            short_circuit = true;
            ++it;
        }
        return it;
    }

    auto format(const fae::log_level& level, auto& ctx) const
    {
        if (short_circuit)
        {
            return std::format_to(ctx.out(), "bleh");
        }

        std::string_view name;
        switch (level)
        {
        case fae::log_level::debug:
            name = "debug";
            break;
        case fae::log_level::info:
            name = "info";
            break;
        case fae::log_level::warning:
            name = "warning";
            break;
        case fae::log_level::error:
            name = "error";
            break;
        case fae::log_level::fatal:
            name = "fatal";
            break;
        default:
            name = "unknown";
            break;
        }
        return std::format_to(ctx.out(), "{}", name);
    }
};

namespace fae
{
    struct log_options
    {
        bool show_level = true;
        log_level level = log_level::debug;
        bool show_time = false;
        bool show_source_location = false;
        std::source_location source_location = std::source_location::current();
        bool show_stacktrace = false;
#ifndef FAE_PLATFORM_WEB
        std::stacktrace stacktrace = std::stacktrace::current();
#endif
    };

    template <std::formattable<char> t_log_arg>
    constexpr auto log(const t_log_arg& msg, const log_options& options = {}) noexcept -> void
    {
        if constexpr (config::is_release_build)
        {
            if (options.level == log_level::fatal)
            {
#ifdef FAE_PLATFORM_WEB
                emscripten_force_exit(exit_failure);
#endif
                std::exit(exit_failure);
            }
            return;
        }
        std::string result_msg;
        if (options.show_time)
        {
            result_msg += std::format("[{}] ", std::chrono::system_clock::now());
        }
        if (options.show_source_location)
        {
            result_msg += std::format("[{}:{}] ", options.source_location.file_name(), options.source_location.line());
        }
        if (options.show_level)
        {
            result_msg += std::format("[{}] ", options.level);
        }

        result_msg += std::format("{}", msg);

#ifndef FAE_PLATFORM_WEB
        if (options.show_stacktrace)
        {
            result_msg += std::format("\n{}", options.stacktrace);
        }
#endif

        std::println("{}", result_msg);

        if (options.level == log_level::fatal)
        {
#ifdef FAE_PLATFORM_WEB
            emscripten_force_exit(exit_failure);
#endif
            std::exit(exit_failure);
        }
    }

    template <typename t_log_arg>
    constexpr auto log_debug(const t_log_arg& msg, const log_options& options = {}) noexcept -> void
    {
        auto debug_options = options;
        debug_options.level = log_level::debug;
        log(msg, debug_options);
    }

    template <typename t_log_arg>
    constexpr auto log_info(const t_log_arg& msg, const log_options& options = {}) noexcept -> void
    {
        auto info_options = options;
        info_options.level = log_level::info;
        log(msg, info_options);
    }

    template <typename t_log_arg>
    constexpr auto log_warning(const t_log_arg& msg, const log_options& options = {}) noexcept -> void
    {
        auto warning_options = options;
        warning_options.level = log_level::warning;
        log(msg, warning_options);
    }

    template <typename t_log_arg>
    constexpr auto log_error(const t_log_arg& msg, const log_options& options = {}) noexcept -> void
    {
        auto error_options = options;
        error_options.level = log_level::error;
        log(msg, error_options);
    }

    template <typename t_log_arg>
    constexpr auto log_fatal(const t_log_arg& msg, const log_options& options = {}) noexcept -> void
    {
        auto error_options = options;
        error_options.level = log_level::fatal;
        log(msg, error_options);
    }
}
