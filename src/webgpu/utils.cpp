#include "fae/webgpu/utils.hpp"

#include <fstream>
#include <string>

#ifdef FAE_PLATFORM_WEB
#include <emscripten/emscripten.h>
#endif

#include "fae/logging.hpp"


namespace fae
{
    auto request_adapter_sync(wgpu::Instance instance, wgpu::RequestAdapterOptions adapter_options) noexcept -> wgpu::Adapter
    {
        struct request_adapter_data
        {
            wgpu::Adapter adapter;
#ifdef FAE_PLATFORM_WEB
            bool done = false;
#endif
        };
        auto data = new request_adapter_data{};
#ifndef FAE_PLATFORM_WEB
        instance.RequestAdapter(&adapter_options,
            wgpu::RequestAdapterCallbackInfo{
                .mode = wgpu::CallbackMode::AllowSpontaneous,
                .callback = [](WGPURequestAdapterStatus cStatus, WGPUAdapter cAdapter, const char* message, void* userdata)
                {
                    const auto status = static_cast<wgpu::RequestAdapterStatus>(cStatus);
                    if (status != wgpu::RequestAdapterStatus::Success)
                    {
                        fae::log_fatal(std::format("failed to request adapter: {}", message));
                    }
                    auto data = reinterpret_cast<request_adapter_data*>(userdata);
                    data->adapter = wgpu::Adapter::Acquire(cAdapter);
                },
                .userdata = data,
            });
#else
        instance.RequestAdapter(&adapter_options, [](WGPURequestAdapterStatus cStatus, WGPUAdapter cAdapter, char const* message, void* userdata)
            {
                const auto status = static_cast<wgpu::RequestAdapterStatus>(cStatus);
                if (status != wgpu::RequestAdapterStatus::Success)
                {
                    fae::log_fatal(std::format("failed to request adapter: {}", message));
                }
                auto data = reinterpret_cast<request_adapter_data*>(userdata);
                data->adapter = wgpu::Adapter::Acquire(cAdapter);
                data->done = true; }, data);
        while (!data->done)
        {
            emscripten_sleep(10);
        }
#endif
        auto adapter = data->adapter;
        delete data;
        return adapter;
    }

    auto request_device_sync(wgpu::Adapter adapter, wgpu::DeviceDescriptor device_descriptor) noexcept -> wgpu::Device
    {
        struct request_device_data
        {
            wgpu::Device device;
#ifdef FAE_PLATFORM_WEB
            bool done = false;
#endif
        };
        auto data = new request_device_data{};
#ifndef FAE_PLATFORM_WEB
        adapter.RequestDevice(&device_descriptor,
            wgpu::RequestDeviceCallbackInfo{
                .mode = wgpu::CallbackMode::AllowSpontaneous,
                .callback = [](WGPURequestDeviceStatus cStatus, WGPUDevice cDevice, char const* message, void* userdata)
                {
                    const auto status = static_cast<wgpu::RequestDeviceStatus>(cStatus);
                    if (status != wgpu::RequestDeviceStatus::Success)
                    {
                        fae::log_fatal(std::format("failed to request device: ", message));
                    }
                    auto data = reinterpret_cast<request_device_data*>(userdata);
                    data->device = wgpu::Device::Acquire(cDevice);
                },
                .userdata = data,
            });
#else
        adapter.RequestDevice(&device_descriptor, [](WGPURequestDeviceStatus cStatus, WGPUDevice cDevice, char const* message, void* userdata)
            {
                const auto status = static_cast<wgpu::RequestDeviceStatus>(cStatus);
                if (status != wgpu::RequestDeviceStatus::Success)
                {
                    fae::log_fatal(std::format("failed to request device: ", message));
                }
                auto data = reinterpret_cast<request_device_data*>(userdata);
                data->device = wgpu::Device::Acquire(cDevice);
                data->done = true; }, data);
        while (!data->done)
        {
            emscripten_sleep(10);
        }
#endif
        auto device = data->device;
        delete data;
        return device;
    }

    auto get_features(const wgpu::Adapter& adapter) noexcept -> std::vector<wgpu::FeatureName>
    {
        const auto feature_count = adapter.EnumerateFeatures(nullptr);
        std::vector<wgpu::FeatureName> features(feature_count);
        adapter.EnumerateFeatures(features.data());
        return features;
    }

    auto get_features(const wgpu::Device& device) noexcept -> std::vector<wgpu::FeatureName>
    {
        const auto feature_count = device.EnumerateFeatures(nullptr);
        std::vector<wgpu::FeatureName> features(feature_count);
        device.EnumerateFeatures(features.data());
        return features;
    }

    wgpu::Buffer create_buffer(const wgpu::Device& device,
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

    wgpu::Buffer create_buffer_with_data(const wgpu::Device& device,
        std::string_view label,
        const void* data,
        std::size_t size,
        wgpu::BufferUsage usage)
    {
        auto buffer = create_buffer(device, label, size, usage);
        device.GetQueue().WriteBuffer(buffer, 0, data, size);
        return buffer;
    }

    wgpu::ShaderModule create_shader_module_from_str(const wgpu::Device& device,
        std::string_view label,
        std::string_view src)
    {
        wgpu::ShaderModuleWGSLDescriptor wgsl_desc;
        wgsl_desc.code = src.data();
        wgpu::ShaderModuleDescriptor desc{
            .nextInChain = &wgsl_desc,
            .label = label.data(),
        };
        auto shader = device.CreateShaderModule(&desc);
        return shader;
    }

    std::optional<wgpu::ShaderModule> create_shader_module_from_path(const wgpu::Device& device,
        std::string_view label,
        const std::filesystem::path& path)
        {
            auto file = std::ifstream(path);
            if (!file.is_open())
            {
                return std::nullopt;
            }
            file.seekg(0, std::ios::end);
            std::size_t size = file.tellg();
            auto shader_src = std::string(size, ' ');
            file.seekg(0);
            file.read(shader_src.data(), size);

            return create_shader_module_from_str(device, label, shader_src);
        }

    wgpu::Texture create_texture(const wgpu::Device& device,
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
}