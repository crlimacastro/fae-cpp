#pragma once
#include <imgui.h>

namespace fae
{
    struct application;
    struct init_step;
    struct deinit_step;
    struct ui_begin_step;
    struct ui_end_step;

    struct imgui_plugin
    {
        auto init(application& app) const noexcept -> void;
    };

    auto init_imgui(const init_step& step) noexcept -> void;
    auto begin_imgui(const ui_begin_step& step) noexcept -> void;
    auto end_imgui(const ui_end_step& step) noexcept -> void;
    auto deinit_imgui(const deinit_step& step) noexcept -> void;
}
