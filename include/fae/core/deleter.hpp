#pragma once

namespace fae
{
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
}
