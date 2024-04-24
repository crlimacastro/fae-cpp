module;
#include <concepts>
#include <unordered_set>
#include <typeindex>
#if defined(__EMSCRIPTEN__)
#include <emscripten/emscripten.h>
#endif

export module fae:application;

import :core;
import :event;
import :resource_manager;
import :scheduler;
import :ecs_world;

export namespace fae
{
    struct application;

    /*
    a plugin promises to mutate the application in some helpful way
    (adding systems, adding resources, creating entities, even adding other plugins)
    and can only be added once
    */
    template <typename tplugin>
    concept plugin = requires(tplugin plugin, application &app) {
        {
            plugin.init(app)
        } -> std::same_as<void>;
    };

    /* application quit event. invoke to stop the application */
    struct application_quit
    {
    };

    struct application_commands
    {
        resource_manager &resources;
        scheduler &scheduler;
        ecs_world &ecs_world;
    };

    struct init_step
    {
        application_commands commands;
    };
    struct start_step
    {
        application_commands commands;
    };
    struct pre_update_step
    {
        application_commands commands;
    };
    struct update_step
    {
        application_commands commands;
    };
    struct post_update_step
    {
        application_commands commands;
    };
    struct stop_step
    {
        application_commands commands;
    };
    struct deinit_step
    {
        application_commands commands;
    };

    struct application
    {
        bool is_running{};
        resource_manager resources{};
        scheduler scheduler{};
        ecs_world ecs_world{};
        std::unordered_set<std::type_index> plugins{};

        auto step() -> void
        {
            const auto commands = application_commands{
                .resources = resources,
                .scheduler = scheduler,
                .ecs_world = ecs_world,
            };
            scheduler.invoke(pre_update_step{
                .commands = commands,
            });
            scheduler.invoke(update_step{
                .commands = commands,
            });
            scheduler.invoke(post_update_step{
                .commands = commands,
            });
        }

        auto run() -> void
        {
            is_running = true;
            scheduler.add_system<application_quit>([&]([[maybe_unused]] const application_quit &event)
                                                   { is_running = false; });
            const auto commands = application_commands{
                .resources = resources,
                .scheduler = scheduler,
                .ecs_world = ecs_world,
            };
            scheduler.invoke(init_step{
                .commands = commands,
            });
            scheduler.invoke(start_step{
                .commands = commands,
            });
#if defined(__EMSCRIPTEN__)
            emscripten_set_main_loop(step, 0, false);
            // emscripten set cleanup function like stop & deinit steps below?
#else
            while (is_running)
            {
                step();
            }
            scheduler.invoke(stop_step{
                .commands = commands,
            });
            scheduler.invoke(deinit_step{
                .commands = commands,
            });
#endif
        }

        template <typename t_resource>
        [[maybe_unused]] inline constexpr auto insert_resource(t_resource &&resource = {}) noexcept -> application &
        {
            resources.insert(std::forward<t_resource>(resource));
            return *this;
        }

        template <typename t_resource, typename... t_args>
        [[maybe_unused]] inline constexpr auto emplace_resource(t_args &&...args) noexcept -> application &
        {
            resources.emplace<t_resource>(std::forward<t_args>(args)...);
            return *this;
        }

        template <typename t_arg>
        [[maybe_unused]] inline constexpr auto add_system(const typename event<t_arg>::t_listener &system) noexcept -> application &
        {
            scheduler.add_system<t_arg>(system);
            return *this;
        }

        [[maybe_unused]] inline constexpr auto add_plugin(const plugin auto &plugin) noexcept -> application &
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
