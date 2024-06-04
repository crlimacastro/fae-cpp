#pragma once

namespace fae
{
    struct resource_manager;
    struct renderer;

    [[nodiscard]] auto make_webgpu_renderer(resource_manager& resources) noexcept -> renderer;
}
