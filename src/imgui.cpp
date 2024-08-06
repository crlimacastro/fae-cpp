#include "fae/imgui.hpp"

#include <imgui.h>
#include <backends/imgui_impl_sdl3.h>
#include <backends/imgui_impl_wgpu.h>

#include "fae/application.hpp"
#include "fae/windowing.hpp"
#include "fae/webgpu.hpp"
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
        auto& io = ImGui::GetIO();

        auto maybe_primary_window = step.resources.get<primary_window>();
        if (!maybe_primary_window)
        {
            fae::log_fatal("no primary window found while initializing imgui plugin");
        }
        auto& primary_window = *maybe_primary_window;
        auto maybe_sdl_window = primary_window.window_entity.get_component<fae::sdl_window>();
        if (!maybe_sdl_window)
        {
            fae::log_fatal("primary window does not have sdl window component");
        }
        auto& sdl_window = *maybe_sdl_window;

        ImGui_ImplSDL3_InitForOther(sdl_window.raw.get());

        auto maybe_webgpu = step.resources.get<fae::webgpu>();
        if (!maybe_webgpu)
        {
            fae::log_fatal("no webgpu resource found while initializing imgui plugin");
        }
        auto& webgpu = *maybe_webgpu;

        auto wgpu_init_info = ImGui_ImplWGPU_InitInfo{};
        wgpu_init_info.Device = webgpu.device.Get();
        wgpu_init_info.RenderTargetFormat = static_cast<WGPUTextureFormat>(webgpu.surface.GetPreferredFormat(webgpu.adapter));
        wgpu_init_info.DepthStencilFormat = static_cast<WGPUTextureFormat>(webgpu.render_pipeline.depth_texture.GetFormat());
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
        step.resources.use_resource<fae::webgpu>([&](fae::webgpu& webgpu)
            { ImGui_ImplWGPU_RenderDrawData(ImGui::GetDrawData(), webgpu.current_render.ui_render_pass.Get()); });
    }

    auto deinit_imgui(const deinit_step& step) noexcept -> void
    {
        ImGui_ImplSDL3_Shutdown();
        ImGui_ImplWGPU_Shutdown();
    }
}
