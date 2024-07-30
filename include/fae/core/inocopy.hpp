#pragma once

namespace fae
{
    /*
    deletes copy constructor and copy assignment operator when inherited
    */
    struct inocopy
    {
        inocopy(const inocopy&) = delete;
        auto operator=(const inocopy&) -> inocopy& = delete;
    };
}
