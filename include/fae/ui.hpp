#pragma once

#include <imgui.h>
#include <misc/cpp/imgui_stdlib.h>

namespace fae
{
    namespace ui
    {
        using namespace ImGui;
    }

struct entity_commands;
    struct application;
    struct asset_manager;
    struct scheduler;
    struct ecs_world;

    struct render_step;

    struct ui_settings
    {
        bool hide_ui = false;
    };

    struct ui_begin_step
    {
        entity_commands& global_entity;
        asset_manager& assets;
        scheduler& scheduler;
        ecs_world& ecs_world;
    };

    struct ui_render_step
    {
        entity_commands& global_entity;
        asset_manager& assets;
        scheduler& scheduler;
        ecs_world& ecs_world;
    };

    struct ui_end_step
    {
        entity_commands& global_entity;
        asset_manager& assets;
        scheduler& scheduler;
        ecs_world& ecs_world;
    };

    struct ui_plugin
    {
        auto init(application& app) const noexcept -> void;
    };

    auto render_ui(const render_step& step) noexcept -> void;
}
