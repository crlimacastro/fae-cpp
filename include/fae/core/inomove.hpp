#pragma once

namespace fae
{
    /*
    deletes move constructor and move assignment operator when inherited
    */
    struct inomove
    {
        inomove(inomove&&) noexcept = delete;
        auto operator=(inomove&&) noexcept -> inomove& = delete;
    };
}
