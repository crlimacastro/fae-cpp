#pragma once

#include "fae/entity.hpp"

namespace fae
{
    struct application;
    struct resource_manager;
    struct asset_manager;
    struct scheduler;
    struct ecs_world;

    struct ui_render_step;

    struct editor
    {
        entity_id_t selected_entity;
    };

    struct editor_render_step
    {
        resource_manager& resources;
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
