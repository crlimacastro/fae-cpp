#pragma once

#include <any>
#include <vector>

#include <entt/entt.hpp>

#include "fae/core.hpp"

namespace fae
{
	using entity_id_t = entt::entity;
	using untity_registry_t = entt::registry;

	struct entity
	{
		entity_id_t id;
		untity_registry_t &registry;

		template <typename t_component>
		[[nodiscard]] inline constexpr auto get_component() const noexcept -> optional_reference<const t_component>
		{

			auto maybe_component = registry.try_get<const t_component>(id);
			if (!maybe_component)
			{
				return std::nullopt;
			}
			return optional_reference<const t_component>(*maybe_component);
		}

		template <typename t_component>
		[[nodiscard]] inline constexpr auto get_component() noexcept -> optional_reference<t_component>
		{

			auto maybe_component = registry.try_get<t_component>(id);
			if (!maybe_component)
			{
				return std::nullopt;
			}
			return optional_reference<t_component>(*maybe_component);
		}

		template <typename t_component>
		[[nodiscard]] inline constexpr auto set_and_get_component(t_component &&value) noexcept -> t_component &
		{
			registry.emplace_or_replace<t_component>(id, std::forward<t_component &&>(value));
			return registry.get<t_component>(id);
		}

		template <typename t_component>
		[[maybe_unused]] inline constexpr auto set_component(t_component &&value) noexcept -> entity &
		{
			[[maybe_unused]] auto maybe_component = set_and_get_component<t_component>(std::forward<t_component &&>(value));
			return *this;
		}

		template <typename t_component>
		[[maybe_unused]] inline constexpr auto use_component(std::function<void(t_component &)> callback) noexcept -> entity &
		{
			auto maybe_component = get_component<t_component>();
			if (maybe_component)
			{
				callback(*maybe_component);
			}
			return *this;
		}

		template <typename t_component>
		[[maybe_unused]] inline constexpr auto use_component(std::function<void(const t_component &)> callback) const noexcept -> const entity &
		{
			const auto maybe_component = get_component<const t_component>();
			if (maybe_component)
			{
				callback(*maybe_component);
			}
			return *this;
		}

		inline auto destroy() noexcept -> void
		{
			registry.destroy(id);
		}
	};
}
