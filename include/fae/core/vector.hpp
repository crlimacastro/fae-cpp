#pragma once

#include <vector>
#include <cstdlib>

namespace fae
{
    template <typename t_container>
    [[nodiscard]] constexpr auto sizeof_data(const t_container& v) noexcept -> std::size_t
    {
        return v.size() * sizeof(typename t_container::value_type);
    }
}
