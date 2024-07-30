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
                            .colorAttachmentCount = 1,
                            .colorAttachments = &color_attachment,
                            .depthStencilAttachment = &depth_attachment,
                        };
                        auto render_pass = command_encoder.BeginRenderPass(&render_pass_desc);
                        render_pass.SetPipeline(webgpu.render_pipeline.pipeline);
                        webgpu.current_render.command_encoder = command_encoder;
                        webgpu.current_render.render_pass = render_pass;
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
                            std::vector<t_uniforms> uniforms;
                            for (auto& render_command : webgpu.current_render.render_commands)
                            {
                                uniforms.push_back(render_command.uniform_data);
                            }
                            auto sizeof_uniforms = uniforms.size() * webgpu.render_pipeline.uniform_stride;
                            auto uniform_buffer = create_buffer(webgpu.device, "fae_uniform_buffer", sizeof_uniforms, wgpu::BufferUsage::Uniform);

                            std::uint32_t uniform_offset = 0;
                            auto queue = webgpu.device.GetQueue();
                            for (auto& uniform : uniforms)
                            {
                                queue.WriteBuffer(uniform_buffer, uniform_offset, &uniform, sizeof(t_uniforms));
                                uniform_offset += webgpu.render_pipeline.uniform_stride;
                            }

                            uniform_offset = 0;
                            for (auto& render_command : webgpu.current_render.render_commands)
                            {
                                auto bind_entries = std::vector<wgpu::BindGroupEntry>{
                                    wgpu::BindGroupEntry{
                                        .binding = 0,
                                        .buffer = uniform_buffer,
                                        .size = sizeof(t_uniforms),
                                    },
                                    wgpu::BindGroupEntry{
                                        .binding = 1,
                                        .textureView = render_command.texture_view,
                                    },
                                    wgpu::BindGroupEntry{
                                        .binding = 2,
                                        .sampler = render_command.sampler,
                                    },
                                };
                                auto bind_group_descriptor = wgpu::BindGroupDescriptor{
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
                            }
                        }

                        webgpu.current_render.render_pass.End();
                        auto command_buffer = webgpu.current_render.command_encoder.Finish();
                        webgpu.device.GetQueue().Submit(1, &command_buffer);
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
                        t_uniforms uniforms;
                        resources.use_resource<fae::active_camera>([&](active_camera active_camera)
                            {
                    auto &camera = active_camera.camera();
                    auto &camera_transform = active_camera.transform();
                    uniforms.view = math::lookAt(camera_transform.position, camera_transform.position + camera_transform.forward(), fae::vec3(0.f, 1.f, 0.f));
                    uniforms.projection = math::perspective(math::radians(camera.fov), camera.aspect, camera.near_plane, camera.far_plane); });
                        auto transform = fae::transform{};
                        transform.position = args.position;
                        transform.rotation = args.rotation;
                        transform.scale = args.scale;
                        uniforms.model = transform.to_mat4();
                        uniforms.tint = args.tint.to_vec4();
                        auto time = resources.get_or_emplace<fae::time>(fae::time{});
                        auto t = time.elapsed().seconds_f32();
                        uniforms.time = t;
                        webgpu.current_render.render_commands.push_back(fae::webgpu::current_render::render_command{
                            .vertex_data = mesh.vertices,
                            .index_data = mesh.indices,
                            .uniform_data = uniforms,
                        });
                    });
            },
            .render_model =
                [&](const renderer::render_model_args& args)
            {
                resources.use_resource<fae::webgpu>([&](fae::webgpu& webgpu)
                    {
                    t_uniforms uniforms;
                    resources.use_resource<fae::active_camera>([&](active_camera active_camera)
                    {
                        auto &camera = active_camera.camera();
                        auto &camera_transform = active_camera.transform();
                        uniforms.view = math::lookAt(camera_transform.position, camera_transform.position + camera_transform.forward(), fae::vec3(0.f, 1.f, 0.f));
                        uniforms.projection = math::perspective(math::radians(camera.fov), camera.aspect, camera.near_plane, camera.far_plane); });
                        uniforms.model = args.transform.to_mat4();
                        auto time = resources.get_or_emplace<fae::time>(fae::time{});
                        auto t = time.elapsed().seconds_f32();
                        uniforms.time = t;

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
                        .uniform_data = uniforms,
                        .texture_view = texture_and_view.view,
                        .sampler = sampler,
                  }); });
            },
        };
    }
}
