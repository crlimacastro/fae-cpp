#pragma once

#include <webgpu/webgpu_cpp.h>

#include "fae/application/application_step.hpp"
#include "fae/math.hpp"
#include "fae/webgpu/webgpu.hpp"

namespace fae
{
    struct window_resized;
    struct entity_commands;
    struct asset_manager;

    struct global_uniforms_t
    {
        vec3 camera_world_position = { 0.f, 0.f, 0.f };
        float time = 0;
    };
    static_assert(sizeof(global_uniforms_t) % 16 == 0, "uniform buffer must be aligned on 16 bytes");

    struct local_uniforms_t
    {
        mat4 model = mat4(1.f);
        mat4 view = mat4(1.f);
        mat4 projection = mat4(1.f);
        vec4 tint = { 1.f, 1.f, 1.f, 1.f };
    };
    static_assert(sizeof(local_uniforms_t) % 16 == 0, "uniform buffer must be aligned on 16 bytes");

    [[nodiscard]] auto create_default_render_pipeline(ecs_world& ecs_world, entity_commands& global_entity, asset_manager& assets) noexcept -> render_pipeline;

    struct webgpu_default_render_pipeline
    {
        // wgpu::ShaderModule shader_module;
        // wgpu::RenderPipeline pipeline;
        // wgpu::Texture depth_texture;
        // std::uint32_t uniform_stride;

        // auto create_buffers() -> void;
        // auto update_uniforms() -> void;
        // auto write_buffers() -> void;
    };
}
