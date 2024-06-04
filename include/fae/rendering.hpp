#pragma once

#include "fae/application.hpp"

#include "rendering/renderer.hpp"
#include "rendering/sdl_renderer.hpp"
#include "rendering/webgpu_renderer.hpp"

namespace fae
{

    struct render_step
    {
        resource_manager& resources;
        scheduler& scheduler;
        ecs_world& ecs_world;
    };

    auto update_rendering(const update_step& step) noexcept -> void;

    struct rendering_plugin
    {
        auto init(application& app) const noexcept -> void;
    };
}
