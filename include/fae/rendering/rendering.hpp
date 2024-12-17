#pragma once

#include <concepts>
#include <type_traits>

#include "material.hpp"
#include "mesh.hpp"
#include "model.hpp"
#include "render_pass.hpp"
#include "render_pipeline.hpp"
#include "renderer.hpp"
#include "texture.hpp"
#include "webgpu_renderer.hpp"

namespace fae
{
    struct application;
    struct entity_commands;
    struct asset_manager;
    struct scheduler;
    struct ecs_world;
    struct update_step;
    struct window_resized;

    struct render_step
    {
        entity_commands& global_entity;
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
