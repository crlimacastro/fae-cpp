#pragma once

#include <concepts>
#include <typeindex>
#include <unordered_set>
#ifdef FAE_PLATFORM_WEB
#include <emscripten/emscripten.h>
#endif

#include "fae/core.hpp"
#include "fae/event.hpp"
#include "fae/resource_manager.hpp"
#include "fae/scheduler.hpp"
#include "fae/ecs_world.hpp"

namespace fae
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

	struct init_step
	{
		resource_manager &resources;
		scheduler &scheduler;
		ecs_world &ecs_world;
	};
	struct start_step
	{
		resource_manager &resources;
		scheduler &scheduler;
		ecs_world &ecs_world;
	};
	struct pre_update_step
	{
		resource_manager &resources;
		scheduler &scheduler;
		ecs_world &ecs_world;
	};
	struct update_step
	{
		resource_manager &resources;
		scheduler &scheduler;
		ecs_world &ecs_world;
	};
	struct post_update_step
	{
		resource_manager &resources;
		scheduler &scheduler;
		ecs_world &ecs_world;
	};
	struct stop_step
	{
		resource_manager &resources;
		scheduler &scheduler;
		ecs_world &ecs_world;
	};
	struct deinit_step
	{
		resource_manager &resources;
		scheduler &scheduler;
		ecs_world &ecs_world;
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
			scheduler.invoke(pre_update_step{
				.resources = resources,
				.scheduler = scheduler,
				.ecs_world = ecs_world,
			});
			scheduler.invoke(update_step{
				.resources = resources,
				.scheduler = scheduler,
				.ecs_world = ecs_world,
			});
			scheduler.invoke(post_update_step{
				.resources = resources,
				.scheduler = scheduler,
				.ecs_world = ecs_world,
			});
		}

#ifdef FAE_PLATFORM_WEB
		static auto step_callback(void *arg) -> void
		{
			static_cast<application *>(arg)->step();
		}
#endif

		auto run() -> void
		{
			is_running = true;
			scheduler.add_system<application_quit>(
				[&]([[maybe_unused]] const application_quit &event)
				{
					is_running = false;
				});
			scheduler.invoke(init_step{
				.resources = resources,
				.scheduler = scheduler,
				.ecs_world = ecs_world,
			});
			scheduler.invoke(start_step{
				.resources = resources,
				.scheduler = scheduler,
				.ecs_world = ecs_world,
			});
#ifdef FAE_PLATFORM_WEB
			emscripten_set_main_loop_arg(&application::step_callback, this, 0, 1);
			// emscripten set cleanup function like stop & deinit steps below?
#else
			while (is_running)
			{
				step();
			}
			scheduler.invoke(stop_step{
				.resources = resources,
				.scheduler = scheduler,
				.ecs_world = ecs_world,
			});
			scheduler.invoke(deinit_step{
				.resources = resources,
				.scheduler = scheduler,
				.ecs_world = ecs_world,
			});
#endif
		}

		template <typename t_resource, typename... t_args>
		[[maybe_unused]] inline constexpr auto
		emplace_resource(t_args &&...args) noexcept -> application &
		{
			resources.emplace<t_resource>(std::forward<t_args>(args)...);
			return *this;
		}

		template <typename t_resource>
		[[maybe_unused]] inline constexpr auto
		insert_resource(t_resource &&resource = {}) noexcept -> application &
		{
			resources.insert(std::forward<t_resource>(resource));
			return *this;
		}

		template <typename t_resource>
		[[nodiscard]] inline constexpr auto
		insert_or_assign_resource(t_resource &&resource) noexcept -> application &
		{
			resources.insert_or_assign(std::forward<t_resource>(resource));
			return *this;
		}

		template <typename t_arg>
		[[maybe_unused]] inline constexpr auto
		add_system(const typename event<t_arg>::t_listener &system) noexcept
			-> application &
		{
			scheduler.add_system<t_arg>(system);
			return *this;
		}

		[[maybe_unused]] inline constexpr auto
		add_plugin(const plugin auto &plugin) noexcept -> application &
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
} // namespace fae
