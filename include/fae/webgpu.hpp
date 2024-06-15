#pragma once

#include <webgpu/webgpu_cpp.h>

#include "fae/logging.hpp"
#include "fae/math.hpp"

#include "fae/webgpu/sdl_impl.hpp"
#include "fae/webgpu/string_utils.hpp"
#include "fae/webgpu/utils.hpp"

namespace fae
{
    struct application;

    struct t_uniforms
    {
        mat4 model = mat4(1.f);
        mat4 view = mat4(1.f);
        mat4 projection = mat4(1.f);
        vec4 tint = { 1.f, 1.f, 1.f, 1.f };
        float time = 0;
        float padding0;
        float padding1;
        float padding2;
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

        wgpu::Color clear_color = { 0, 0, 0, 1 };
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
        wgpu::RequestAdapterOptions adapter_options{};
        wgpu::DeviceDescriptor device_descriptor{
#ifndef FAE_PLATFORM_WEB
            .deviceLostCallbackInfo = wgpu::DeviceLostCallbackInfo{
                .mode = wgpu::CallbackMode::AllowSpontaneous,
                .callback = [](WGPUDevice const* device, WGPUDeviceLostReason cReason, char const* message, void* userdata)
                {
                    auto reason = static_cast<wgpu::DeviceLostReason>(cReason);
                    switch (reason)
                    {
                    case wgpu::DeviceLostReason::Destroyed:
                        // don't log this reason to not scare the user, happens naturally at the end of the application lifecycle
                        break;
                    case wgpu::DeviceLostReason::Unknown:
                    case wgpu::DeviceLostReason::InstanceDropped:
                    case wgpu::DeviceLostReason::FailedCreation:
                        fae::log_info(std::format("[wgpu] Device lost. [reason] {} [message] {}", to_string(reason), message));
                        break;
                    }
                },
            }
#endif
        };
#ifndef FAE_PLATFORM_WEB
        wgpu::LoggingCallback logging_callback = [](WGPULoggingType cType, char const* message, void* userdata)
        {
            auto type = static_cast<wgpu::LoggingType>(cType);
            fae::log_info(std::format("[wgpu] [type] {} [message] {}", to_string(type), message));
        };
#endif
        wgpu::ErrorCallback error_callback = [](WGPUErrorType cType, const char* message, void* userdata)
        {
            auto type = static_cast<wgpu::ErrorType>(cType);
            fae::log_error(std::format("[wgpu] [type] {} [message] {}", to_string(type), message));
        };

        auto init(application& app) const noexcept -> void;
    };
}
