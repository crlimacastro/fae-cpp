#pragma once

#include <webgpu/webgpu_cpp.h>

#include "fae/logging.hpp"
#include "fae/math.hpp"
#include "fae/windowing.hpp"

#include "fae/webgpu/sdl_impl.hpp"
#include "fae/webgpu/string_utils.hpp"
#include "fae/webgpu/utils.hpp"

#include "fae/rendering/mesh.hpp"

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
    static_assert(sizeof(t_uniforms) % 16 == 0, "uniform buffer must be aligned on 16 bytes");

    struct shader_module
    {
        std::string shader_src;
        std::string label = "";

        static auto load(std::filesystem::path path) -> std::optional<shader_module>
        {
            auto shader_module = fae::shader_module{};
            auto file = std::ifstream(path);
            if (!file.is_open())
            {
                return std::nullopt;
            }
            file.seekg(0, std::ios::end);
            std::size_t size = file.tellg();
            shader_module.shader_src = std::string(size, ' ');
            file.seekg(0);
            file.read(shader_module.shader_src.data(), size);
            return shader_module;
        }

        auto create(wgpu::Device device) -> wgpu::ShaderModule
        {
            return create_shader_module_from_str(device, "shader_module", shader_src);
        }
    };

    struct webgpu_render_pipeline
    {
        wgpu::ShaderModule shader_module;
        wgpu::RenderPipeline pipeline;
        wgpu::Texture depth_texture;
        std::uint32_t uniform_stride;
    };

    auto create_default_render_pipeline(application& app) noexcept -> webgpu_render_pipeline;

    struct webgpu
    {
        wgpu::Instance instance;
        wgpu::Adapter adapter;
        wgpu::Device device;
        wgpu::Surface surface;
        std::function<webgpu_render_pipeline(application&)> create_render_pipeline = create_default_render_pipeline;
        webgpu_render_pipeline render_pipeline;

        wgpu::Color clear_color = { 0, 0, 0, 1 };
        struct current_render
        {
            wgpu::CommandEncoder command_encoder;
            wgpu::RenderPassEncoder render_pass;

            struct render_command
            {
                std::vector<vertex> vertex_data;
                std::vector<std::uint32_t> index_data;
                t_uniforms uniform_data;
            };
            std::vector<render_command> render_commands;
        };
        current_render current_render{};
    };

    auto reconfigure_on_window_resized(const fae::window_resized& e) noexcept -> void;

    struct webgpu_plugin
    {
        wgpu::InstanceDescriptor instance_descriptor{};
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
