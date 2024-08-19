#pragma once
#include <type_traits>
#include <concepts>

namespace fae
{
    struct resource_manager;
    struct asset_manager;
    struct scheduler;
    struct ecs_world;

    template <typename step_t>
    concept application_step = requires(step_t step) {
        { step.resources } -> std::convertible_to<resource_manager&>;
        { step.assets } -> std::convertible_to<asset_manager&>;
        { step.scheduler } -> std::convertible_to<scheduler&>;
        { step.ecs_world } -> std::convertible_to<ecs_world&>;
    };

    struct init_step
    {
        resource_manager& resources;
        asset_manager& assets;
        scheduler& scheduler;
        ecs_world& ecs_world;
    };
    struct start_step
    {
        resource_manager& resources;
        asset_manager& assets;
        scheduler& scheduler;
        ecs_world& ecs_world;
    };
    struct pre_update_step
    {
        resource_manager& resources;
        asset_manager& assets;
        scheduler& scheduler;
        ecs_world& ecs_world;
    };
    struct update_step
    {
        resource_manager& resources;
        asset_manager& assets;
        scheduler& scheduler;
        ecs_world& ecs_world;
    };
    struct post_update_step
    {
        resource_manager& resources;
        asset_manager& assets;
        scheduler& scheduler;
        ecs_world& ecs_world;
    };
    struct stop_step
    {
        resource_manager& resources;
        asset_manager& assets;
        scheduler& scheduler;
        ecs_world& ecs_world;
    };
    struct deinit_step
    {
        resource_manager& resources;
        asset_manager& assets;
        scheduler& scheduler;
        ecs_world& ecs_world;
    };
}
