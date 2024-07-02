#include "fae/rendering/webgpu_renderer.hpp"

#include <cstdint>

#include "fae/rendering/renderer.hpp"
#include "fae/resource_manager.hpp"
#include "fae/math.hpp"
#include "fae/time.hpp"
#include "fae/webgpu.hpp"
#include "fae/color.hpp"
#include "fae/rendering/renderer.hpp"

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
                        webgpu.current_render.vertex_data.clear();
                        webgpu.current_render.index_data.clear();
                        webgpu.current_render.uniform_data.clear();

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
                            .view = webgpu.depth_texture.CreateView(),
                            .depthLoadOp = wgpu::LoadOp::Clear,
                            .depthStoreOp = wgpu::StoreOp::Store,
                            .depthClearValue = 1.0,
                        };
                        auto render_pass_desc = wgpu::RenderPassDescriptor{
                            .colorAttachmentCount = 1,
                            .colorAttachments = &color_attachment,
                            .depthStencilAttachment = &depth_attachment,
                        };
                        auto render_pass = command_encoder.BeginRenderPass(&render_pass_desc);
                        render_pass.SetPipeline(webgpu.render_pipeline);
                        render_pass.SetBindGroup(0, webgpu.uniform_buffer.bind_group);
                        webgpu.current_render.render_pass = render_pass;
                        webgpu.current_render.command_encoder = command_encoder;
                    });
            },
            .end =
                [&]()
            {
                resources.use_resource<fae::webgpu>(
                    [&](webgpu& webgpu)
                    {
                        webgpu.device.GetQueue().WriteBuffer(webgpu.uniform_buffer.buffer, 0, webgpu.current_render.uniform_data.data(), sizeof_data(webgpu.current_render.uniform_data));
                        const auto vertex_buffer = create_buffer_with_data(
                            webgpu.device, "vertex_buffer", webgpu.current_render.vertex_data.data(), sizeof_data(webgpu.current_render.vertex_data),
                            wgpu::BufferUsage::Vertex);
                        webgpu.current_render.render_pass.SetVertexBuffer(0, vertex_buffer);
                        const auto index_buffer = create_buffer_with_data(
                            webgpu.device, "index_buffer", webgpu.current_render.index_data.data(), sizeof_data(webgpu.current_render.index_data),
                            wgpu::BufferUsage::Index);
                        webgpu.current_render.render_pass.SetIndexBuffer(index_buffer, wgpu::IndexFormat::Uint32);
                        if (!webgpu.current_render.index_data.empty())
                        {
                            webgpu.current_render.render_pass.DrawIndexed(webgpu.current_render.index_data.size());
                        }
                        webgpu.current_render.render_pass.End();
                        auto command_buffer = webgpu.current_render.command_encoder.Finish();
                        auto queue = webgpu.device.GetQueue();
                        queue.Submit(1, &command_buffer);
#ifndef FAE_PLATFORM_WEB
                        webgpu.surface.Present();
                        webgpu.instance.ProcessEvents();
#endif
                    });
            },
            .render_cube =
                [&](const renderer::draw_cube_args& args)
            {
                resources.use_resource<fae::webgpu>(
                    [&](webgpu& webgpu)
                    {
                        webgpu.current_render.vertex_data = std::vector<float>{
                            // clang-format off
				            -.5f, .5f, -.5f, 1.f, 		1.f, 1.f, 1.f, 1.f,		1.f, 0.f, 0.f, 1.f,			0.f, 1.f, // 0 left up back
				            -.5f, -.5f, -.5f, 1.f,		1.f, 1.f, 1.f, 1.f,		1.f, 0.f, 0.f, 1.f,			0.f, 0.f, // 1 left down back
				            .5f, -.5f, -.5f, 1.f,		1.f, 1.f, 1.f, 1.f,		1.f, 0.f, 0.f, 1.f,			1.f, 0.f, // 2 right down back
				            .5f, .5f, -.5f, 1.f,		1.f, 1.f, 1.f, 1.f,		1.f, 0.f, 0.f, 1.f,			1.f, 1.f, // 3 right up back
				            -.5f, .5f, .5f, 1.f, 		1.f, 1.f, 1.f, 1.f,		1.f, 0.f, 0.f, 1.f,			0.f, 1.f, // 4 left up front
				            -.5f, -.5f, .5f, 1.f,		1.f, 1.f, 1.f, 1.f,		1.f, 0.f, 0.f, 1.f,			0.f, 0.f, // 5 left down front
				            .5f, -.5f, .5f, 1.f,		1.f, 1.f, 1.f, 1.f,		1.f, 0.f, 0.f, 1.f,			1.f, 0.f, // 6 right down front
				            .5f, .5f, .5f, 1.f,			1.f, 1.f, 1.f, 1.f,		1.f, 0.f, 0.f, 1.f,			1.f, 1.f, // 7 right up front
                            // clang-format on
                        };
                        webgpu.current_render.index_data = std::vector<std::uint32_t>{
                            // clang-format off
				            0, 1, 2,
				            0, 2, 3,
				            3, 2, 6,
				            3, 6, 7,
				            7, 6, 5,
				            7, 5, 4,
				            4, 5, 1,
				            4, 1, 0,
				            0, 3, 7,
				            0, 7, 4,
				            1, 5, 6,
				            1, 6, 2,
                            // clang-format on
                        };

                        t_uniforms uniforms;
                        auto transform = fae::transform{};
                        transform.position = args.position;
                        transform.rotation = args.rotation;
                        transform.scale = args.scale;
                        uniforms.model = transform.to_mat4();
                        auto fov = 45.f;
                        auto aspect = 1920.f / 1080.f;
                        auto near_plane = 0.1f;
                        auto far_plane = 1000.f;
                        uniforms.projection = math::perspective(math::radians(fov), aspect, near_plane, far_plane);
                        uniforms.tint = args.tint.to_vec4();
                        auto time = resources.get_or_emplace<fae::time>(fae::time{});
                        auto t = time.elapsed().seconds_f32();
                        uniforms.time = t;
                        webgpu.current_render.uniform_data.resize(sizeof(uniforms));
                        std::memcpy(webgpu.current_render.uniform_data.data(), &uniforms, sizeof(uniforms));
                    });
            },
            .render_model = [&](const fae::model& model)
            {
                // TODO
            },
        };
    }
}
