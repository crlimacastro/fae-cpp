#pragma once

#include <algorithm>
#include <ranges>
#include <span>
#include <tuple>
#include <vector>
#include <utility>
#include <stack>

#include <entt/entt.hpp>

#include "fae/entity.hpp"

namespace fae
{
    struct ecs_world
    {
        entt::registry registry{};

        [[nodiscard]] inline constexpr auto create_entity() noexcept -> fae::entity
        {
            return fae::entity{
                .id = registry.create(),
                .registry = registry,
            };
        }

        template <typename... t_args>
        [[nodiscard]] inline constexpr auto query() noexcept -> std::vector<std::tuple<fae::entity, t_args&...>>
        {
            std::vector<std::tuple<fae::entity, t_args&...>> query_results;
            registry.view<t_args...>().each([&](auto entity, auto&... args)
                { query_results.emplace_back(fae::entity{ .id = entity, .registry = registry }, args...); });
            return query_results;
        }

        [[nodiscard]] auto entities() noexcept -> std::vector<fae::entity>
        {
            std::vector<fae::entity> entities;

            std::stack<fae::entity> entity_stack;
            for (auto entity : registry.view<entt::entity>())
            {
                entity_stack.push(fae::entity{ .id = entity, .registry = registry });
            }
            while (!entity_stack.empty()) {
                entities.push_back(entity_stack.top());
                entity_stack.pop();
            }
            return entities;
        }
    };
}
