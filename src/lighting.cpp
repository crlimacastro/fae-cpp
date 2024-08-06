#include "fae/lighting.hpp"

#include "fae/application.hpp"

namespace fae
{
    auto lighting_plugin::init(application& app) const noexcept -> void
    {
        app
            .insert_resource(ambient_light_info{})
            .insert_resource(directional_light_info{})
            .add_system<update_step>(update_lighting);
    }

    auto update_lighting(const update_step& step) noexcept -> void
    {
        step.resources.use_resource<ambient_light_info>([&](ambient_light_info& info)
            {
                info.clear();
                auto i = 0;
                for (auto& [entity, ambient_light] : step.ecs_world.query<const ambient_light>())
                {
                    info.colors[i] = ambient_light.color.to_vec4();
                    i++;
                } });

        step.resources.use_resource<directional_light_info>([&](directional_light_info& info)
            {
                info.clear();
                auto i = 0;
                for (auto& [entity, directional_light] : step.ecs_world.query<const directional_light>())
                {
                    info.directions[i] = { directional_light.direction, 0.f };
                    info.colors[i] = directional_light.color.to_vec4();
                    i++;
                } });
    }
}
