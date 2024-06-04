#pragma once

#include <cstdint>
#include <functional>
#include <optional>
#include <variant>
#include <vector>

namespace fae
{
    constexpr int exit_success = 0;
    constexpr int exit_failure = 1;

    /*
    deletes copy constructor and copy assignment operator when inherited
    */
    struct inocopy
    {
        inocopy(const inocopy&) = delete;
        auto operator=(const inocopy&) -> inocopy& = delete;
    };

    /*
    deletes move constructor and move assignment operator when inherited
    */
    struct inomove
    {
        inomove(inomove&&) noexcept = delete;
        auto operator=(inomove&&) noexcept -> inomove& = delete;
    };

    /*
    define a deleter<T> anywhere and include it to use a smart pointer with a custom deleter functor
    (delete functions should not throw any exceptions)
    e.g.
    template<>
    struct deleter<T> {
            void operator()(T* ptr) noexcept {
                    free(ptr);
            }
    };

    std::unique_ptr<T, fae::deleter<T>>
    */
    template <typename t>
    struct deleter
    {
        auto operator()(t*) noexcept -> void = 0;
    };

    template <typename t>
    struct optional_reference
    {
        optional_reference(std::optional<std::reference_wrapper<t>> some_ref) noexcept : option_ref(std::move(some_ref)) {}
        optional_reference(std::nullopt_t null_opt) noexcept : option_ref(null_opt) {}
        operator bool() const noexcept { return option_ref.has_value(); }
        auto operator*() noexcept -> t& { return (*option_ref).get(); }
        auto operator*() const noexcept -> const t& { return (*option_ref).get(); }
        auto operator->() noexcept -> t* { return &(*option_ref).get(); }
        auto operator->() const noexcept -> const t* { return &(*option_ref).get(); }

      private:
        std::optional<std::reference_wrapper<t>> option_ref{};
    };

    template <typename... ts>
    struct match_arms : ts...
    {
        using ts::operator()...;
    };

    /*
            type matching syntax for std::variants
            e.g.
            std::variant<int, float> my_variant;
            fae::match(
                    my_variant,
                    [&](int value) {
                            // do something if int type value
                    },
                    [&](float value) {
                            // do something if float type value
                    },
                    [&](auto value) {
                            // do something generic for all other types
                    });
            */
    template <typename... ts, typename... tarms>
    auto match(std::variant<ts...> variant, tarms&&... arms)
    {
        return std::visit(match_arms{std::forward<tarms>(arms)...}, variant);
    }

    template <typename t, typename t_alloc>
    [[nodiscard]] inline constexpr auto sizeof_data(const std::vector<t, t_alloc>& v) -> std::size_t
    {
        return v.size() * sizeof(typename std::vector<t, t_alloc>::value_type);
    }
}
