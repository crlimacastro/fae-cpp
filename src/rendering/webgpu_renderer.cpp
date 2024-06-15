#include "fae/rendering/webgpu_renderer.hpp"

#include <cstdint>

#include "fae/rendering/renderer.hpp"
#include "fae/resource_manager.hpp"
#include "fae/math.hpp"
#include "fae/time.hpp"
#include "fae/webgpu.hpp"
#include "fae/color.hpp"

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
                        // webgpu.current_render.uniform_data.resize((3 * 4 * 16) + (1 * 4 * 4) + (4 * 4));

                        wgpu::SurfaceTexture surface_texture;
                        webgpu.surface.GetCurrentTexture(&surface_texture);
                        auto surface_texture_view = surface_texture.texture.CreateView();

                        auto fov = 45.f;
                        auto aspect = 1920.f / 1080.f;
                        auto near_plane = 0.1f;
                        auto far_plane = 1000.f;

                        static auto transform = fae::transform{};

                        auto time = resources.get_or_emplace<fae::time>(fae::time{});
                        auto dt = time.delta().seconds_f32();
                        auto t = time.elapsed().seconds_f32();

                        transform.position = { 1.2f * std::cos(t), 1.2f * std::sinf(t * 2), -5.f };
                        transform.rotation *= math::angleAxis(math::radians(60.f) * dt, vec3(0.0f, 1.0f, 0.0f));

                        struct t_uniforms
                        {
                            mat4 model = mat4(1.f);
                            mat4 view = mat4(1.f);
                            mat4 projection = mat4(1.f);
                            vec4 tint = { 1.f, 1.f, 1.f, 1.f };
                            float time = 0;
                            float padding0;
                            float padding1;
                            float padding2;
                        };
                        static_assert(sizeof(t_uniforms) % 16 == 0, "uniform buffer must be aligned on 16 bytes");
                        t_uniforms uniforms;
                        uniforms.model = transform.to_mat4();
                        uniforms.projection = math::perspective(math::radians(fov), aspect, near_plane, far_plane);
                        static auto hsva = color_hsva::from_rgba(colors::red);
                        hsva.h = static_cast<float>(static_cast<int>(hsva.h + dt * 120) % 360);
                        auto tint = hsva.to_rgba().to_array();
                        uniforms.tint = hsva.to_rgba().to_vec4();
                        uniforms.time = t;
                        webgpu.current_render.uniform_data.resize(sizeof(uniforms));
                        std::memcpy(webgpu.current_render.uniform_data.data(), &uniforms, sizeof(uniforms));

                        webgpu.device.GetQueue().WriteBuffer(webgpu.uniform_buffer.buffer, 0, &uniforms, sizeof(uniforms));
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
                        const auto vertex_buffer = create_buffer_with_data(
                            webgpu.device, "vertex_buffer", webgpu.current_render.vertex_data.data(), sizeof_data(webgpu.current_render.vertex_data),
                            wgpu::BufferUsage::Vertex);
                        webgpu.current_render.render_pass.SetVertexBuffer(0, vertex_buffer);
                        const auto index_buffer = create_buffer_with_data(
                            webgpu.device, "index_buffer", webgpu.current_render.index_data.data(), sizeof_data(webgpu.current_render.index_data),
                            wgpu::BufferUsage::Index);
                        webgpu.current_render.render_pass.SetIndexBuffer(index_buffer, wgpu::IndexFormat::Uint32);
                        webgpu.current_render.render_pass.DrawIndexed(webgpu.current_render.index_data.size());
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
        };
    }
}
