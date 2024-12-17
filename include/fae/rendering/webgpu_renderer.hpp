#pragma once

namespace fae
{
    struct ecs_world;
    struct entity_commands;
    struct renderer;

    [[nodiscard]] auto make_webgpu_renderer(ecs_world& ecs_world, entity_commands& global_entity) noexcept -> renderer;
}
