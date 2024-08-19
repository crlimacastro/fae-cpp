#include "fae/webgpu/webgpu.hpp"

#include <optional>
#include <format>
#include <string>
#include <string_view>
#include <filesystem>
#include <cstddef>

#include "fae/application/application.hpp"
#include "fae/sdl.hpp"
#include "fae/windowing.hpp"
#include "fae/rendering/mesh.hpp"
#include "fae/lighting.hpp"
#include "fae/webgpu/default_render_pipeline.hpp"

namespace fae
{
    auto webgpu_plugin::init(application& app) const noexcept -> void
    {
        app
            .add_plugin(windowing_plugin{})
            .add_system<window_resized>(reconfigure_on_window_resized);

        auto& webgpu = app.resources.emplace_and_get<fae::webgpu>(fae::webgpu{
            .instance = wgpu::CreateInstance(),
        });
        webgpu.adapter = request_adapter_sync(webgpu.instance, adapter_options);
        webgpu.device = request_device_sync(webgpu.adapter, device_descriptor);
#ifndef FAE_PLATFORM_WEB
        webgpu.device.SetLoggingCallback(logging_callback, nullptr);
#endif
        webgpu.device.SetUncapturedErrorCallback(error_callback, nullptr);
        auto maybe_primary_window = app.resources.get<primary_window>();
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
        auto& sdl_window = *maybe_sdl_window;
        webgpu.surface = get_sdl_webgpu_surface(webgpu.instance, sdl_window.raw.get());
        auto& window = primary_window.window();
        auto window_size = window.get_size();

        auto surface_format = webgpu.surface.GetPreferredFormat(webgpu.adapter);

        auto surface_config = wgpu::SurfaceConfiguration{
            .device = webgpu.device,
            .format = surface_format,
            .usage = wgpu::TextureUsage::RenderAttachment,
            .width = static_cast<std::uint32_t>(window_size.width),
            .height = static_cast<std::uint32_t>(window_size.height),
            .presentMode = wgpu::PresentMode::Fifo,
        };
        webgpu.surface.Configure(&surface_config);
    }

    auto reconfigure_on_window_resized(const fae::window_resized& e) noexcept -> void
    {
        e.resources.use_resource<fae::webgpu>([&](webgpu& webgpu)
            {
            auto window_width = e.width;
            auto window_height = e.height;

            if (window_width == 0 || window_height == 0)
            {
                return;
            }

            webgpu.surface.Unconfigure();

            auto surface_config = wgpu::SurfaceConfiguration{
                .device = webgpu.device,
                .format = webgpu.surface.GetPreferredFormat(webgpu.adapter),
                .usage = wgpu::TextureUsage::RenderAttachment,
                .width = static_cast<std::uint32_t>(window_width),
                .height = static_cast<std::uint32_t>(window_height),
                .presentMode = wgpu::PresentMode::Fifo,
            };
            webgpu.surface.Configure(&surface_config); });
    }
}
