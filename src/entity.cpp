module;
#include <vector>
#include <any>
#include <entt/entt.hpp>

export module fae:entity;

import :core;

export namespace fae
{
    struct entity
    {
        entt::entity id;
        entt::registry &registry;

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
    };
}