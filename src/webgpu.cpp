#include "fae/webgpu.hpp"

#include <optional>
#include <format>
#include <string>
#include <string_view>
#include <filesystem>
#include <cstddef>

#include "fae/application.hpp"
#include "fae/sdl.hpp"
#include "fae/windowing.hpp"
#include "fae/rendering/mesh.hpp"

namespace fae
{
    auto reconfigure_on_window_resized(const fae::window_resized& e) noexcept -> void
    {
        e.resources.use_resource<fae::webgpu>([&](webgpu& webgpu)
            {
            auto window_width = e.width;
            auto window_height = e.height;
            auto surface_config = wgpu::SurfaceConfiguration{
                .device = webgpu.device,
                .format = webgpu.surface.GetPreferredFormat(webgpu.adapter),
                .usage = wgpu::TextureUsage::RenderAttachment,
                .width = static_cast<std::uint32_t>(window_width),
                .height = static_cast<std::uint32_t>(window_height),
                .presentMode = wgpu::PresentMode::Fifo,
            };
            webgpu.surface.Configure(&surface_config);

            webgpu.render_pipeline.depth_texture.Destroy();
            webgpu.render_pipeline.depth_texture = create_texture(
            webgpu.device, "Fae Depth texture",
            {
                .width = static_cast<std::uint32_t>(window_width),
                .height = static_cast<std::uint32_t>(window_height),
            },
            wgpu::TextureFormat::Depth24Plus, wgpu::TextureUsage::RenderAttachment); });
    }

    auto create_default_render_pipeline(application& app) noexcept -> webgpu_render_pipeline
    {
        auto maybe_default_shader_module = app.assets.load<shader_module>("default.wgsl");
        if (!maybe_default_shader_module)
        {
            fae::log_fatal("failed to load shader module");
        }
        auto default_shader_module = *maybe_default_shader_module;

        auto maybe_webgpu = app.resources.get<fae::webgpu>();
        if (!maybe_webgpu)
        {
            fae::log_fatal("webgpu resource not found");
        }
        auto& webgpu = *maybe_webgpu;

        auto shader_module = default_shader_module.create(webgpu.device);

        auto vertex_attributes = std::vector<wgpu::VertexAttribute>{
            wgpu::VertexAttribute{
                .format = wgpu::VertexFormat::Float32x4,
                .offset = offsetof(vertex, position),
                .shaderLocation = 0,
            },
            wgpu::VertexAttribute{
                .format = wgpu::VertexFormat::Float32x4,
                .offset = offsetof(vertex, color),
                .shaderLocation = 1,
            },
            wgpu::VertexAttribute{
                .format = wgpu::VertexFormat::Float32x3,
                .offset = offsetof(vertex, normal),
                .shaderLocation = 2,
            },
            wgpu::VertexAttribute{
                .format = wgpu::VertexFormat::Float32x2,
                .offset = offsetof(vertex, uv),
                .shaderLocation = 3,
            },
        };

        auto vertex_buffer_layout = wgpu::VertexBufferLayout{
            .arrayStride = sizeof(vertex),
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
        auto surface_format = webgpu.surface.GetPreferredFormat(webgpu.adapter);
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
        auto depth_texture_format = wgpu::TextureFormat::Depth24Plus;
        auto depth_stencil = wgpu::DepthStencilState{
            .format = depth_texture_format,
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
                .cullMode = wgpu::CullMode::Back,
            },
            .depthStencil = &depth_stencil,
            .multisample = wgpu::MultisampleState{},
            .fragment = &fragment_state,
        };

        auto render_pipeline = webgpu.device.CreateRenderPipeline(&pipeline_descriptor);

        auto maybe_primary_window = app.resources.get<primary_window>();
        if (!maybe_primary_window)
        {
            fae::log_fatal("primary window resource not found");
        }
        auto primary_window = *maybe_primary_window;
        auto& window = primary_window.window();
        auto window_size = window.get_size();

        auto depth_texture = create_texture(
            webgpu.device, "Fae Depth texture",
            {
                .width = static_cast<std::uint32_t>(window_size.width),
                .height = static_cast<std::uint32_t>(window_size.height),
            },
            depth_texture_format, wgpu::TextureUsage::RenderAttachment);

        constexpr std::uint64_t uniform_buffer_size = sizeof(fae::t_uniforms);
        auto uniform_buffer = create_buffer(webgpu.device, "uniform_buffer", uniform_buffer_size, wgpu::BufferUsage::Uniform);
        auto bind_entries = std::vector<wgpu::BindGroupEntry>{
            wgpu::BindGroupEntry{
                .binding = 0,
                .buffer = uniform_buffer,
                .size = uniform_buffer_size,
            },
        };
        auto bind_group_descriptor = wgpu::BindGroupDescriptor{
            .layout = render_pipeline.GetBindGroupLayout(0),
            .entryCount = static_cast<std::uint32_t>(bind_entries.size()),
            .entries = bind_entries.data(),
        };
        auto uniform_bind_group = webgpu.device.CreateBindGroup(&bind_group_descriptor);

        return webgpu_render_pipeline{
            .shader_module = shader_module,
            .pipeline = render_pipeline,
            .depth_texture = depth_texture,
            .uniform_buffer = webgpu_uniform_buffer{
                .buffer = uniform_buffer,
                .bind_group = uniform_bind_group,
            },
        };
    }

    auto webgpu_plugin::init(application& app) const noexcept -> void
    {
        app
            .add_plugin(windowing_plugin{})
            .add_system<window_resized>(reconfigure_on_window_resized);

        auto& webgpu = app.resources.emplace_and_get<fae::webgpu>(fae::webgpu{
            .instance = wgpu::CreateInstance(&instance_descriptor),
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
        auto& window = primary_window.window();
        auto window_size = window.get_size();

        auto surface_format = webgpu.surface.GetPreferredFormat(webgpu.adapter);

        auto surface_config = wgpu::SurfaceConfiguration{
            .device = webgpu.device,
            .format = surface_format,
            .usage = wgpu::TextureUsage::RenderAttachment,
            .width = static_cast<std::uint32_t>(window_size.width),
            .height = static_cast<std::uint32_t>(window_size.height),
            .presentMode = wgpu::PresentMode::Fifo,
        };
        webgpu.surface.Configure(&surface_config);

        webgpu.render_pipeline = webgpu.create_render_pipeline(app);
    }
}
