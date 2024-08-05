#pragma once

namespace fae
{
    struct application;
    struct resource_manager;
    struct asset_manager;
    struct scheduler;
    struct ecs_world;

    struct render_step;

    struct ui_begin_step
    {
        resource_manager& resources;
        asset_manager& assets;
        scheduler& scheduler;
        ecs_world& ecs_world;
    };

    struct ui_render_step
    {
        resource_manager& resources;
        asset_manager& assets;
        scheduler& scheduler;
        ecs_world& ecs_world;
    };

    struct ui_end_step
    {
        resource_manager& resources;
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
