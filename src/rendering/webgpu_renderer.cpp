#include "fae/rendering/webgpu_renderer.hpp"

#include <cstdint>

#include "fae/rendering/renderer.hpp"
#include "fae/resource_manager.hpp"
#include "fae/math.hpp"
#include "fae/time.hpp"
#include "fae/webgpu.hpp"
#include "fae/color.hpp"
#include "fae/rendering/renderer.hpp"
#include "fae/rendering/mesh.hpp"
#include "fae/camera.hpp"
#include "fae/lighting.hpp"
#include "fae/rendering/material.hpp"

namespace fae
{
    [[nodiscard]] auto
    make_webgpu_renderer(resource_manager& resources) noexcept -> renderer
    {
        return renderer{
            .get_clear_color =
                [&]()
            {
                auto clear_color = colors::black;
                resources.use_resource<fae::webgpu>(
                    [&](webgpu& webgpu)
                    {
                        clear_color = color{
                            .r = static_cast<std::uint8_t>(
                                webgpu.clear_color.r * 255.0f),
                            .g = static_cast<std::uint8_t>(
                                webgpu.clear_color.g * 255.0f),
                            .b = static_cast<std::uint8_t>(
                                webgpu.clear_color.b * 255.0f),
                            .a = static_cast<std::uint8_t>(
                                webgpu.clear_color.a * 255.0f),
                        };
                    });
                return clear_color;
            },
            .set_clear_color =
                [&](color value)
            {
                resources.use_resource<fae::webgpu>(
                    [&](webgpu& webgpu)
                    {
                        webgpu.clear_color = wgpu::Color{
                            .r = value.r / 255.0f,
                            .g = value.g / 255.0f,
                            .b = value.b / 255.0f,
                            .a = value.a / 255.0f,
                        };
                    });
            },
            .clear =
                [&]()
            {
                // TODO
            },
            .begin =
                [&]()
            {
                resources.use_resource<fae::webgpu>(
                    [&](webgpu& webgpu)
                    {
                        webgpu.current_render.render_commands.clear();

                        wgpu::SurfaceTexture surface_texture;
                        webgpu.surface.GetCurrentTexture(&surface_texture);
                        auto surface_texture_view = surface_texture.texture.CreateView();

                        auto command_encoder = webgpu.device.CreateCommandEncoder();
                        auto color_attachment = wgpu::RenderPassColorAttachment{
                            .view = surface_texture_view,
                            .depthSlice = WGPU_DEPTH_SLICE_UNDEFINED,
                            .resolveTarget = nullptr,
                            .loadOp = wgpu::LoadOp::Clear,
                            .storeOp = wgpu::StoreOp::Store,
                            .clearValue = webgpu.clear_color,
                        };
                        auto depth_attachment = wgpu::RenderPassDepthStencilAttachment{
                            .view = webgpu.render_pipeline.depth_texture.CreateView(),
                            .depthLoadOp = wgpu::LoadOp::Clear,
                            .depthStoreOp = wgpu::StoreOp::Store,
                            .depthClearValue = 1.0,
                            .stencilReadOnly = true,
                        };
                        auto render_pass_desc = wgpu::RenderPassDescriptor{
                            .label = "fae_render_pass",
                            .colorAttachmentCount = 1,
                            .colorAttachments = &color_attachment,
                            .depthStencilAttachment = &depth_attachment,
                        };
                        auto render_pass = command_encoder.BeginRenderPass(&render_pass_desc);
                        render_pass.SetPipeline(webgpu.render_pipeline.pipeline);
                        webgpu.current_render.command_encoder = command_encoder;
                        webgpu.current_render.render_pass = render_pass;

                        auto ui_command_encoder = webgpu.device.CreateCommandEncoder();
                        auto ui_color_attachment = wgpu::RenderPassColorAttachment{
                            .view = surface_texture_view,
                            .depthSlice = WGPU_DEPTH_SLICE_UNDEFINED,
                            .resolveTarget = nullptr,
                            .loadOp = wgpu::LoadOp::Load,
                            .storeOp = wgpu::StoreOp::Store,
                        };
                        auto ui_depth_attachment = wgpu::RenderPassDepthStencilAttachment{
                            .view = webgpu.render_pipeline.depth_texture.CreateView(),
                            .depthLoadOp = wgpu::LoadOp::Clear,
                            .depthStoreOp = wgpu::StoreOp::Store,
                            .depthClearValue = 1.0,
                            .stencilReadOnly = true,
                        };
                        auto ui_render_pass_desc = wgpu::RenderPassDescriptor{
                            .label = "fae_ui_render_pass",
                            .colorAttachmentCount = 1,
                            .colorAttachments = &ui_color_attachment,
                            .depthStencilAttachment = &ui_depth_attachment,
                        };
                        auto ui_render_pass = ui_command_encoder.BeginRenderPass(&ui_render_pass_desc);
                        webgpu.current_render.ui_command_encoder = ui_command_encoder;
                        webgpu.current_render.ui_render_pass = ui_render_pass;
                    });
            },
            .end =
                [&]()
            {
                resources.use_resource<fae::webgpu>(
                    [&](webgpu& webgpu)
                    {
                        if (!webgpu.current_render.render_commands.empty())
                        {
                            resources.use_resource<fae::active_camera>([&](active_camera active_camera)
                                {
                                if (!active_camera.camera_entity.valid())
                                    return;
                            auto &camera = active_camera.camera();
                            auto &camera_transform = active_camera.transform();

                            t_global_uniforms global_uniforms;
                            global_uniforms.camera_world_position = camera_transform.position;
                            auto time = resources.get_or_emplace<fae::time>(fae::time{});
                            auto t = time.elapsed().seconds_f32();
                            global_uniforms.time = t;

                            auto global_uniforms_buffer = create_buffer(webgpu.device, "fae_global_uniforms_buffer", sizeof(t_global_uniforms), wgpu::BufferUsage::Uniform);

                            std::vector<t_local_uniforms> local_uniforms;
                            for (auto& render_command : webgpu.current_render.render_commands)
                            {
                                local_uniforms.push_back(render_command.uniform_data);
                            }
                            auto sizeof_uniforms = local_uniforms.size() * webgpu.render_pipeline.uniform_stride;
                            auto local_uniforms_buffer = create_buffer(webgpu.device, "fae_local_uniforms_buffer", sizeof_uniforms, wgpu::BufferUsage::Uniform);

                            std::uint32_t uniform_offset = 0;
                            auto queue = webgpu.device.GetQueue();

                            queue.WriteBuffer(global_uniforms_buffer, 0, &global_uniforms, sizeof(t_global_uniforms));

                            for (auto& uniform : local_uniforms)
                            {
                                queue.WriteBuffer(local_uniforms_buffer, uniform_offset, &uniform, sizeof(t_local_uniforms));
                                uniform_offset += webgpu.render_pipeline.uniform_stride;
                            }

                            auto ambient_light_info_buffer = create_buffer(webgpu.device, "ambient_light_info_buffer", sizeof(fae::directional_light_info), wgpu::BufferUsage::Uniform);
                            resources.use_resource<fae::ambient_light_info>([&](fae::ambient_light_info info)
                                { queue.WriteBuffer(ambient_light_info_buffer, 0, &info, sizeof(fae::ambient_light_info)); });

                            auto directional_light_info_buffer = create_buffer(webgpu.device, "fae_directional_light_info_buffer", sizeof(fae::directional_light_info), wgpu::BufferUsage::Uniform);
                            resources.use_resource<fae::directional_light_info>([&](fae::directional_light_info info)
                                { queue.WriteBuffer(directional_light_info_buffer, 0, &info, sizeof(fae::directional_light_info)); });

                            uniform_offset = 0;
                            for (auto& render_command : webgpu.current_render.render_commands)
                            {
                                auto bind_entries = std::vector<wgpu::BindGroupEntry>{
                                    wgpu::BindGroupEntry{
                                        .binding = 0,
                                        .buffer = global_uniforms_buffer,
                                        .size = sizeof(t_global_uniforms),
                                    },
                                    wgpu::BindGroupEntry{
                                        .binding = 1,
                                        .buffer = local_uniforms_buffer,
                                        .size = sizeof(t_local_uniforms),
                                    },
                                    wgpu::BindGroupEntry{
                                        .binding = 2,
                                        .textureView = render_command.texture_view,
                                    },
                                    wgpu::BindGroupEntry{
                                        .binding = 3,
                                        .sampler = render_command.sampler,
                                    },
                                    wgpu::BindGroupEntry{
                                        .binding = 4,
                                        .buffer = ambient_light_info_buffer,
                                        .size = sizeof(fae::ambient_light_info),
                                    },
                                    wgpu::BindGroupEntry{
                                        .binding = 5,
                                        .buffer = directional_light_info_buffer,
                                        .size = sizeof(fae::directional_light_info),
                                    },
                                };
                                auto bind_group_descriptor = wgpu::BindGroupDescriptor{
                                    .label = "fae_bind_group",
                                    .layout = webgpu.render_pipeline.pipeline.GetBindGroupLayout(0),
                                    .entryCount = static_cast<std::size_t>(bind_entries.size()),
                                    .entries = bind_entries.data(),
                                };

                                auto uniform_bind_group = webgpu.device.CreateBindGroup(&bind_group_descriptor);
                                webgpu.current_render.render_pass.SetBindGroup(0, uniform_bind_group, 1, &uniform_offset);
                                uniform_offset += webgpu.render_pipeline.uniform_stride;

                                const auto vertex_buffer = create_buffer_with_data(
                                    webgpu.device, "indexed_render_data_vertex_buffer", render_command.vertex_data.data(), sizeof_data(render_command.vertex_data),
                                    wgpu::BufferUsage::Vertex);
                                webgpu.current_render.render_pass.SetVertexBuffer(0, vertex_buffer);
                                if (!render_command.index_data.empty())
                                {
                                    const auto index_buffer = create_buffer_with_data(
                                        webgpu.device, "indexed_render_data_index_buffer", render_command.index_data.data(), sizeof_data(render_command.index_data),
                                        wgpu::BufferUsage::Index);
                                    webgpu.current_render.render_pass.SetIndexBuffer(index_buffer, wgpu::IndexFormat::Uint32);
                                    webgpu.current_render.render_pass.DrawIndexed(render_command.index_data.size());
                                }
                                else
                                {
                                    webgpu.current_render.render_pass.Draw(render_command.vertex_data.size());
                                }
                            } });
                        }

                        webgpu.current_render.render_pass.End();
                        auto command_buffer = webgpu.current_render.command_encoder.Finish();
                        webgpu.current_render.ui_render_pass.End();
                        auto ui_command_buffer = webgpu.current_render.ui_command_encoder.Finish();

                        auto commands = std::vector<wgpu::CommandBuffer>{ command_buffer, ui_command_buffer };
                        webgpu.device.GetQueue().Submit(commands.size(), commands.data());
#ifndef FAE_PLATFORM_WEB
                        webgpu.surface.Present();
                        webgpu.instance.ProcessEvents();
#endif
                    });
            },
            .render_cube =
                [&](const renderer::render_cube_args& args)
            {
                resources.use_resource<fae::webgpu>(
                    [&](webgpu& webgpu)
                    {
                        auto mesh = meshes::cube();
                        t_local_uniforms local_uniforms;
                        resources.use_resource<fae::active_camera>([&](active_camera active_camera)
                            {
                                if (!active_camera.camera_entity.valid())
                            return;
                    auto &camera = active_camera.camera();
                    auto &camera_transform = active_camera.transform();

                    resources.use_resource<fae::primary_window>([&](fae::primary_window primary_window)
                    {
                        if (!primary_window.window_entity.valid())
                            return;
                        auto &window = primary_window.window();
                        auto window_size = window.get_size();
                        auto aspect_ratio = static_cast<float>(window_size.width) / static_cast<float>(window_size.height);

                        local_uniforms.view = math::lookAt(camera_transform.position, camera_transform.position + camera_transform.forward(), fae::vec3(0.f, 1.f, 0.f));
                        local_uniforms.projection = math::perspective(math::radians(camera.fov), aspect_ratio, camera.near_plane, camera.far_plane); });
                        auto transform = fae::transform{};
                        transform.position = args.position;
                        transform.rotation = args.rotation;
                        transform.scale = args.scale;
                        local_uniforms.model = transform.to_mat4();
                        local_uniforms.tint = args.tint.to_vec4();
                        webgpu.current_render.render_commands.push_back(fae::webgpu::current_render::render_command{
                            .vertex_data = mesh.vertices,
                            .index_data = mesh.indices,
                            .uniform_data = local_uniforms,
                        }); });
                    });
            },
            .render_model =
                [&](const renderer::render_model_args& args)
            {
                resources.use_resource<fae::webgpu>([&](fae::webgpu& webgpu)
                    {
                        t_local_uniforms local_uniforms;
                        resources.use_resource<fae::active_camera>([&](active_camera active_camera)
                        {
                            if (!active_camera.camera_entity.valid())
                                return;
                            auto &camera = active_camera.camera();
                            auto &camera_transform = active_camera.transform();

                            local_uniforms.view = math::lookAt(camera_transform.position, camera_transform.position + camera_transform.forward(), fae::vec3(0.f, 1.f, 0.f));
                            resources.use_resource<fae::primary_window>([&](fae::primary_window primary_window)
                        {
                            if (!primary_window.window_entity.valid())
                                return;
                            auto &window = primary_window.window();
                            auto window_size = window.get_size();
                            auto aspect_ratio = static_cast<float>(window_size.width) / static_cast<float>(window_size.height);

                            local_uniforms.projection = math::perspective(math::radians(camera.fov), aspect_ratio, camera.near_plane, camera.far_plane); });
                            local_uniforms.model = args.transform.to_mat4();

                            static auto cache = std::unordered_map<const texture*, texture_and_view>();
                            auto maybe_texture_and_view = cache.find(&args.model.material.diffuse);
                            if (maybe_texture_and_view == cache.end())
                            {
                                maybe_texture_and_view = cache.insert({ &args.model.material.diffuse, create_texture_with_mips_and_view(webgpu.device, args.model.material.diffuse) }).first;
                            }
                            auto texture_and_view = maybe_texture_and_view->second;

                            auto sample_descriptor = wgpu::SamplerDescriptor
                            {
                                .addressModeU = wgpu::AddressMode::Repeat,
                                .addressModeV = wgpu::AddressMode::Repeat,
                                .addressModeW = wgpu::AddressMode::Repeat,
                                .magFilter = wgpu::FilterMode::Nearest,
                                .minFilter = wgpu::FilterMode::Nearest,
                                .mipmapFilter = wgpu::MipmapFilterMode::Nearest,
                                .lodMinClamp = 0.f,
                                .lodMaxClamp = 32.f,
                                .compare = wgpu::CompareFunction::Undefined,
                                .maxAnisotropy = 1,
                            };

                            auto sampler = webgpu.device.CreateSampler(&sample_descriptor);

                        webgpu.current_render.render_commands.push_back(fae::webgpu::current_render::render_command{
                            .vertex_data = args.model.mesh.vertices,
                            .index_data = args.model.mesh.indices,
                            .uniform_data = local_uniforms,
                            .texture_view = texture_and_view.view,
                            .sampler = sampler,
                  }); }); });
            },
        };
    }
}
