#pragma once

#include <string>
#include <string_view>
#include <type_traits>

#include <magic_enum.hpp>

namespace fae
{
    template <typename t>
        requires std::is_enum_v<t>
    [[nodiscard]] inline constexpr auto to_string(const t& value) -> std::string
    {
        return std::string(magic_enum::enum_name(value));
    }

    template <typename t>
        requires std::is_enum_v<t>
    [[nodiscard]] inline constexpr auto to_string_view(const t& value) -> std::string_view
    {
        return magic_enum::enum_name(value);
    }
}
