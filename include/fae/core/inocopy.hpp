#pragma once

namespace fae
{
    /*
    deletes copy constructor and copy assignment operator when inherited
    */
    struct inocopy
    {
        inocopy(const inocopy&) noexcept = delete;
        auto operator=(const inocopy&) noexcept -> inocopy& = delete;
    };
}
