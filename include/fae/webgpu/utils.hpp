#pragma once

#include <string_view>
#include <optional>
#include <filesystem>

#include <webgpu/webgpu_cpp.h>

#include "fae/rendering/texture.hpp"

namespace fae
{
    [[nodiscard]] auto request_adapter_sync(wgpu::Instance instance, wgpu::RequestAdapterOptions adapter_options = {}) noexcept -> wgpu::Adapter;
    [[nodiscard]] auto request_device_sync(wgpu::Adapter adapter, wgpu::DeviceDescriptor device_descriptor = {}) noexcept -> wgpu::Device;
    [[nodiscard]] auto get_features(const wgpu::Adapter& adapter) noexcept -> std::vector<wgpu::FeatureName>;
    [[nodiscard]] auto get_features(const wgpu::Device& device) noexcept -> std::vector<wgpu::FeatureName>;
    [[nodiscard]] wgpu::Buffer create_buffer(const wgpu::Device& device,
        std::string_view label,
        std::size_t size,
        wgpu::BufferUsage usage);
    [[nodiscard]] wgpu::Buffer create_buffer_with_data(const wgpu::Device& device,
        std::string_view label,
        const void* data,
        std::size_t size,
        wgpu::BufferUsage usage);
    [[nodiscard]] wgpu::ShaderModule create_shader_module_from_str(const wgpu::Device& device,
        std::string_view label,
        std::string_view src);
    [[nodiscard]] std::optional<wgpu::ShaderModule> create_shader_module_from_path(const wgpu::Device& device,
        std::string_view label,
        const std::filesystem::path& path);
    [[nodiscard]] wgpu::Texture create_texture(const wgpu::Device& device,
        std::string_view label,
        wgpu::Extent3D extent,
        wgpu::TextureFormat format,
        wgpu::TextureUsage usage);

    struct texture_and_view
    {
        wgpu::Texture texture;
        wgpu::TextureView view;
    };
    [[nodiscard]] texture_and_view create_texture_with_mips_and_view(const wgpu::Device& device,
        texture texture);
}
