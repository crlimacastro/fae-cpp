#pragma once

#include <string>
#include <cstdint>

#include <webgpu/webgpu_cpp.h>

namespace fae
{
    [[nodiscard]] auto to_string(const wgpu::Limits& limits, const std::string& indent) noexcept -> std::string;
    auto log_properties(const wgpu::Adapter& adapter) noexcept -> void;
    auto log_features(const wgpu::Adapter& adapter) noexcept -> void;
    auto log_limits(const wgpu::Adapter& adapter) noexcept -> void;
    auto log_adapter_info(const wgpu::Adapter& adapter) noexcept -> void;
    auto log_device_features(const wgpu::Device& device) noexcept -> void;
    auto log_limits(const wgpu::Device& device) noexcept -> void;
    auto log_device_info(wgpu::Device& device) noexcept -> void;
}
