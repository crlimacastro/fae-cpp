#pragma once

namespace fae
{
    struct resource_manager;
    struct renderer;

    /* incomplete/deprecated for now */
    [[nodiscard]] auto make_sdl_renderer(resource_manager& resources) noexcept -> renderer;
}