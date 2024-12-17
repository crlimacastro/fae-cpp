#pragma once

#include <concepts>

namespace fae
{
    struct entity_commands;
    struct asset_manager;
    struct scheduler;
    struct ecs_world;

    template <typename step_t>
    concept application_step = requires(step_t step) {
        { step.global_entity } -> std::convertible_to<entity_commands&>;
        { step.assets } -> std::convertible_to<asset_manager&>;
        { step.scheduler } -> std::convertible_to<scheduler&>;
        { step.ecs_world } -> std::convertible_to<ecs_world&>;
    };

    struct init_step
    {
        entity_commands& global_entity;
        asset_manager& assets;
        scheduler& scheduler;
        ecs_world& ecs_world;
    };
    struct start_step
    {
        entity_commands& global_entity;
        asset_manager& assets;
        scheduler& scheduler;
        ecs_world& ecs_world;
    };
    struct pre_update_step
    {
        entity_commands& global_entity;
        asset_manager& assets;
        scheduler& scheduler;
        ecs_world& ecs_world;
    };
    struct update_step
    {
        entity_commands& global_entity;
        asset_manager& assets;
        scheduler& scheduler;
        ecs_world& ecs_world;
    };
    struct post_update_step
    {
        entity_commands& global_entity;
        asset_manager& assets;
        scheduler& scheduler;
        ecs_world& ecs_world;
    };
    struct stop_step
    {
        entity_commands& global_entity;
        asset_manager& assets;
        scheduler& scheduler;
        ecs_world& ecs_world;
    };
    struct deinit_step
    {
        entity_commands& global_entity;
        asset_manager& assets;
        scheduler& scheduler;
        ecs_world& ecs_world;
    };
}
