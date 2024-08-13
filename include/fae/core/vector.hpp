#pragma once

#include <vector>
#include <cstdlib>

namespace fae
{
    template <typename t, typename t_alloc>
    [[nodiscard]] constexpr auto sizeof_data(const std::vector<t, t_alloc>& v) noexcept -> std::size_t
    {
        return v.size() * sizeof(typename std::vector<t, t_alloc>::value_type);
    }
}
