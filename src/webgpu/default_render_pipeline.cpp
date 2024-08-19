#include "fae/webgpu/default_render_pipeline.hpp"

#include "fae/application/application.hpp"
#include "fae/asset_manager.hpp"
#include "fae/resource_manager.hpp"
#include "fae/core/offset_of.hpp"
#include "fae/windowing.hpp"
#include "fae/webgpu.hpp"
#include "fae/lighting.hpp"
#include "fae/rendering/render_pipeline.hpp"
#include "fae/rendering/render_pass.hpp"

auto fae::create_default_render_pipeline(fae::resource_manager& resources, fae::asset_manager& assets) noexcept -> render_pipeline
{
    auto maybe_webgpu = resources.get<fae::webgpu>();
    if (!maybe_webgpu)
    {
        fae::log_fatal("webgpu resource not found");
    }
    auto& webgpu = *maybe_webgpu;
    auto maybe_default_shader_module = create_shader_module_from_path(webgpu.device, "default_shader_module", assets.resolve_path("default.wgsl"));
    if (!maybe_default_shader_module)
    {
        fae::log_fatal("failed to load shader module");
    }
    auto shader_module = *maybe_default_shader_module;

    auto vertex_attributes = std::vector<wgpu::VertexAttribute>{
        wgpu::VertexAttribute{
            .format = wgpu::VertexFormat::Float32x4,
            .offset = fae::offset_of(&vertex::position),
            .shaderLocation = 0,
        },
        wgpu::VertexAttribute{
            .format = wgpu::VertexFormat::Float32x4,
            .offset = fae::offset_of(&vertex::color),
            .shaderLocation = 1,
        },
        wgpu::VertexAttribute{
            .format = wgpu::VertexFormat::Float32x3,
            .offset = fae::offset_of(&vertex::normal),
            .shaderLocation = 2,
        },
        wgpu::VertexAttribute{
            .format = wgpu::VertexFormat::Float32x2,
            .offset = fae::offset_of(&vertex::uv),
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
                .minBindingSize = sizeof(global_uniforms_t),
            },
        },
        wgpu::BindGroupLayoutEntry{
            .binding = 1,
            .visibility = wgpu::ShaderStage::Vertex | wgpu::ShaderStage::Fragment,
            .buffer = wgpu::BufferBindingLayout{
                .type = wgpu::BufferBindingType::Uniform,
                .hasDynamicOffset = true,
                .minBindingSize = sizeof(local_uniforms_t),
            },
        },
        wgpu::BindGroupLayoutEntry{
            .binding = 2,
            .visibility = wgpu::ShaderStage::Fragment,
            .texture = wgpu::TextureBindingLayout{
                .sampleType = wgpu::TextureSampleType::Float,
                .viewDimension = wgpu::TextureViewDimension::e2D,
            },
        },
        wgpu::BindGroupLayoutEntry{
            .binding = 3,
            .visibility = wgpu::ShaderStage::Fragment,
            .sampler = wgpu::SamplerBindingLayout{
                .type = wgpu::SamplerBindingType::Filtering,
            },
        },
        wgpu::BindGroupLayoutEntry{
            .binding = 4,
            .visibility = wgpu::ShaderStage::Vertex | wgpu::ShaderStage::Fragment,
            .buffer = wgpu::BufferBindingLayout{
                .type = wgpu::BufferBindingType::Uniform,
                .minBindingSize = sizeof(ambient_light_info),
            },
        },
        wgpu::BindGroupLayoutEntry{
            .binding = 5,
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

    auto bind_group_layouts = std::vector<wgpu::BindGroupLayout>{
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

    auto webgpu_render_pipeline = webgpu.device.CreateRenderPipeline(&pipeline_descriptor);

    auto maybe_primary_window = resources.get<primary_window>();
    if (!maybe_primary_window)
    {
        fae::log_fatal("primary window resource not found");
    }
    auto primary_window = *maybe_primary_window;
    auto& window = primary_window.window();
    auto window_size = window.get_size();

    auto ceil_to_next_multiple = [](std::uint32_t value, std::uint32_t step) noexcept -> std::uint32_t
    {
        uint32_t divide_and_ceil = value / step + (value % step == 0 ? 0 : 1);
        return step * divide_and_ceil;
    };

    auto supported_limits = wgpu::SupportedLimits{};
    webgpu.device.GetLimits(&supported_limits);
    auto device_limits = supported_limits.limits;
    auto uniform_stride = ceil_to_next_multiple(
        (std::uint32_t)sizeof(local_uniforms_t),
        (std::uint32_t)device_limits.minUniformBufferOffsetAlignment);

    std::size_t id = webgpu.render_pipelines.size();
    webgpu.render_pipelines.push_back(webgpu::render_pipeline{
        .shader_module = shader_module,
        .render_pipeline = webgpu_render_pipeline,
        .depth_texture = create_texture(
        webgpu.device, "Fae Depth texture",
        {
            .width = static_cast<std::uint32_t>(window_size.width),
            .height = static_cast<std::uint32_t>(window_size.height),
        },
        depth_texture_format, wgpu::TextureUsage::RenderAttachment),
        .uniform_stride = uniform_stride,
    });

    auto& render_pipeline = webgpu.render_pipelines[id];

    return fae::render_pipeline{
        .get_id = [&, id]()
        { return id; },
        .prepare_render_pass = [&](std::size_t id)
        {
            auto command_encoder = webgpu.device.CreateCommandEncoder();

            wgpu::SurfaceTexture surface_texture;
            webgpu.surface.GetCurrentTexture(&surface_texture);
            auto surface_texture_view = surface_texture.texture.CreateView();

            auto color_attachment = wgpu::RenderPassColorAttachment{
                .view = surface_texture_view,
                .depthSlice = WGPU_DEPTH_SLICE_UNDEFINED,
                .resolveTarget = nullptr,
                .loadOp = wgpu::LoadOp::Clear,
                .storeOp = wgpu::StoreOp::Store,
                .clearValue = webgpu.clear_color,
            };
            auto depth_attachment = wgpu::RenderPassDepthStencilAttachment{
                .view = render_pipeline.depth_texture.CreateView(),
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
            auto render_pass_encoder = command_encoder.BeginRenderPass(&render_pass_desc);
            render_pass_encoder.SetPipeline(render_pipeline.render_pipeline);

            webgpu.render_passes[id].command_encoder = command_encoder;
            webgpu.render_passes[id].render_pass_encoder = render_pass_encoder; },
        .on_window_resized = [&](const window_resized& window_resized_event)
        { window_resized_event.resources.use_resource<fae::webgpu>([&](fae::webgpu& webgpu)
              {
            render_pipeline.depth_texture.Destroy();
            render_pipeline.depth_texture = create_texture(
            webgpu.device, "Fae Depth texture",
            {
                .width = static_cast<std::uint32_t>(window_resized_event.width),
                .height = static_cast<std::uint32_t>(window_resized_event.height),
            },
            wgpu::TextureFormat::Depth24Plus,
            wgpu::TextureUsage::RenderAttachment); }); }
    };
}
