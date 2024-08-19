#pragma once
#include <cstddef>

namespace fae
{
    template <typename type_t, typename member_t>
    constexpr auto offset_of(const member_t type_t::*member) -> std::size_t
    {
        return reinterpret_cast<std::size_t>(&(((type_t*)nullptr)->*member));
    }
}
