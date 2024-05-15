module;

#include "fae/webgpu.hpp"
#include <string>
#include <sstream>

export module fae:webgpu.string_utils;

import :logging;

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
		case wgpu::FeatureName::DawnLoadResolveTexture:
			return "DawnLoadResolveTexture";
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

	[[nodiscard]] auto to_string(wgpu::LoggingType value) noexcept -> std::string
	{
		switch (value)
		{
		case wgpu::LoggingType::Verbose:
			return "Verbose";
		case wgpu::LoggingType::Info:
			return "Info";
		case wgpu::LoggingType::Warning:
			return "Warning";
		case wgpu::LoggingType::Error:
			return "Error";
		}
	}

	[[nodiscard]] auto to_string(wgpu::ErrorType value) noexcept -> std::string
	{
		switch (value)
		{
		case wgpu::ErrorType::NoError:
			return "NoError";
		case wgpu::ErrorType::Validation:
			return "Validation";
		case wgpu::ErrorType::OutOfMemory:
			return "OutOfMemory";
		case wgpu::ErrorType::Internal:
			return "Internal";
		case wgpu::ErrorType::Unknown:
			return "Unknown";
		case wgpu::ErrorType::DeviceLost:
			return "DeviceLost";
		}
	}

	[[nodiscard]] auto to_string(wgpu::DeviceLostReason value) noexcept -> std::string
	{
		switch (value)
		{
		case wgpu::DeviceLostReason::Undefined:
			break;
		case wgpu::DeviceLostReason::Destroyed:
			return "Destroyed";
		case wgpu::DeviceLostReason::InstanceDropped:
			return "InstanceDropped";
		case wgpu::DeviceLostReason::FailedCreation:
			return "FailedCreation";
		}
		return "Undefined";
	}
} // namespace fae
