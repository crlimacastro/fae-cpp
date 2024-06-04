#pragma once

#include <string>
#include <cstdint>

#include <webgpu/webgpu_cpp.h>

namespace fae
{
    [[nodiscard]] auto to_string(wgpu::FeatureName feature_name) noexcept -> std::string;
    [[nodiscard]] auto to_string(wgpu::AdapterType adapter_type) noexcept -> std::string;
    [[nodiscard]] auto to_string(wgpu::BackendType backend_type) noexcept -> std::string;
    [[nodiscard]] auto to_string(const wgpu::AdapterProperties& adapter_properties) noexcept -> std::string;
    [[nodiscard]] auto format(std::uint64_t num) noexcept -> std::string;
    [[nodiscard]] auto to_string(const wgpu::Limits& limits, const std::string& indent) noexcept -> std::string;
    auto log_properties(const wgpu::Adapter& adapter) noexcept -> void;
    auto log_features(const wgpu::Adapter& adapter) noexcept -> void;
    auto log_limits(const wgpu::Adapter& adapter) noexcept -> void;
    auto log_adapter_info(const wgpu::Adapter& adapter) noexcept -> void;
    auto log_device_features(const wgpu::Device& device) noexcept -> void;
    auto log_limits(const wgpu::Device& device) noexcept -> void;
    auto log_device_info(wgpu::Device& device) noexcept -> void;
#ifndef FAE_PLATFORM_WEB
    [[nodiscard]] auto to_string(wgpu::LoggingType value) noexcept -> std::string;
#endif
    [[nodiscard]] auto to_string(wgpu::ErrorType value) noexcept -> std::string;
    [[nodiscard]] auto to_string(wgpu::DeviceLostReason value) noexcept -> std::string;
}
