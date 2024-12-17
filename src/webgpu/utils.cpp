#include "fae/webgpu/utils.hpp"

#include <fstream>
#include <string>
#include <bit>

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
                .callback = [](WGPURequestAdapterStatus cStatus, WGPUAdapter cAdapter, WGPUStringView message, void* userdata)
                {
                    const auto status = static_cast<wgpu::RequestAdapterStatus>(cStatus);
                    if (status != wgpu::RequestAdapterStatus::Success)
                    {
                        fae::log_fatal(std::format("failed to request adapter: {}", message.data));
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
            emscripten_sleep(1);
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
                .callback = [](WGPURequestDeviceStatus cStatus, WGPUDevice cDevice, WGPUStringView message, void* userdata)
                {
                    const auto status = static_cast<wgpu::RequestDeviceStatus>(cStatus);
                    if (status != wgpu::RequestDeviceStatus::Success)
                    {
                        fae::log_fatal(std::format("failed to request device: ", message.data));
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

    [[nodiscard]] texture_and_view create_texture_with_mips_and_view(const wgpu::Device& device,
        texture texture)
    {
        auto texture_desc = wgpu::TextureDescriptor{
            .usage = wgpu::TextureUsage::CopyDst | wgpu::TextureUsage::TextureBinding,
            .dimension = wgpu::TextureDimension::e2D,
            .size = { static_cast<std::uint32_t>(texture.width), static_cast<std::uint32_t>(texture.height), 1 },
            .format = wgpu::TextureFormat::RGBA8Unorm,
            .mipLevelCount = static_cast<std::uint32_t>(std::bit_width(std::max(texture.width, texture.height))),
            .sampleCount = 1,
            .viewFormatCount = 0,
            .viewFormats = nullptr,
        };

        auto wgpu_texture = device.CreateTexture(&texture_desc);

        auto texture_view_desc = wgpu::TextureViewDescriptor{
            .format = wgpu::TextureFormat::RGBA8Unorm,
            .dimension = wgpu::TextureViewDimension::e2D,
            .baseMipLevel = 0,
            .mipLevelCount = texture_desc.mipLevelCount,
            .baseArrayLayer = 0,
            .arrayLayerCount = 1,
            .aspect = wgpu::TextureAspect::All,
        };
        auto texture_view = wgpu_texture.CreateView(&texture_view_desc);

        // write mipmaps
        auto source = wgpu::TextureDataLayout{
            .offset = 0,
            .bytesPerRow = static_cast<std::uint32_t>(4 * texture.width),
            .rowsPerImage = static_cast<std::uint32_t>(texture.height),
        };

        auto destination = wgpu::ImageCopyTexture{
            .texture = wgpu_texture,
            .mipLevel = 0,
            .origin = { 0, 0, 0 },
            .aspect = wgpu::TextureAspect::All,
        };

        auto queue = device.GetQueue();

        wgpu::Extent3D mipLevelSize = texture_desc.size;
        std::vector<unsigned char> previousLevelPixels;
        wgpu::Extent3D previousMipLevelSize;
        for (uint32_t level = 0; level < texture_desc.mipLevelCount; ++level)
        {
            // Pixel data for the current level
            std::vector<unsigned char> pixels(4 * mipLevelSize.width * mipLevelSize.height);
            if (level == 0)
            {
                // We cannot really avoid this copy since we need this
                // in previousLevelPixels at the next iteration
                memcpy(pixels.data(), texture.data.data(), pixels.size());
            }
            else
            {
                // Create mip level data
                for (uint32_t i = 0; i < mipLevelSize.width; ++i)
                {
                    for (uint32_t j = 0; j < mipLevelSize.height; ++j)
                    {
                        unsigned char* p = &pixels[4 * (j * mipLevelSize.width + i)];
                        // Get the corresponding 4 pixels from the previous level
                        unsigned char* p00 = &previousLevelPixels[4 * ((2 * j + 0) * previousMipLevelSize.width + (2 * i + 0))];
                        unsigned char* p01 = &previousLevelPixels[4 * ((2 * j + 0) * previousMipLevelSize.width + (2 * i + 1))];
                        unsigned char* p10 = &previousLevelPixels[4 * ((2 * j + 1) * previousMipLevelSize.width + (2 * i + 0))];
                        unsigned char* p11 = &previousLevelPixels[4 * ((2 * j + 1) * previousMipLevelSize.width + (2 * i + 1))];
                        // Average
                        p[0] = (p00[0] + p01[0] + p10[0] + p11[0]) / 4;
                        p[1] = (p00[1] + p01[1] + p10[1] + p11[1]) / 4;
                        p[2] = (p00[2] + p01[2] + p10[2] + p11[2]) / 4;
                        p[3] = (p00[3] + p01[3] + p10[3] + p11[3]) / 4;
                    }
                }
            }

            // Upload data to the GPU texture
            destination.mipLevel = level;
            source.bytesPerRow = 4 * mipLevelSize.width;
            source.rowsPerImage = mipLevelSize.height;
            queue.WriteTexture(&destination, pixels.data(), pixels.size(), &source, &mipLevelSize);

            previousLevelPixels = std::move(pixels);
            previousMipLevelSize = mipLevelSize;
            mipLevelSize.width /= 2;
            mipLevelSize.height /= 2;
        }

        return texture_and_view{
            .texture = wgpu_texture,
            .view = texture_view,
        };
    }
}
