#pragma once

#include <webgpu/webgpu_cpp.h>

struct SDL_Window;

namespace fae
{
    [[nodiscard]] auto get_sdl_webgpu_surface(wgpu::Instance instance, SDL_Window* window) noexcept -> wgpu::Surface;
}
