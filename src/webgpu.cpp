module;

#include "fae/sdl.hpp"
#include <format>
#include <string>
#include <string_view>
#ifdef SDL_PLATFORM_WIN32
#include <Windows.h>
#endif
#include "fae/webgpu.hpp"

export module fae:webgpu;

import :logging;
import :application;
import :windowing;
import :sdl;
export import :webgpu.sdl_impl;
export import :webgpu.string_utils;

export namespace fae
{
	wgpu::Buffer create_buffer(const wgpu::Device &device,
		std::string_view label,
		std::size_t size,
		wgpu::BufferUsage usage)
	{
		wgpu::BufferDescriptor desc{
			.label = label.data(),
			.usage = usage | wgpu::BufferUsage::CopyDst,
			.size = size,
		};
		return device.CreateBuffer(&desc);
	}

	wgpu::Buffer create_buffer_with_data(const wgpu::Device &device,
		std::string_view label,
		const void *data,
		std::size_t size,
		wgpu::BufferUsage usage)
	{
		auto buffer = create_buffer(device, label, size, usage);
		device.GetQueue().WriteBuffer(buffer, 0, data, size);
		return buffer;
	}

	wgpu::ShaderModule create_shader_module(const wgpu::Device &device,
		std::string_view label,
		const char *src)
	{
		wgpu::ShaderModuleWGSLDescriptor wgsl_desc;
		wgsl_desc.code = src;
		wgpu::ShaderModuleDescriptor desc{
			.nextInChain = &wgsl_desc,
			.label = label.data(),
		};
		auto shader = device.CreateShaderModule(&desc);
		return shader;
	}

	wgpu::Texture create_texture(const wgpu::Device &device,
		std::string_view label,
		wgpu::Extent3D extent,
		wgpu::TextureFormat format,
		wgpu::TextureUsage usage)
	{
		wgpu::TextureDescriptor desc{
			.label = label.data(),
			.usage = usage,
			.size = extent,
			.format = format,
		};
		return device.CreateTexture(&desc);
	}

	struct uniform_buffer
	{
		wgpu::Buffer buffer;
		std::size_t size;
		wgpu::BindGroup bind_group;
	};

	struct webgpu
	{
		wgpu::Instance instance;
		wgpu::Adapter adapter;
		wgpu::Device device;
		wgpu::Surface surface;
		wgpu::RenderPipeline render_pipeline;

		uniform_buffer uniform_buffer;
		wgpu::Texture depth_texture;

		wgpu::Color clear_color = {0, 0, 0, 1};
		struct current_render
		{
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
		wgpu::InstanceDescriptor instance_descriptor{};
		wgpu::RequestAdapterOptions adapter_options{};
		wgpu::DeviceDescriptor device_descriptor{
			.deviceLostCallbackInfo = wgpu::DeviceLostCallbackInfo{
				.mode = wgpu::CallbackMode::AllowSpontaneous,
				.callback = [](WGPUDevice const *device, WGPUDeviceLostReason cReason, char const *message, void *userdata)
				{
					auto reason = static_cast<wgpu::DeviceLostReason>(cReason);
					switch (reason)
					{
					case wgpu::DeviceLostReason::Destroyed:
						// don't log this reason to not scare the user, happens naturally at the end of the application lifecycle
						break;
					case wgpu::DeviceLostReason::Undefined:
					case wgpu::DeviceLostReason::InstanceDropped:
					case wgpu::DeviceLostReason::FailedCreation:
						fae::log_info(std::format("[wgpu] Device lost. [reason] {} [message] {}", to_string(reason), message));
						break;
					}
				},
			}};
		wgpu::LoggingCallback logging_callback = [](WGPULoggingType cType, char const *message, void *userdata)
		{
			auto type = static_cast<wgpu::LoggingType>(cType);
			fae::log_info(std::format("[wgpu] [type] {} [message] {}", to_string(type), message));
		};
		wgpu::ErrorCallback error_callback = [](WGPUErrorType cType, const char *message, void *userdata)
		{
			auto type = static_cast<wgpu::ErrorType>(cType);
			fae::log_error(std::format("[wgpu] [type] {} [message] {}", to_string(type), message));
		};

		auto init(application &app) const noexcept -> void
		{
			app.add_plugin(windowing_plugin{});
			auto &webgpu = app.resources.emplace_and_get<fae::webgpu>(fae::webgpu{
				.instance = wgpu::CreateInstance(&instance_descriptor),
			});
			struct request_adapter_data
			{
				const webgpu_plugin &plugin;
				application &app;
				fae::webgpu &webgpu;
			};

			webgpu.instance.RequestAdapter(&adapter_options,
				wgpu::RequestAdapterCallbackInfo{
					.mode = wgpu::CallbackMode::AllowSpontaneous,
					.callback = [](WGPURequestAdapterStatus cStatus, WGPUAdapter adapter, const char *message, void *userdata)
					{
						const auto status = static_cast<wgpu::RequestAdapterStatus>(cStatus);
						if (status != wgpu::RequestAdapterStatus::Success)
						{
							fae::log_fatal(std::format("failed to request adapter: {}", message));
						}
						auto data = reinterpret_cast<request_adapter_data *>(userdata);
						struct request_device_data
						{
							const webgpu_plugin &plugin;
							application &app;
							fae::webgpu &webgpu;
						};
						data->webgpu.adapter = wgpu::Adapter::Acquire(adapter);
						data->webgpu.adapter.RequestDevice(&data->plugin.device_descriptor,
							wgpu::RequestDeviceCallbackInfo{
								.mode = wgpu::CallbackMode::AllowSpontaneous,
								.callback = [](WGPURequestDeviceStatus cStatus, WGPUDevice device, char const *message, void *userdata)
								{
									const auto status = static_cast<wgpu::RequestDeviceStatus>(cStatus);
									if (status != wgpu::RequestDeviceStatus::Success)
									{
										fae::log_fatal(std::format("failed to request device: ", message));
									}
									auto data = reinterpret_cast<request_device_data *>(userdata);
									data->webgpu.device = wgpu::Device::Acquire(device);
									data->webgpu.device.SetLoggingCallback(data->plugin.logging_callback, nullptr);
									data->webgpu.device.SetUncapturedErrorCallback(data->plugin.error_callback, nullptr);
									auto maybe_primary_window = data->app.resources.get<primary_window>();
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
									auto &sdl_window = *maybe_sdl_window;
									data->webgpu.surface = get_sdl_webgpu_surface(data->webgpu.instance, sdl_window.raw.get());
									auto window = primary_window.window();
									auto window_size = window.get_size();

									auto surface_format = data->webgpu.surface.GetPreferredFormat(data->webgpu.adapter);
									wgpu::SurfaceConfiguration surface_config = wgpu::SurfaceConfiguration{
										.device = device,
										.format = surface_format,
										.usage = wgpu::TextureUsage::RenderAttachment,
										.width = static_cast<std::uint32_t>(window_size.width),
										.height = static_cast<std::uint32_t>(window_size.height),
										.presentMode = wgpu::PresentMode::Fifo,
									};
									data->webgpu.surface.Configure(&surface_config);

									auto shader_module = create_shader_module(data->webgpu.device, "shader_module", R"(
struct t_uniforms
{
	model : mat4x4f,
	view : mat4x4f,
	projection : mat4x4f,
};
@group(0) @binding(0) var<uniform> uniforms : t_uniforms;

struct vertex_input {
	@builtin(vertex_index) vertex_index: u32,
	@location(0) position: vec4f,
	@location(1) color: vec4f,
	@location(2) uv: vec2f,
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
	out.color = in.color;
	out.uv = in.uv;
    return out;
}

@fragment
fn fs_main(in: vertex_output) -> @location(0) vec4f {
	return in.color;
}
)");

									auto vertex_attributes = std::array<wgpu::VertexAttribute, 3>{
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
											.format = wgpu::VertexFormat::Float32x2,
											.offset = 2 * sizeof(float),
											.shaderLocation = 2,
										},
									};

									auto vertex_buffer_layout = wgpu::VertexBufferLayout{
										.arrayStride = 10 * sizeof(float),
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
									data->webgpu.render_pipeline = data->webgpu.device.CreateRenderPipeline(&pipeline_descriptor);

									data->webgpu.depth_texture = create_texture(
										data->webgpu.device, "Depth texture",
										{
											.width = static_cast<std::uint32_t>(window_size.width),
											.height = static_cast<std::uint32_t>(window_size.height),
										},
										wgpu::TextureFormat::Depth24Plus, wgpu::TextureUsage::RenderAttachment);

									constexpr std::uint64_t uniform_buffer_size = 4 * 16 * 3; // mat4x4<f32> * 3
									auto uniform_buffer = create_buffer(device, "uniform_buffer", uniform_buffer_size, wgpu::BufferUsage::Uniform);
									auto bind_entries = std::array<wgpu::BindGroupEntry, 1>{
										wgpu::BindGroupEntry{
											.binding = 0,
											.buffer = uniform_buffer,
											.size = uniform_buffer_size,
										},
									};
									auto bind_group_descriptor = wgpu::BindGroupDescriptor{
										.layout = data->webgpu.render_pipeline.GetBindGroupLayout(0),
										.entryCount = static_cast<std::uint32_t>(bind_entries.size()),
										.entries = bind_entries.data(),
									};
									auto uniform_bind_group = data->webgpu.device.CreateBindGroup(&bind_group_descriptor);

									data->webgpu.uniform_buffer = fae::uniform_buffer{
										.buffer = uniform_buffer,
										.size = uniform_buffer_size,
										.bind_group = uniform_bind_group,
									};

									delete data;
								},
								.userdata = new request_device_data{
									.plugin = data->plugin,
									.app = data->app,
									.webgpu = data->webgpu,
								},
							});
						delete data;
					},
					.userdata = new request_adapter_data{
						.plugin = *this,
						.app = app,
						.webgpu = webgpu,
					},
				});
		}
	};
} // namespace fae
