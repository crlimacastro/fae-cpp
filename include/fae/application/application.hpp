#pragma once

#include <concepts>
#include <unordered_set>

#include "fae/ecs_world.hpp"
#include "fae/event.hpp"
#include "fae/asset_manager.hpp"
#include "fae/scheduler.hpp"
#include "fae/ecs_world.hpp"
#include "fae/entity.hpp"
#include "fae/application/application_step.hpp"

namespace fae
{
    struct application;

    /*
    a plugin promises to mutate the application in some helpful way
    (adding systems, adding resources, creating entities, even adding other plugins)
    and can only be added once
    */
    template <typename plugin_t>
    concept plugin = requires(plugin_t plugin, application& app) {
        {
            plugin.init(app)
        } -> std::same_as<void>;
    };

    /* application quit event. invoke to stop the application */
    struct application_quit
    {
    };

    struct application
    {
        bool is_running{};
        asset_manager assets{};
        scheduler scheduler{};
        ecs_world ecs_world{};
        std::unordered_set<std::type_index> plugins{};
        entity_commands global_entity = ecs_world.create_entity();

        auto step() -> void;
        ;
        auto run() -> void;

        template <typename t_component>
        [[maybe_unused]] inline auto set_global_component(t_component&& value) noexcept -> application&
        {
            global_entity.set_component<t_component>(std::forward<t_component&&>(value));
            return *this;
        }

        template <typename t_arg>
        [[maybe_unused]] inline auto
        add_system(const typename event<t_arg>::t_listener& system) noexcept
            -> application&
        {
            scheduler.add_system<t_arg>(system);
            return *this;
        }

        [[maybe_unused]] inline auto
        add_plugin(const plugin auto& plugin) noexcept -> application&
        {
            const auto key = std::type_index(typeid(plugin));
            if (plugins.contains(key))
            {
                return *this;
            }
            plugins.emplace(key);
            plugin.init(*this);
            return *this;
        }
    };
}
