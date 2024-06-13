#include "fae/webgpu.hpp"

#include <optional>
#include <format>
#include <string>
#include <string_view>
#include <filesystem>

#include "fae/application.hpp"
#include "fae/sdl.hpp"
#include "fae/windowing.hpp"

namespace fae
{
    auto webgpu_plugin::init(application& app) const noexcept -> void
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

        auto surface_config = wgpu::SurfaceConfiguration{
            .device = webgpu.device,
            .format = surface_format,
            .usage = wgpu::TextureUsage::RenderAttachment,
            .width = static_cast<std::uint32_t>(window_size.width),
            .height = static_cast<std::uint32_t>(window_size.height),
            .presentMode = wgpu::PresentMode::Fifo,
        };
        webgpu.surface.Configure(&surface_config);

        struct fae_shader_module
        {
            std::string shader_src;

            static auto load(std::filesystem::path path) -> std::optional<fae_shader_module>
            {
                auto shader_module = fae_shader_module{};
                auto file = std::ifstream(path);
                if (!file.is_open())
                {
                    return std::nullopt;
                }
                file.seekg(0, std::ios::end);
                std::size_t size = file.tellg();
                shader_module.shader_src = std::string(size, ' ');
                file.seekg(0);
                file.read(shader_module.shader_src.data(), size);
                return shader_module;
            }

            auto create(wgpu::Device device) -> wgpu::ShaderModule
            {
                return create_shader_module_from_str(device, "shader_module", shader_src);
            }
        };

        auto maybe_fae_shader_module = app.assets.load<fae_shader_module>("main.wgsl");
        if (!maybe_fae_shader_module)
        {
            fae::log_fatal("failed to load shader module");
        }
        auto fae_shader_module = *maybe_fae_shader_module;
        auto shader_module = fae_shader_module.create(webgpu.device);

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
}
