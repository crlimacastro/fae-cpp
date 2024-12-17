#include "fae/imgui.hpp"

#include <imgui.h>
#include <backends/imgui_impl_sdl3.h>
#include <backends/imgui_impl_wgpu.h>

#include "fae/application/application.hpp"
#include "fae/windowing.hpp"
#include "fae/webgpu/webgpu.hpp"
#include "fae/ui.hpp"

namespace fae
{
    auto imgui_plugin::init(application& app) const noexcept -> void
    {
        app
            .add_system<init_step>(init_imgui)
            .add_system<ui_begin_step>(begin_imgui)
            .add_system<ui_end_step>(end_imgui)
            .add_system<deinit_step>(deinit_imgui);
    }

    auto init_imgui(const init_step& step) noexcept -> void
    {
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGui::StyleColorsDark();
        auto& io = ImGui::GetIO();
        io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;

        auto maybe_primary_window = step.global_entity.get_component<primary_window>();
        if (!maybe_primary_window)
        {
            fae::log_fatal("no primary window found while initializing imgui plugin");
        }
        auto& primary_window = *maybe_primary_window;
        auto maybe_sdl_window = entity_commands{ .id = primary_window.window_entity, .registry = step.ecs_world.registry }.get_component<SDL_Window*>();
        if (!maybe_sdl_window)
        {
            fae::log_fatal("primary window does not have sdl window component");
        }
        auto& sdl_window = *maybe_sdl_window;

        ImGui_ImplSDL3_InitForOther(sdl_window);

        auto maybe_webgpu = step.global_entity.get_component<fae::webgpu>();
        if (!maybe_webgpu)
        {
            fae::log_fatal("no webgpu resource found while initializing imgui plugin");
        }
        auto& webgpu = *maybe_webgpu;

        auto wgpu_init_info = ImGui_ImplWGPU_InitInfo{};
        wgpu_init_info.Device = webgpu.device.Get();
        auto capabilities = wgpu::SurfaceCapabilities{};
        webgpu.surface.GetCapabilities(webgpu.adapter, &capabilities);
        wgpu_init_info.RenderTargetFormat = static_cast<WGPUTextureFormat>(capabilities.formats[0]);
        wgpu_init_info.DepthStencilFormat = static_cast<WGPUTextureFormat>(webgpu.depth_texture_format);
        ImGui_ImplWGPU_Init(&wgpu_init_info);
    }

    auto begin_imgui(const ui_begin_step& step) noexcept -> void
    {
        ImGui_ImplWGPU_NewFrame();
        ImGui_ImplSDL3_NewFrame();
        ImGui::NewFrame();
    }

    auto end_imgui(const ui_end_step& step) noexcept -> void
    {
        ImGui::EndFrame();
        ImGui::Render();
        step.global_entity.use_component<fae::webgpu>([&](fae::webgpu& webgpu)
            {
                for (auto &render_pass : webgpu.render_passes)
                {
                    if (render_pass.label == "fae_ui_render_pass")
                    {
                        ImGui_ImplWGPU_RenderDrawData(ImGui::GetDrawData(), render_pass.render_pass_encoder.Get());
                    }
                } });
    }

    auto deinit_imgui(const deinit_step& step) noexcept -> void
    {
        ImGui_ImplSDL3_Shutdown();
        ImGui_ImplWGPU_Shutdown();
    }
}
