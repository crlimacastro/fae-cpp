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
#include "fae/lighting.hpp"

namespace fae
{
    auto reconfigure_on_window_resized(const fae::window_resized& e) noexcept -> void
    {
        e.resources.use_resource<fae::webgpu>([&](webgpu& webgpu)
            {
            auto window_width = e.width;
            auto window_height = e.height;

            if (window_width == 0 || window_height == 0)
            {
                return;
            }
            webgpu.render_pipeline.depth_texture.Destroy();
            webgpu.surface.Unconfigure();

            auto surface_config = wgpu::SurfaceConfiguration{
                .device = webgpu.device,
                .format = webgpu.surface.GetPreferredFormat(webgpu.adapter),
                .usage = wgpu::TextureUsage::RenderAttachment,
                .width = static_cast<std::uint32_t>(window_width),
                .height = static_cast<std::uint32_t>(window_height),
                .presentMode = wgpu::PresentMode::Fifo,
            };
            webgpu.surface.Configure(&surface_config);

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
        auto maybe_webgpu = app.resources.get<fae::webgpu>();
        if (!maybe_webgpu)
        {
            fae::log_fatal("webgpu resource not found");
        }
        auto& webgpu = *maybe_webgpu;
        auto maybe_default_shader_module = create_shader_module_from_path(webgpu.device, "default_shader_module", app.assets.resolve_path("default.wgsl"));
        if (!maybe_default_shader_module)
        {
            fae::log_fatal("failed to load shader module");
        }
        auto shader_module = *maybe_default_shader_module;

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
            .attributeCount = static_cast<std::size_t>(vertex_attributes.size()),
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

        auto bind_group_layout_entries = std::vector<wgpu::BindGroupLayoutEntry>{
            wgpu::BindGroupLayoutEntry{
                .binding = 0,
                .visibility = wgpu::ShaderStage::Vertex | wgpu::ShaderStage::Fragment,
                .buffer = wgpu::BufferBindingLayout{
                    .type = wgpu::BufferBindingType::Uniform,
                    .hasDynamicOffset = true,
                    .minBindingSize = sizeof(t_uniforms),
                },
            },
            wgpu::BindGroupLayoutEntry{
                .binding = 1,
                .visibility = wgpu::ShaderStage::Fragment,
                .texture = wgpu::TextureBindingLayout{
                    .sampleType = wgpu::TextureSampleType::Float,
                    .viewDimension = wgpu::TextureViewDimension::e2D,
                },
            },
            wgpu::BindGroupLayoutEntry{
                .binding = 2,
                .visibility = wgpu::ShaderStage::Fragment,
                .sampler = wgpu::SamplerBindingLayout{
                    .type = wgpu::SamplerBindingType::Filtering,
                },
            },
            wgpu::BindGroupLayoutEntry{
                .binding = 3,
                .visibility = wgpu::ShaderStage::Vertex | wgpu::ShaderStage::Fragment,
                .buffer = wgpu::BufferBindingLayout{
                    .type = wgpu::BufferBindingType::Uniform,
                    .minBindingSize = sizeof(ambient_light_info),
                },
            },
            wgpu::BindGroupLayoutEntry{
                .binding = 4,
                .visibility = wgpu::ShaderStage::Vertex | wgpu::ShaderStage::Fragment,
                .buffer = wgpu::BufferBindingLayout{
                    .type = wgpu::BufferBindingType::Uniform,
                    .minBindingSize = sizeof(directional_light_info),
                },
            },
        };

        auto bind_group_layout_desc = wgpu::BindGroupLayoutDescriptor{
            .label = "fae_bind_group_layout",
            .entryCount = static_cast<std::size_t>(bind_group_layout_entries.size()),
            .entries = bind_group_layout_entries.data(),
        };

        auto bind_group_layouts = std::vector<wgpu::BindGroupLayout>
        {
            webgpu.device.CreateBindGroupLayout(&bind_group_layout_desc),
        };

        auto pipeline_layout_desc = wgpu::PipelineLayoutDescriptor{
            .label = "fae_pipeline_layout",
            .bindGroupLayoutCount = static_cast<std::size_t>(bind_group_layouts.size()),
            .bindGroupLayouts = bind_group_layouts.data(),
        };

        auto pipeline_layout = webgpu.device.CreatePipelineLayout(&pipeline_layout_desc);

        wgpu::RenderPipelineDescriptor pipeline_descriptor{
            .label = "fae_render_pipeline",
            .layout = pipeline_layout,
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

        auto ceil_to_next_multiple = [](std::uint32_t value, std::uint32_t step) noexcept -> std::uint32_t
        {
            uint32_t divide_and_ceil = value / step + (value % step == 0 ? 0 : 1);
            return step * divide_and_ceil;
        };

        auto supported_limits = wgpu::SupportedLimits{};
        webgpu.device.GetLimits(&supported_limits);
        auto device_limits = supported_limits.limits;
        auto uniform_stride = ceil_to_next_multiple(
            (std::uint32_t)sizeof(t_uniforms),
            (std::uint32_t)device_limits.minUniformBufferOffsetAlignment);

        return webgpu_render_pipeline{
            .shader_module = shader_module,
            .pipeline = render_pipeline,
            .depth_texture = depth_texture,
            .uniform_stride = uniform_stride,
        };
    }

    auto webgpu_plugin::init(application& app) const noexcept -> void
    {
        app
            .add_plugin(windowing_plugin{})
            .add_system<window_resized>(reconfigure_on_window_resized);

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
