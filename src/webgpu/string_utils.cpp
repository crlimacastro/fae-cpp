#include "fae/webgpu/string_utils.hpp"

#include <sstream>

#include <webgpu/webgpu_cpp.h>

#include "fae/logging.hpp"
#include "fae/webgpu/utils.hpp"
#include "fae/core.hpp"

namespace fae
{
    [[nodiscard]] auto format(std::uint64_t num) noexcept -> std::string
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

    auto to_string(const wgpu::AdapterProperties& adapter_properties) noexcept -> std::string
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

    auto to_string(const wgpu::Limits& limits, const std::string& indent) noexcept -> std::string
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

    auto log_properties(const wgpu::Adapter& adapter) noexcept -> void
    {
        wgpu::AdapterProperties properties;
        adapter.GetProperties(&properties);
        log_info(to_string(properties));
    }

    auto log_features(const wgpu::Adapter& adapter) noexcept -> void
    {
        const auto features = get_features(adapter);
        log_info("Adapter Features:");
        for (const auto& f : features)
        {
            log_info("\t" + to_string(f));
        }
    }

    auto log_limits(const wgpu::Adapter& adapter) noexcept -> void
    {
        wgpu::SupportedLimits adapterLimits;
        if (adapter.GetLimits(&adapterLimits))
        {
            log_info("Adapter Limits:");
            log_info(to_string(adapterLimits.limits, "\t"));
        }
    }

    auto log_adapter_info(const wgpu::Adapter& adapter) noexcept -> void
    {
        log_properties(adapter);
        log_features(adapter);
        log_limits(adapter);
    }

        auto log_device_features(const wgpu::Device& device) noexcept -> void
    {
        const auto features = get_features(device);
        log_info("Device Features:");
        for (const auto& f : features)
        {
            log_info("\t" + to_string(f));
        }
    }

    auto log_limits(const wgpu::Device& device) noexcept -> void
    {
        wgpu::SupportedLimits deviceLimits;
        if (device.GetLimits(&deviceLimits))
        {
            log_info("Device Limits:");
            log_info(to_string(deviceLimits.limits, "\t"));
        }
    }

    auto log_device_info(wgpu::Device& device) noexcept -> void
    {
        log_device_features(device);
        log_limits(device);
    }
}
