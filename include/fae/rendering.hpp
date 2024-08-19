#pragma once

#include <concepts>
#include <type_traits>

#include "rendering/render_pipeline.hpp"
#include "rendering/renderer.hpp"
#include "rendering/webgpu_renderer.hpp"
#include "rendering/mesh.hpp"
#include "rendering/model.hpp"
#include "rendering/texture.hpp"
#include "rendering/material.hpp"

namespace fae
{
    struct application;
    struct resource_manager;
    struct asset_manager;
    struct scheduler;
    struct ecs_world;
    struct update_step;
    struct window_resized;

    struct render_step
    {
        resource_manager& resources;
        asset_manager& assets;
        scheduler& scheduler;
        ecs_world& ecs_world;
        render_pass& render_pass;
    };

    struct default_render_pipeline
    {
        render_pipeline render_pipeline;
    };

    struct visibility
    {
        bool visible = true;
    };

    struct rendering_plugin
    {
        auto init(application& app) const noexcept -> void;
    };

    auto update_rendering(const update_step& step) noexcept -> void;
    auto render_models(const render_step& step) noexcept -> void;
    auto resize_active_render_passes(const window_resized& e) noexcept -> void;
}
