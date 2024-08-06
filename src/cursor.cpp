#include "fae/cursor.hpp"

#include <SDL3/SDL.h>
#include <imgui.h>

namespace fae
{
    auto show_cursor() noexcept -> void
    {
        SDL_ShowCursor();
    }

    auto hide_cursor() noexcept -> void
    {
        SDL_HideCursor();
    }
}
