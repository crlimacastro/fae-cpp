#pragma once

#include <concepts>
#include <type_traits>

#include "fae/application.hpp"

#include "rendering/renderer.hpp"
#include "rendering/sdl_renderer.hpp"
#include "rendering/webgpu_renderer.hpp"
#include "rendering/mesh.hpp"
#include "rendering/model.hpp"
#include "rendering/texture.hpp"
#include "rendering/material.hpp"

namespace fae
{
    // template <typename pipeline_t>
    // concept rendering_pipeline = requires(pipeline_t pipeline) {
    //     { pipeline.create_shader_program() } -> std::same_as<void>;
    //     { pipeline.update_uniforms() } -> std::same_as<void>;
    //     { pipeline.create_buffers() } -> std::same_as<void>;
    // };

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
