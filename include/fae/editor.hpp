#pragma once

#include "fae/entity.hpp"

namespace fae
{
    struct application;
    struct asset_manager;
    struct scheduler;
    struct ecs_world;

    struct ui_render_step;

    struct editor
    {
        entity selected_entity;
    };

    struct editor_render_step
    {
        entity_commands& global_entity;
        asset_manager& assets;
        scheduler& scheduler;
        ecs_world& ecs_world;
    };

    struct editor_plugin
    {
        auto init(application& app) const noexcept -> void;
    };

    auto ui_render_editor(const ui_render_step& step) noexcept -> void;
    auto ui_render_entity_scene(const editor_render_step& step) noexcept -> void;
    auto ui_render_entity_inspector(const editor_render_step& step) noexcept -> void;
    auto ui_render_resources(const editor_render_step& step) noexcept -> void;
}
