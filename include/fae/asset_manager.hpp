#pragma once

#include <any>
#include <functional>
#include <optional>
#include <typeindex>
#include <unordered_map>
#include <concepts>
#include <filesystem>
#include <fstream>

#include "fae/core.hpp"

namespace fae
{
    template <typename t_asset>
    concept asset = requires(const std::filesystem::path& path) {
        { t_asset::load(path) } -> std::same_as<std::optional<t_asset>>;
    };

    struct asset_manager
    {
        template <asset t_asset>
        [[nodiscard]] auto load(const std::filesystem::path& path) noexcept
            -> optional_reference<t_asset>
        {
            auto resolved_path = resolve_path(path);
            if (m_assets.find(resolved_path) != m_assets.end())
            {
                return optional_reference<t_asset>(std::any_cast<t_asset&>(m_assets.at(resolved_path)));
            }

            auto maybe_asset = t_asset::load(resolved_path);
            if (maybe_asset)
            {
                auto asset = *maybe_asset;
                m_assets.insert_or_assign(resolved_path, std::any(std::forward<t_asset&&>(asset)));
                return optional_reference<t_asset>(std::any_cast<t_asset&>(m_assets.at(resolved_path)));
            }

            return std::nullopt;
        }

        [[nodiscard]] auto resolve_path(const std::filesystem::path& path) const noexcept
            -> std::filesystem::path
        {
            return FAE_ASSET_DIR / path;
        }

      private:
        std::unordered_map<std::filesystem::path, std::any> m_assets{};
    };
}
