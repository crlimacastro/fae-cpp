#pragma once

#include <vector>
#include <string>
#include <functional>

#include <entt/entt.hpp>

#include "fae/core/optional_reference.hpp"

namespace fae
{
    using entity = entt::entity;
    using entity_registry_t = entt::registry;

    struct name
    {
        std::string value;
    };

    struct parent
    {
        entity value;
    };

    struct children
    {
        std::vector<entity> value;
    };

    struct entity_commands
    {
        entity id;
        entity_registry_t& registry;

        template <typename... t_component>
        [[nodiscard]] inline constexpr auto has_components() const noexcept -> bool
        {
            return registry.all_of<t_component...>(id);
        }

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
        [[nodiscard]] inline constexpr auto get_or_set_component(t_component&& value) noexcept -> t_component&
        {
            if (!has_components<t_component>())
            {
                return set_and_get_component<t_component>(std::forward<t_component&&>(value));
            }

			auto component = get_component<t_component>();
            return *component;
        }

        template <typename t_component>
        [[nodiscard]] inline constexpr auto set_and_get_component(t_component&& value) noexcept -> t_component&
        {
            registry.emplace_or_replace<t_component>(id, std::forward<t_component&&>(value));
            return registry.get<t_component>(id);
        }

        template <typename t_component>
        [[maybe_unused]] inline constexpr auto set_component(t_component&& value) noexcept -> entity_commands&
        {
            [[maybe_unused]] auto maybe_component = set_and_get_component<t_component>(std::forward<t_component&&>(value));
            return *this;
        }

        template <typename t_component>
        [[maybe_unused]] inline constexpr auto use_component(std::function<void(t_component&)> callback) noexcept -> entity_commands&
        {
            auto maybe_component = get_component<t_component>();
            if (maybe_component)
            {
                callback(*maybe_component);
            }
            return *this;
        }

        template <typename t_component>
        [[maybe_unused]] inline constexpr auto use_component(std::function<void(const t_component&)> callback) const noexcept -> const entity_commands&
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

        [[nodiscard]] inline auto valid() noexcept -> bool
        {
            return registry.valid(id);
        }
    };
}
