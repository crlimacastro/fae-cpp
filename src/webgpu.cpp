module;
#ifdef __EMSCRIPTEN__
#include <emscripten/emscripten.h>
#else
#include <webgpu/webgpu_cpp.h>
#endif
#include <SDL3/SDL.h>
#include <format>
#include <string>
#ifdef SDL_PLATFORM_WIN32
#include <Windows.h>
#endif
#include <ostream>

export module fae:webgpu;

import :logging;
import :application;
import :windowing;
import :sdl;

export namespace fae
{
    [[nodiscard]] auto to_string(wgpu::FeatureName feature_name) noexcept -> std::string
    {

        switch (feature_name)
        {
        case wgpu::FeatureName::DepthClipControl:
            return "DepthClipControl";
        case wgpu::FeatureName::Depth32FloatStencil8:
            return "Depth32FloatStencil8";
        case wgpu::FeatureName::TimestampQuery:
            return "TimestampQuery";
        case wgpu::FeatureName::TextureCompressionBC:
            return "TextureCompressionBC";
        case wgpu::FeatureName::TextureCompressionETC2:
            return "TextureCompressionETC2";
        case wgpu::FeatureName::TextureCompressionASTC:
            return "TextureCompressionASTC";
        case wgpu::FeatureName::IndirectFirstInstance:
            return "IndirectFirstInstance";
        case wgpu::FeatureName::ShaderF16:
            return "ShaderF16";
        case wgpu::FeatureName::RG11B10UfloatRenderable:
            return "RG11B10UfloatRenderable";
        case wgpu::FeatureName::BGRA8UnormStorage:
            return "BGRA8UnormStorage";
        case wgpu::FeatureName::Float32Filterable:
            return "Float32Filterable";
        case wgpu::FeatureName::DawnInternalUsages:
            return "DawnInternalUsages";
        case wgpu::FeatureName::DawnMultiPlanarFormats:
            return "DawnMultiPlanarFormats";
        case wgpu::FeatureName::DawnNative:
            return "DawnNative";
        case wgpu::FeatureName::ChromiumExperimentalTimestampQueryInsidePasses:
            return "ChromiumExperimentalTimestampQueryInsidePasses";
        case wgpu::FeatureName::ImplicitDeviceSynchronization:
            return "ImplicitDeviceSynchronization";
        case wgpu::FeatureName::SurfaceCapabilities:
            return "SurfaceCapabilities";
        case wgpu::FeatureName::TransientAttachments:
            return "TransientAttachments";
        case wgpu::FeatureName::MSAARenderToSingleSampled:
            return "MSAARenderToSingleSampled";
        case wgpu::FeatureName::DualSourceBlending:
            return "DualSourceBlending";
        case wgpu::FeatureName::D3D11MultithreadProtected:
            return "D3D11MultithreadProtected";
        case wgpu::FeatureName::ANGLETextureSharing:
            return "ANGLETextureSharing";
        case wgpu::FeatureName::ChromiumExperimentalSubgroups:
            return "ChromiumExperimentalSubgroups";
        case wgpu::FeatureName::ChromiumExperimentalSubgroupUniformControlFlow:
            return "ChromiumExperimentalSubgroupUniformControlFlow";
        case wgpu::FeatureName::PixelLocalStorageCoherent:
            return "PixelLocalStorageCoherent";
        case wgpu::FeatureName::PixelLocalStorageNonCoherent:
            return "PixelLocalStorageNonCoherent";
        case wgpu::FeatureName::Unorm16TextureFormats:
            return "Unorm16TextureFormats";
        case wgpu::FeatureName::Snorm16TextureFormats:
            return "Snorm16TextureFormats";
        case wgpu::FeatureName::MultiPlanarFormatExtendedUsages:
            return "MultiPlanarFormatExtendedUsages";
        case wgpu::FeatureName::MultiPlanarFormatP010:
            return "MultiPlanarFormatP010";
        case wgpu::FeatureName::HostMappedPointer:
            return "HostMappedPointer";
        case wgpu::FeatureName::MultiPlanarRenderTargets:
            return "MultiPlanarRenderTargets";
        case wgpu::FeatureName::MultiPlanarFormatNv12a:
            return "MultiPlanarFormatNv12a";
        case wgpu::FeatureName::FramebufferFetch:
            return "FramebufferFetch";
        case wgpu::FeatureName::BufferMapExtendedUsages:
            return "BufferMapExtendedUsages";
        case wgpu::FeatureName::AdapterPropertiesMemoryHeaps:
            return "AdapterPropertiesMemoryHeaps";
        case wgpu::FeatureName::AdapterPropertiesD3D:
            return "AdapterPropertiesD3D";
        case wgpu::FeatureName::AdapterPropertiesVk:
            return "AdapterPropertiesVk";
        case wgpu::FeatureName::R8UnormStorage:
            return "R8UnormStorage";
        case wgpu::FeatureName::FormatCapabilities:
            return "FormatCapabilities";
        case wgpu::FeatureName::DrmFormatCapabilities:
            return "DrmFormatCapabilities";
        case wgpu::FeatureName::Norm16TextureFormats:
            return "Norm16TextureFormats";
        case wgpu::FeatureName::SharedTextureMemoryVkDedicatedAllocation:
            return "SharedTextureMemoryVkDedicatedAllocation";
        case wgpu::FeatureName::SharedTextureMemoryAHardwareBuffer:
            return "SharedTextureMemoryAHardwareBuffer";
        case wgpu::FeatureName::SharedTextureMemoryDmaBuf:
            return "SharedTextureMemoryDmaBuf";
        case wgpu::FeatureName::SharedTextureMemoryOpaqueFD:
            return "SharedTextureMemoryOpaqueFD";
        case wgpu::FeatureName::SharedTextureMemoryZirconHandle:
            return "SharedTextureMemoryZirconHandle";
        case wgpu::FeatureName::SharedTextureMemoryDXGISharedHandle:
            return "SharedTextureMemoryDXGISharedHandle";
        case wgpu::FeatureName::SharedTextureMemoryD3D11Texture2D:
            return "SharedTextureMemoryD3D11Texture2D";
        case wgpu::FeatureName::SharedTextureMemoryIOSurface:
            return "SharedTextureMemoryIOSurface";
        case wgpu::FeatureName::SharedTextureMemoryEGLImage:
            return "SharedTextureMemoryEGLImage";
        case wgpu::FeatureName::SharedFenceVkSemaphoreOpaqueFD:
            return "SharedFenceVkSemaphoreOpaqueFD";
        case wgpu::FeatureName::SharedFenceVkSemaphoreSyncFD:
            return "SharedFenceVkSemaphoreSyncFD";
        case wgpu::FeatureName::SharedFenceVkSemaphoreZirconHandle:
            return "SharedFenceVkSemaphoreZirconHandle";
        case wgpu::FeatureName::SharedFenceDXGISharedHandle:
            return "SharedFenceDXGISharedHandle";
        case wgpu::FeatureName::SharedFenceMTLSharedEvent:
            return "SharedFenceMTLSharedEvent";
        case wgpu::FeatureName::SharedBufferMemoryD3D12Resource:
            return "SharedBufferMemoryD3D12Resource";
        case wgpu::FeatureName::StaticSamplers:
            return "StaticSamplers";
        case wgpu::FeatureName::YCbCrVulkanSamplers:
            return "YCbCrVulkanSamplers";
        case wgpu::FeatureName::ShaderModuleCompilationOptions:
            return "ShaderModuleCompilationOptions";
        case wgpu::FeatureName::Undefined:
            break;
        }
        return "Undefined";
    }

    [[nodiscard]] auto to_string(wgpu::AdapterType adapter_type) noexcept -> std::string
    {
        switch (adapter_type)
        {
        case wgpu::AdapterType::DiscreteGPU:
            return "DiscreteGPU";
        case wgpu::AdapterType::IntegratedGPU:
            return "IntegratedGPU";
        case wgpu::AdapterType::CPU:
            return "CPU";
        case wgpu::AdapterType::Unknown:
            break;
        }
        return "Unknown";
    }

    [[nodiscard]] auto to_string(wgpu::BackendType backend_type) noexcept -> std::string
    {
        switch (backend_type)
        {
        case wgpu::BackendType::Null:
            return "Null";
        case wgpu::BackendType::WebGPU:
            return "WebGPU";
        case wgpu::BackendType::D3D11:
            return "D3D11";
        case wgpu::BackendType::D3D12:
            return "D3D12";
        case wgpu::BackendType::Metal:
            return "Metal";
        case wgpu::BackendType::Vulkan:
            return "Vulkan";
        case wgpu::BackendType::OpenGL:
            return "OpenGL";
        case wgpu::BackendType::OpenGLES:
            return "OpenGLES";
        case wgpu::BackendType::Undefined:
            break;
        }
        return "Undefined";
    }

    [[nodiscard]] auto to_string(const wgpu::AdapterProperties &adapter_properties) noexcept -> std::string
    {
        std::stringstream out;

        out << "VendorID: " << adapter_properties.vendorID << std::endl;
        out << "Vendor: " << adapter_properties.vendorName << std::endl;
        out << "Architecture: " << adapter_properties.architecture << std::endl;
        out << "DeviceID: " << adapter_properties.deviceID << std::endl;
        out << "Name: " << adapter_properties.name << std::endl;
        out << "Driver description: " << adapter_properties.driverDescription << std::endl;
        out << "Adapter Type: " << to_string(adapter_properties.adapterType)
            << std::endl;
        out << "Backend Type: " << to_string(adapter_properties.backendType)
            << std::endl;
        return out.str();
    }

    [[nodiscard]] auto format(uint64_t num) noexcept -> std::string
    {
        auto s = std::to_string(num);
        std::stringstream ret;

        auto remainder = s.length() % 3;
        ret << s.substr(0, remainder);
        for (size_t i = remainder; i < s.length(); i += 3)
        {
            if (i > 0)
            {
                ret << ",";
            }
            ret << s.substr(i, 3);
        }
        return ret.str();
    }

    [[nodiscard]] auto to_string(const wgpu::Limits &limits, const std::string &indent) noexcept -> std::string
    {
        std::stringstream out;

        out << indent
            << "maxTextureDimension1D: " << format(limits.maxTextureDimension1D)
            << std::endl;
        out << indent
            << "maxTextureDimension2D: " << format(limits.maxTextureDimension2D)
            << std::endl;
        out << indent
            << "maxTextureDimension3D: " << format(limits.maxTextureDimension3D)
            << std::endl;
        out << indent
            << "maxTextureArrayLayers: " << format(limits.maxTextureArrayLayers)
            << std::endl;
        out << indent << "maxBindGroups: " << format(limits.maxBindGroups)
            << std::endl;
        out << indent << "maxDynamicUniformBuffersPerPipelineLayout: "
            << format(limits.maxDynamicUniformBuffersPerPipelineLayout)
            << std::endl;
        out << indent << "maxDynamicStorageBuffersPerPipelineLayout: "
            << format(limits.maxDynamicStorageBuffersPerPipelineLayout)
            << std::endl;
        out << indent << "maxSampledTexturesPerShaderStage: "
            << format(limits.maxSampledTexturesPerShaderStage) << std::endl;
        out << indent << "maxSamplersPerShaderStage: "
            << format(limits.maxSamplersPerShaderStage) << std::endl;
        out << indent << "maxStorageBuffersPerShaderStage: "
            << format(limits.maxStorageBuffersPerShaderStage) << std::endl;
        out << indent << "maxStorageTexturesPerShaderStage: "
            << format(limits.maxStorageTexturesPerShaderStage) << std::endl;
        out << indent << "maxUniformBuffersPerShaderStage: "
            << format(limits.maxUniformBuffersPerShaderStage) << std::endl;
        out << indent << "maxUniformBufferBindingSize: "
            << format(limits.maxUniformBufferBindingSize) << std::endl;
        out << indent << "maxStorageBufferBindingSize: "
            << format(limits.maxStorageBufferBindingSize) << std::endl;
        out << indent << "minUniformBufferOffsetAlignment: "
            << format(limits.minUniformBufferOffsetAlignment) << std::endl;
        out << indent << "minStorageBufferOffsetAlignment: "
            << format(limits.minStorageBufferOffsetAlignment) << std::endl;
        out << indent << "maxVertexBuffers: " << format(limits.maxVertexBuffers)
            << std::endl;
        out << indent
            << "maxVertexAttributes: " << format(limits.maxVertexAttributes)
            << std::endl;
        out << indent << "maxVertexBufferArrayStride: "
            << format(limits.maxVertexBufferArrayStride) << std::endl;
        out << indent << "maxInterStageShaderComponents: "
            << format(limits.maxInterStageShaderComponents) << std::endl;
        out << indent << "maxInterStageShaderVariables: "
            << format(limits.maxInterStageShaderVariables) << std::endl;
        out << indent
            << "maxColorAttachments: " << format(limits.maxColorAttachments)
            << std::endl;
        out << indent << "maxComputeWorkgroupStorageSize: "
            << format(limits.maxComputeWorkgroupStorageSize) << std::endl;
        out << indent << "maxComputeInvocationsPerWorkgroup: "
            << format(limits.maxComputeInvocationsPerWorkgroup) << std::endl;
        out << indent << "maxComputeWorkgroupSizeX: "
            << format(limits.maxComputeWorkgroupSizeX) << std::endl;
        out << indent << "maxComputeWorkgroupSizeY: "
            << format(limits.maxComputeWorkgroupSizeY) << std::endl;
        out << indent << "maxComputeWorkgroupSizeZ: "
            << format(limits.maxComputeWorkgroupSizeZ) << std::endl;
        out << indent << "maxComputeWorkgroupsPerDimension: "
            << format(limits.maxComputeWorkgroupsPerDimension) << std::endl;

        return out.str();
    }

    auto log_properties(const wgpu::Adapter &adapter) noexcept -> void
    {
        wgpu::AdapterProperties properties;
        adapter.GetProperties(&properties);
        log_info(to_string(properties));
    }

    [[nodiscard]] auto get_features(const wgpu::Adapter &adapter) noexcept -> std::vector<wgpu::FeatureName>
    {
        const auto feature_count = adapter.EnumerateFeatures(nullptr);
        std::vector<wgpu::FeatureName> features(feature_count);
        adapter.EnumerateFeatures(features.data());
        return features;
    }

    auto log_features(const wgpu::Adapter &adapter) noexcept -> void
    {
        const auto features = get_features(adapter);
        log_info("Adapter Features:");
        for (const auto &f : features)
        {
            log_info("\t" + to_string(f));
        }
    }

    auto log_limits(const wgpu::Adapter &adapter) noexcept -> void
    {
        wgpu::SupportedLimits adapterLimits;
        if (adapter.GetLimits(&adapterLimits))
        {
            log_info("Adapter Limits:");
            log_info(to_string(adapterLimits.limits, "\t"));
        }
    }

    auto log_adapter_info(const wgpu::Adapter &adapter) noexcept -> void
    {
        log_properties(adapter);
        log_features(adapter);
        log_limits(adapter);
    }

    [[nodiscard]] auto get_features(const wgpu::Device &device) noexcept -> std::vector<wgpu::FeatureName>
    {
        const auto feature_count = device.EnumerateFeatures(nullptr);
        std::vector<wgpu::FeatureName> features(feature_count);
        device.EnumerateFeatures(features.data());
        return features;
    }

    auto log_device_features(const wgpu::Device &device) noexcept -> void
    {
        const auto features = get_features(device);
        log_info("Device Features:");
        for (const auto &f : features)
        {
            log_info("\t" + to_string(f));
        }
    }

    auto log_limits(const wgpu::Device &device) noexcept -> void
    {
        wgpu::SupportedLimits deviceLimits;
        if (device.GetLimits(&deviceLimits))
        {
            log_info("Device Limits:");
            log_info(to_string(deviceLimits.limits, "\t"));
        }
    }

    auto log_device_info(wgpu::Device &device) noexcept -> void
    {
        log_device_features(device);
        log_limits(device);
    }

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
        auto surface_descriptor = WGPUSurfaceDescriptor{};
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
        surface_descriptor.nextInChain = (const WGPUChainedStruct *)&windows_surface_descriptor;
        surface_descriptor.label = "windows_sdl_webgpu_surface";
#elif defined(SDL_PLATFORM_)
#elif defined(SDL_PLATFORM_MACOS)
        NSWindow *nswindow = (__bridge NSWindow *)SDL_GetProperty(SDL_GetWindowProperties(window), SDL_PROP_WINDOW_COCOA_WINDOW_POINTER, NULL);
        if (nswindow)
        {
            auto macos_surface_descriptor = WGPUSurfaceDescriptorFromMetalLayer{
                .chain = WGPUChainedStruct{
                    .sType = WGPUSType_SurfaceDescriptorFromMetalLayer,
                },
                .layer = CAMetalLayer * nswindow->contentView.layer,
            };
        }
        surface_descriptor.nextInChain = (const WGPUChainedStruct *)&macos_surface_descriptor;
        surface_descriptor.label = "macos_sdl_webgpu_surface";
    }
#elif defined(SDL_PLATFORM_LINUX)
        if (SDL_strcmp(SDL_GetCurrentVideoDriver(), "x11") == 0)
        {
            Display *xdisplay = (Display *)SDL_GetProperty(SDL_GetWindowProperties(window), SDL_PROP_WINDOW_X11_DISPLAY_POINTER, NULL);
            Window xwindow = (Window)SDL_GetNumberProperty(SDL_GetWindowProperties(window), SDL_PROP_WINDOW_X11_WINDOW_NUMBER, 0);
            if (xdisplay && xwindow)
            {
                auto linux_x11_surface_descriptor = WGPUSurfaceDescriptorFromXlib{
                    .chain = WGPUChainedStruct{
                        .sType = WGPUSType_SurfaceDescriptorFromXlib,
                    },
                    .display = xdisplay,
                    .window = xwindow,
                };
                surface_descriptor.nextInChain = (const WGPUChainedStruct *)&linux_x11_surface_descriptor;
                surface_descriptor.label = "linux_x11_sdl_webgpu_surface";
            }
        }
        else if (SDL_strcmp(SDL_GetCurrentVideoDriver(), "wayland") == 0)
        {
            struct wl_display *display = (struct wl_display *)SDL_GetProperty(SDL_GetWindowProperties(window), SDL_PROP_WINDOW_WAYLAND_DISPLAY_POINTER, NULL);
            struct wl_surface *surface = (struct wl_surface *)SDL_GetProperty(SDL_GetWindowProperties(window), SDL_PROP_WINDOW_WAYLAND_SURFACE_POINTER, NULL);
            if (display && surface)
            {
                auto linux_wayland_surface_descriptor = WGPUSurfaceDescriptorFromWayland{
                    .chain = WGPUChainedStruct{
                        .sType = WGPUSType_SurfaceDescriptorFromWayland,
                    },
                    .display = display,
                    .surface = surface,
                };
                surface_descriptor.nextInChain = (const WGPUChainedStruct *)&linux_wayland_surface_descriptor;
                surface_descriptor.label = "linux_wayland_sdl_webgpu_surface";
            }
        }
#error "unsupported platform"
#endif
        return wgpuInstanceCreateSurface(instance, &surface_descriptor);
    }

    struct webgpu
    {
        WGPUInstance instance;
        WGPUAdapter adapter;
        WGPUDevice device;
        WGPUSurface surface;
        WGPUSwapChain swapchain;
        WGPURenderPipeline render_pipeline;

        WGPUColor clear_color = {0, 0, 0, 1};
        struct current_render
        {
            WGPURenderPassEncoder render_pass;
            WGPUCommandEncoder command_encoder;
        };
        current_render current_render{};
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
                                        .selector = "#canvas",
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
	return vec4f(1.0, 1.0, 1.0, 1.0);
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

                                    data->app.emplace_resource<webgpu>(webgpu{
                                        .instance = data->instance,
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
