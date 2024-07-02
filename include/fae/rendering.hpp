#pragma once

#include "fae/application.hpp"

#include "rendering/renderer.hpp"
#include "rendering/sdl_renderer.hpp"
#include "rendering/webgpu_renderer.hpp"
#include "rendering/meshes.hpp"

namespace fae
{
    struct render_step
    {
        resource_manager& resources;
        asset_manager& assets;
        scheduler& scheduler;
        ecs_world& ecs_world;
    };

    struct visibility
    {
        bool visible = true;
    };

    auto update_rendering(const update_step& step) noexcept -> void;
    auto render_models(const render_step& step) noexcept -> void;

    struct rendering_plugin
    {
        auto init(application& app) const noexcept -> void;
    };
}
