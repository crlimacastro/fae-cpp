#pragma once

#include <concepts>
#include <unordered_set>

#include "fae/core.hpp"
#include "fae/ecs_world.hpp"
#include "fae/event.hpp"
#include "fae/resource_manager.hpp"
#include "fae/asset_manager.hpp"
#include "fae/scheduler.hpp"

namespace fae
{
    struct application;

    /*
    a plugin promises to mutate the application in some helpful way
    (adding systems, adding resources, creating entities, even adding other plugins)
    and can only be added once
    */
    template <typename tplugin>
    concept plugin = requires(tplugin plugin, application& app) {
        {
            plugin.init(app)
        } -> std::same_as<void>;
    };

    /* application quit event. invoke to stop the application */
    struct application_quit
    {
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

    struct application
    {
        bool is_running{};
        resource_manager resources{};
        asset_manager assets{};
        scheduler scheduler{};
        ecs_world ecs_world{};
        std::unordered_set<std::type_index> plugins{};

        auto step() -> void;
        ;
        auto run() -> void;

        template <typename t_resource, typename... t_args>
        [[maybe_unused]] inline auto
        emplace_resource(t_args&&... args) noexcept -> application&
        {
            resources.emplace<t_resource>(std::forward<t_args>(args)...);
            return *this;
        }

        template <typename t_resource>
        [[maybe_unused]] inline auto
        insert_resource(t_resource&& resource = {}) noexcept -> application&
        {
            resources.insert(std::forward<t_resource>(resource));
            return *this;
        }

        template <typename t_resource>
        [[nodiscard]] inline auto
        insert_or_assign_resource(t_resource&& resource) noexcept -> application&
        {
            resources.insert_or_assign(std::forward<t_resource>(resource));
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
