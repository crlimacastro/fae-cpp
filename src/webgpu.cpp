module;
#if defined(__EMSCRIPTEN__)
#include <emscripten/emscripten.h>
#else
#include <webgpu/webgpu_cpp.h>
#endif
#include <SDL3/SDL.h>
#include <format>
#include <string>
#if defined(SDL_PLATFORM_WIN32)
#include <Windows.h>
#endif

export module fae:webgpu;

import :logging;
import :application;
import :windowing;
import :sdl;

export namespace fae
{
    [[nodiscard]] auto to_string(WGPUErrorType value) noexcept -> std::string
    {
        switch (value)
        {
        case WGPUErrorType_NoError:
            return "NoError";
        case WGPUErrorType_Validation:
            return "Validation";
        case WGPUErrorType_OutOfMemory:
            return "OutOfMemory";
        case WGPUErrorType_Internal:
            return "Internal";
        case WGPUErrorType_Unknown:
            return "Unknown";
        case WGPUErrorType_DeviceLost:
            return "DeviceLost";
        case WGPUErrorType_Force32:
            return "Force32";
        }
    }

    auto get_sdl_webgpu_surface(WGPUInstance instance, SDL_Window *window) noexcept -> WGPUSurface
    {
#if defined(SDL_PLATFORM_WIN32)
        auto hwnd = (HWND)SDL_GetProperty(SDL_GetWindowProperties(window), SDL_PROP_WINDOW_WIN32_HWND_POINTER, nullptr);
        auto hinstance = (HINSTANCE)GetModuleHandle(nullptr);
        auto windows_surface_descriptor = WGPUSurfaceDescriptorFromWindowsHWND{
            .chain = WGPUChainedStruct{
                .sType = WGPUSType_SurfaceDescriptorFromWindowsHWND,
            },
            .hinstance = hinstance,
            .hwnd = hwnd,
        };
        auto surface_descriptor = WGPUSurfaceDescriptor{
            .nextInChain = (const WGPUChainedStruct *)&windows_surface_descriptor,
            .label = "windows_sdl_webgpu_surface",
        };
        return wgpuInstanceCreateSurface(instance, &surface_descriptor);
#elif defined(SDL_PLATFORM_MACOS)
        NSWindow *nswindow = (__bridge NSWindow *)SDL_GetProperty(SDL_GetWindowProperties(window), SDL_PROP_WINDOW_COCOA_WINDOW_POINTER, NULL);
        if (nswindow)
        {
            // TODO
        }
#elif defined(SDL_PLATFORM_LINUX)
        if (SDL_strcmp(SDL_GetCurrentVideoDriver(), "x11") == 0)
        {
            Display *xdisplay = (Display *)SDL_GetProperty(SDL_GetWindowProperties(window), SDL_PROP_WINDOW_X11_DISPLAY_POINTER, NULL);
            Window xwindow = (Window)SDL_GetNumberProperty(SDL_GetWindowProperties(window), SDL_PROP_WINDOW_X11_WINDOW_NUMBER, 0);
            if (xdisplay && xwindow)
            {
                // TODO
            }
        }
        else if (SDL_strcmp(SDL_GetCurrentVideoDriver(), "wayland") == 0)
        {
            struct wl_display *display = (struct wl_display *)SDL_GetProperty(SDL_GetWindowProperties(window), SDL_PROP_WINDOW_WAYLAND_DISPLAY_POINTER, NULL);
            struct wl_surface *surface = (struct wl_surface *)SDL_GetProperty(SDL_GetWindowProperties(window), SDL_PROP_WINDOW_WAYLAND_SURFACE_POINTER, NULL);
            if (display && surface)
            {
                // TODO
            }
        }
#error "unsupported platform"
#endif
    }

    struct webgpu
    {
        WGPUInstance instance;
        WGPUAdapter adapter;
        WGPUDevice device;
        WGPUSurface surface;
        WGPUSwapChain swapchain;
        WGPURenderPipeline render_pipeline;
    };

    struct webgpu_plugin
    {
        WGPUInstanceDescriptor instance_descriptor{};
        WGPURequestAdapterOptions adapter_options{};
        WGPUDeviceDescriptor device_descriptor{};
        WGPUErrorCallback error_callback = [](WGPUErrorType type, const char *message, void *userdata)
        {
            fae::log_error(std::format("[wgpu_error] type: {} - message: {}", to_string(type), message));
        };

        auto init(application &app) const noexcept -> void
        {
            app.add_plugin(windowing_plugin{});
            auto instance = wgpuCreateInstance(&instance_descriptor);
            struct request_adapter_data
            {
                const webgpu_plugin &plugin;
                application &app;
                WGPUInstance instance;
                WGPUErrorCallback error_callback;
            };
            wgpuInstanceRequestAdapter(
                instance, &adapter_options, [](WGPURequestAdapterStatus status, WGPUAdapter adapter, const char *message, void *userdata)
                {
                        if (status != WGPURequestAdapterStatus_Success)
                        {
                            fae::log_fatal(std::format("failed to request adapter: {}", message));
                        }
                        auto data = reinterpret_cast<request_adapter_data *>(userdata);
                        struct request_device_data
                        {
                            const webgpu_plugin &plugin;
                            application &app;
                            WGPUInstance instance;
                            WGPUErrorCallback error_callback;
                            WGPUAdapter adapter;
                        };
                        wgpuAdapterRequestDevice(adapter, &data->plugin.device_descriptor, [](WGPURequestDeviceStatus status, WGPUDevice device, char const *message, void *userdata)
                                {
                                    if (status != WGPURequestDeviceStatus_Success)
                                    {
                                        fae::log_fatal(std::format("failed to request device: ", message));
                                    }
                                    auto data = reinterpret_cast<request_device_data *>(userdata);
                                    wgpuDeviceSetUncapturedErrorCallback(device, data->error_callback, nullptr);
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
                                    auto sdl_window = *maybe_sdl_window;

#if defined(__EMSCRIPTEN__)
                                    WGPUSurfaceDescriptorFromCanvasHTMLSelector canvas_desc
                                    {
                                        .selector = "#fae-canvas",
                                    };
                                    WGPUSurfaceDescriptor surface_desc
                                    {
                                        .nextInChain = (const WGPUChainedStruct *)&canvas_desc,
                                    };
                                    auto surface = wgpuInstanceCreateSurface(data->instance, &surface_desc);
#else
                                    auto surface = get_sdl_webgpu_surface(data->instance, sdl_window.raw);
#endif
                                    auto window = primary_window.window();
                                    auto window_size = window.get_size();

                                    auto swapchain_descriptor = WGPUSwapChainDescriptor
                                    {
                                        .label = "fae_swapchain",
                                        .usage = WGPUTextureUsage_RenderAttachment,
                                        .format = wgpuSurfaceGetPreferredFormat(surface, data->adapter),
                                        .width = static_cast<std::uint32_t>(window_size.width),
                                        .height = static_cast<std::uint32_t>(window_size.height),
                                        .presentMode = WGPUPresentMode_Fifo,
                                    };
                                    auto swapchain = wgpuDeviceCreateSwapChain(device, surface, &swapchain_descriptor);

                                    WGPUShaderModuleWGSLDescriptor wgsl_desc
                                    {
                                        .chain = WGPUChainedStruct{
                                            .sType = WGPUSType_ShaderModuleWGSLDescriptor,
                                        },
                                        .code = R"(
@vertex
fn vs_main(@builtin(vertex_index) in_vertex_index: u32) -> @builtin(position) vec4f {
    const position = array(vec2f(0, 1), vec2f(-1, -1), vec2f(1, -1));
    return vec4f(position[in_vertex_index], 0, 1);
}

@fragment
fn fs_main() -> @location(0) vec4f {
	return vec4f(0.0, 0.4, 1.0, 1.0);
}
)",
                                    };
                                    WGPUShaderModuleDescriptor shader_module_descriptor
                                    {
                                        .nextInChain = (const WGPUChainedStruct *)&wgsl_desc,
                                    };
                                    WGPUShaderModule shader_module = wgpuDeviceCreateShaderModule(device, &shader_module_descriptor);
                                    auto blend_state = WGPUBlendState
	                                {
	                                	.color = WGPUBlendComponent
	                                	{
	                                		.operation = WGPUBlendOperation_Add,
	                                		.srcFactor = WGPUBlendFactor_SrcAlpha,
	                                		.dstFactor = WGPUBlendFactor_OneMinusSrcAlpha,
	                                	},
	                                	.alpha = WGPUBlendComponent
	                                	{
	                                		.operation = WGPUBlendOperation_Add,
	                                		.srcFactor = WGPUBlendFactor_Zero,
	                                		.dstFactor = WGPUBlendFactor_One,
	                                	},
	                                };
                                    WGPUColorTargetState color_target_state
                                    {
                                        .format = swapchain_descriptor.format,
                                        .blend = &blend_state,
                                        .writeMask = WGPUColorWriteMask_All,
                                    };
                                    WGPUFragmentState fragment_state
                                    {
                                        .module = shader_module,
                                        .entryPoint = "fs_main",
                                        .constantCount = 0,
                                        .constants = nullptr,
                                        .targetCount = 1,
                                        .targets = &color_target_state,
                                    };
                                    WGPURenderPipelineDescriptor pipeline_descriptor
                                    {
                                        .vertex = {
                                            .module = shader_module,
                                            .entryPoint = "vs_main",
                                            .constantCount = 0,
                                            .constants = nullptr,
                                            .bufferCount = 0,
                                            .buffers = nullptr,
                                        },
                                        .primitive = WGPUPrimitiveState
                                		{
                                			.topology = WGPUPrimitiveTopology_TriangleList,
                                			.stripIndexFormat = WGPUIndexFormat_Undefined,
                                			.frontFace = WGPUFrontFace_CCW,
                                			.cullMode = WGPUCullMode_None,
                                		},
                                        .multisample = {
                                            .count = 1,
                                            .mask = ~0u,
			                                .alphaToCoverageEnabled = false,
                                        },
                                        .fragment = &fragment_state,
                                    };
                                    auto render_pipeline = wgpuDeviceCreateRenderPipeline(device, &pipeline_descriptor);

                                    // TODO move this to renderer begin -> update -> end
                                    auto command_encoder_desc = WGPUCommandEncoderDescriptor{};
                                    auto command_encoder = wgpuDeviceCreateCommandEncoder(device, &command_encoder_desc);
                                    WGPURenderPassColorAttachment color_attachment
                                    {
                                        .view =  wgpuSwapChainGetCurrentTextureView(swapchain),
                                        .depthSlice = WGPU_DEPTH_SLICE_UNDEFINED,
                                        .resolveTarget = nullptr,
                                        .loadOp = WGPULoadOp_Clear,
                                        .storeOp = WGPUStoreOp_Store,
                                        .clearValue = WGPUColor{ 0.9, 0.1, 0.2, 1.0 }, // change to stored color of the renderer
                                    };
                                    WGPURenderPassDescriptor render_pass_desc
                                    {
                                        .colorAttachmentCount = 1,
                                        .colorAttachments = &color_attachment,
                                        .depthStencilAttachment = nullptr,
                                    };
                                    auto render_pass = wgpuCommandEncoderBeginRenderPass(command_encoder, &render_pass_desc);
                                    wgpuRenderPassEncoderSetPipeline(render_pass, render_pipeline);
                                    wgpuRenderPassEncoderDraw(render_pass, 3, 1, 0, 0);
                                    wgpuRenderPassEncoderEnd(render_pass);
                                    auto command_buffer_desc = WGPUCommandBufferDescriptor{};
                                    auto command_buffer = wgpuCommandEncoderFinish(command_encoder, &command_buffer_desc);
                                    auto queue = wgpuDeviceGetQueue(device);
                                    wgpuQueueSubmit(queue, 1, &command_buffer);
                                    wgpuSwapChainPresent(swapchain);
                                    wgpuInstanceProcessEvents(data->instance);
                                    // TODO END

                                    data->app.emplace_resource<webgpu>(webgpu{
                                        .adapter = data->adapter,
                                        .device = device,
                                        .surface = surface,
                                        .swapchain = swapchain,
                                        .render_pipeline = render_pipeline,
                                    });
                                    delete data;
                                }, new request_device_data{
                                    .plugin = data->plugin,
                                    .app = data->app,
                                    .instance = data->instance,
                                    .error_callback = data->error_callback,
                                    .adapter = adapter,
                                });
                        delete data; },
                new request_adapter_data{
                    .plugin = *this,
                    .app = app,
                    .instance = instance,
                    .error_callback = error_callback,
                });
        }
    };
}
