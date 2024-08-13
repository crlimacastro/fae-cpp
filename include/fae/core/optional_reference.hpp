#pragma once

#include <optional>
#include <functional>
#include <utility>

namespace fae
{
  /*
  std::optional<t> cannot be templated on reference types (i.e. t&), for that use this optional_reference instead
  */
    template <typename t>
    struct optional_reference
    {
        constexpr optional_reference(std::optional<std::reference_wrapper<t>>&& some_ref) noexcept : option_ref(std::forward<std::optional<std::reference_wrapper<t>>>(some_ref)) {}
        constexpr optional_reference(std::nullopt_t null_opt) noexcept : option_ref(null_opt) {}
        [[nodiscard]] constexpr operator bool() const noexcept { return option_ref.has_value(); }
        constexpr auto operator*() noexcept -> t& { return (*option_ref).get(); }
        constexpr auto operator*() const noexcept -> const t& { return (*option_ref).get(); }
        constexpr auto operator->() noexcept -> t* { return &(*option_ref).get(); }
        constexpr auto operator->() const noexcept -> const t* { return &(*option_ref).get(); }

      private:
        std::optional<std::reference_wrapper<t>> option_ref{};
    };
}
