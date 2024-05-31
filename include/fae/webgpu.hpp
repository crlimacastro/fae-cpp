#pragma once

#include <format>
#include <string>
#include <string_view>

#include <webgpu/webgpu_cpp.h>

#ifdef FAE_PLATFORM_WEB
#include <emscripten/emscripten.h>
#endif

#include "fae/application.hpp"
#include "fae/logging.hpp"
#include "fae/sdl.hpp"
#include "fae/windowing.hpp"

#include "fae/webgpu/sdl_impl.hpp"
#include "fae/webgpu/string_utils.hpp"
#include "fae/webgpu/utils.hpp"

namespace fae
{
    struct webgpu
    {
        wgpu::Instance instance;
        wgpu::Adapter adapter;
        wgpu::Device device;
        wgpu::Surface surface;
        #ifdef FAE_PLATFORM_WEB
        wgpu::SwapChain swapchain;
        #endif
        wgpu::RenderPipeline render_pipeline;

        uniform_buffer uniform_buffer;
        wgpu::Texture depth_texture;

        wgpu::Color clear_color = {0, 0, 0, 1};
        struct current_render
        {
            #ifdef FAE_PLATFORM_WEB
            wgpu::TextureView surface_texture_view;
            #endif
            wgpu::RenderPassEncoder render_pass;
            wgpu::CommandEncoder command_encoder;
            std::vector<float> vertex_data;
            std::vector<std::uint32_t> index_data;
            std::vector<std::uint8_t> uniform_data;
        };
        current_render current_render{};
    };

    struct webgpu_plugin
    {
        wgpu::RequestAdapterOptions adapter_options{};
        wgpu::DeviceDescriptor device_descriptor{
#ifndef FAE_PLATFORM_WEB
            .deviceLostCallbackInfo = wgpu::DeviceLostCallbackInfo{
                .mode = wgpu::CallbackMode::AllowSpontaneous,
                .callback = [](WGPUDevice const* device, WGPUDeviceLostReason cReason, char const* message, void* userdata)
                {
                    auto reason = static_cast<wgpu::DeviceLostReason>(cReason);
                    switch (reason)
                    {
                    case wgpu::DeviceLostReason::Destroyed:
                        // don't log this reason to not scare the user, happens naturally at the end of the application lifecycle
                        break;
                    case wgpu::DeviceLostReason::Unknown:
                    case wgpu::DeviceLostReason::InstanceDropped:
                    case wgpu::DeviceLostReason::FailedCreation:
                        fae::log_info(std::format("[wgpu] Device lost. [reason] {} [message] {}", to_string(reason), message));
                        break;
                    }
                },
            }
#endif
        };
#ifndef FAE_PLATFORM_WEB
        wgpu::LoggingCallback logging_callback = [](WGPULoggingType cType, char const* message, void* userdata)
        {
            auto type = static_cast<wgpu::LoggingType>(cType);
            fae::log_info(std::format("[wgpu] [type] {} [message] {}", to_string(type), message));
        };
#endif
        wgpu::ErrorCallback error_callback = [](WGPUErrorType cType, const char* message, void* userdata)
        {
            auto type = static_cast<wgpu::ErrorType>(cType);
            fae::log_error(std::format("[wgpu] [type] {} [message] {}", to_string(type), message));
        };

        auto init(application& app) const noexcept -> void
        {
            app.add_plugin(windowing_plugin{});
            auto& webgpu = app.resources.emplace_and_get<fae::webgpu>(fae::webgpu{
                .instance = wgpu::CreateInstance(),
            });
            webgpu.adapter = request_adapter_sync(webgpu.instance, adapter_options);
            webgpu.device = request_device_sync(webgpu.adapter, device_descriptor);
#ifndef FAE_PLATFORM_WEB
            webgpu.device.SetLoggingCallback(logging_callback, nullptr);
#endif
            webgpu.device.SetUncapturedErrorCallback(error_callback, nullptr);
            auto maybe_primary_window = app.resources.get<primary_window>();
            if (!maybe_primary_window)
            {
                fae::log_fatal("primary window resource not found");
            }
            auto primary_window = *maybe_primary_window;
            auto maybe_sdl_window = primary_window.window_entity.get_component<sdl_window>();
            if (!maybe_sdl_window)
            {
                fae::log_fatal("sdl window component not found in primary window entity");
            }
            auto& sdl_window = *maybe_sdl_window;
            webgpu.surface = get_sdl_webgpu_surface(webgpu.instance, sdl_window.raw.get());
            auto window = primary_window.window();
            auto window_size = window.get_size();

            auto surface_format = webgpu.surface.GetPreferredFormat(webgpu.adapter);

#ifndef FAE_PLATFORM_WEB
            auto surface_config = wgpu::SurfaceConfiguration{
                .device = webgpu.device,
                .format = surface_format,
                .usage = wgpu::TextureUsage::RenderAttachment,
                .width = static_cast<std::uint32_t>(window_size.width),
                .height = static_cast<std::uint32_t>(window_size.height),
                .presentMode = wgpu::PresentMode::Fifo,
            };
            webgpu.surface.Configure(&surface_config);
#else
            auto swap_chain_descriptor = wgpu::SwapChainDescriptor
            {
                .usage = wgpu::TextureUsage::RenderAttachment,
                .format = surface_format,
                .width = static_cast<std::uint32_t>(window_size.width),
                .height = static_cast<std::uint32_t>(window_size.height),
                .presentMode = wgpu::PresentMode::Fifo,
            };
            webgpu.swapchain = webgpu.device.CreateSwapChain(webgpu.surface, &swap_chain_descriptor);
#endif

            auto shader_source = std::string(R"(
struct t_uniforms
{
	model : mat4x4f,
	view : mat4x4f,
	projection : mat4x4f,
	tint: vec4f,
};
@group(0) @binding(0) var<uniform> uniforms : t_uniforms;

struct vertex_input {
	@builtin(vertex_index) vertex_index: u32,
	@builtin(instance_index) instance_index: u32,
	@location(0) position: vec4f,
	@location(1) color: vec4f,
	@location(2) normal: vec4f,
	@location(3) uv: vec2f,
};

struct vertex_output {
	@builtin(position) position: vec4f,
	@location(0) color: vec4f,
	@location(1) uv: vec2f,
};

@vertex
fn vs_main(in: vertex_input) -> vertex_output {
	var model_view_projection_matrix = uniforms.projection * uniforms.view * uniforms.model;
	var out: vertex_output;
	out.position = model_view_projection_matrix * in.position;
	out.color = uniforms.tint * in.color;
	out.uv = in.uv;
    return out;
}

@fragment
fn fs_main(in: vertex_output) -> @location(0) vec4f {
	return in.color;
}
)");
            auto shader_module = create_shader_module(webgpu.device, "shader_module", shader_source);

            auto vertex_attributes = std::array<wgpu::VertexAttribute, 4>{
                wgpu::VertexAttribute{
                    .format = wgpu::VertexFormat::Float32x4,
                    .offset = 0,
                    .shaderLocation = 0,
                },
                wgpu::VertexAttribute{
                    .format = wgpu::VertexFormat::Float32x4,
                    .offset = 4 * sizeof(float),
                    .shaderLocation = 1,
                },
                wgpu::VertexAttribute{
                    .format = wgpu::VertexFormat::Float32x4,
                    .offset = 8 * sizeof(float),
                    .shaderLocation = 2,
                },
                wgpu::VertexAttribute{
                    .format = wgpu::VertexFormat::Float32x2,
                    .offset = 12 * sizeof(float),
                    .shaderLocation = 3,
                },
            };

            auto vertex_buffer_layout = wgpu::VertexBufferLayout{
                .arrayStride = 14 * sizeof(float),
                .stepMode = wgpu::VertexStepMode::Vertex,
                .attributeCount = static_cast<std::uint32_t>(vertex_attributes.size()),
                .attributes = vertex_attributes.data(),
            };

            auto blend_state = wgpu::BlendState{
                .color = wgpu::BlendComponent{
                    .operation = wgpu::BlendOperation::Add,
                    .srcFactor = wgpu::BlendFactor::SrcAlpha,
                    .dstFactor = wgpu::BlendFactor::OneMinusSrcAlpha,
                },
                .alpha = wgpu::BlendComponent{
                    .operation = wgpu::BlendOperation::Add,
                    .srcFactor = wgpu::BlendFactor::Zero,
                    .dstFactor = wgpu::BlendFactor::One,
                },
            };
            wgpu::ColorTargetState color_target_state{
                .format = surface_format,
                .blend = &blend_state,
                .writeMask = wgpu::ColorWriteMask::All,
            };
            wgpu::FragmentState fragment_state{
                .module = shader_module,
                .entryPoint = "fs_main",
                .constantCount = 0,
                .constants = nullptr,
                .targetCount = 1,
                .targets = &color_target_state,
            };
            auto depth_stencil = wgpu::DepthStencilState{
                .format = wgpu::TextureFormat::Depth24Plus,
                .depthWriteEnabled = true,
                .depthCompare = wgpu::CompareFunction::Less,
            };
            wgpu::RenderPipelineDescriptor pipeline_descriptor{
                .label = "fae_render_pipeline",
                .vertex = wgpu::VertexState{
                    .module = shader_module,
                    .entryPoint = "vs_main",
                    .constantCount = 0,
                    .constants = nullptr,
                    .bufferCount = 1,
                    .buffers = &vertex_buffer_layout,
                },
                .primitive = wgpu::PrimitiveState{
                    .topology = wgpu::PrimitiveTopology::TriangleList,
                    .stripIndexFormat = wgpu::IndexFormat::Undefined,
                    .frontFace = wgpu::FrontFace::CCW,
                    .cullMode = wgpu::CullMode::None,
                },
                .depthStencil = &depth_stencil,
                .multisample = wgpu::MultisampleState{
                    .count = 1,
                    .mask = ~0u,
                    .alphaToCoverageEnabled = false,
                },
                .fragment = &fragment_state,
            };
            webgpu.render_pipeline = webgpu.device.CreateRenderPipeline(&pipeline_descriptor);

            webgpu.depth_texture = create_texture(
                webgpu.device, "Depth texture",
                {
                    .width = static_cast<std::uint32_t>(window_size.width),
                    .height = static_cast<std::uint32_t>(window_size.height),
                },
                wgpu::TextureFormat::Depth24Plus, wgpu::TextureUsage::RenderAttachment);

            constexpr std::uint64_t uniform_buffer_size = (3 * 4 * 16) + (1 * 4 * 4); // (3 * mat4x4<f32>) + (1 * vec4f)
            auto uniform_buffer = create_buffer(webgpu.device, "uniform_buffer", uniform_buffer_size, wgpu::BufferUsage::Uniform);
            auto bind_entries = std::array<wgpu::BindGroupEntry, 1>{
                wgpu::BindGroupEntry{
                    .binding = 0,
                    .buffer = uniform_buffer,
                    .size = uniform_buffer_size,
                },
            };
            auto bind_group_descriptor = wgpu::BindGroupDescriptor{
                .layout = webgpu.render_pipeline.GetBindGroupLayout(0),
                .entryCount = static_cast<std::uint32_t>(bind_entries.size()),
                .entries = bind_entries.data(),
            };
            auto uniform_bind_group = webgpu.device.CreateBindGroup(&bind_group_descriptor);

            webgpu.uniform_buffer = fae::uniform_buffer{
                .buffer = uniform_buffer,
                .size = uniform_buffer_size,
                .bind_group = uniform_bind_group,
            };
        }
    };
} // namespace fae
