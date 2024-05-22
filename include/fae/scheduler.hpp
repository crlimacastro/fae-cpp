#pragma once

#include <any>
#include <typeindex>
#include <unordered_map>

#include "fae/event.hpp"

namespace fae
{
	struct scheduler
	{
		template <typename t_arg>
		[[maybe_unused]] inline constexpr auto add_system(const typename event<t_arg>::t_listener &system) noexcept -> scheduler &
		{
			const auto key = std::type_index(typeid(t_arg));
			if (!m_systems.contains(key))
			{
				m_systems[key] = event<t_arg>{};
			}
			auto &e = std::any_cast<event<t_arg> &>(m_systems[key]);
			e += system;
			return *this;
		}

		template <typename t_arg>
		[[maybe_unused]] inline constexpr auto remove_system(const typename event<t_arg>::t_listener &system) noexcept -> scheduler &
		{
			const auto key = std::type_index(typeid(t_arg));
			if (!m_systems.contains(key))
			{
				return *this;
			}
			auto &e = std::any_cast<event<t_arg> &>(m_systems[key]);
			e -= system;
			return *this;
		}

		template <typename t_arg>
		[[maybe_unused]] inline constexpr auto clear_systems() noexcept -> scheduler &
		{
			const auto key = std::type_index(typeid(t_arg));
			if (!m_systems.contains(key))
			{
				return *this;
			}
			auto &e = std::any_cast<event<t_arg> &>(m_systems[key]);
			e.clear();
			return *this;
		}

		[[maybe_unused]] inline auto clear_systems() noexcept -> scheduler &
		{
			m_systems.clear();
			return *this;
		}

		template <typename t_arg>
		[[maybe_unused]] inline constexpr auto invoke(const t_arg &arg = {}) -> scheduler &
		{
			const auto key = std::type_index(typeid(t_arg));
			if (!m_systems.contains(key))
			{
				return *this;
			}
			auto &e = std::any_cast<event<t_arg> &>(m_systems[key]);
			e.invoke(std::forward<const t_arg &>(arg));
			return *this;
		}

		template <typename t_arg>
		[[maybe_unused]] inline constexpr auto invoke(const t_arg &arg = {}) const -> const scheduler &
		{
			const auto key = std::type_index(typeid(t_arg));
			if (!m_systems.contains(key))
			{
				return *this;
			}
			const auto &e = std::any_cast<event<t_arg> &>(m_systems.at(key));
			e.invoke(std::forward<const t_arg &>(arg));
			return *this;
		}

		template <typename t_arg>
		[[maybe_unused]] inline constexpr auto operator()(const t_arg &arg = {}) -> scheduler &
		{
			return invoke(std::forward<const t_arg &>(arg));
		}

		template <typename t_arg>
		[[maybe_unused]] inline constexpr auto operator()(const t_arg &arg = {}) const -> const scheduler &
		{
			return invoke(std::forward<const t_arg &>(arg));
		}

	private:
		std::unordered_map<std::type_index, std::any> m_systems{};
	};
} // namespace fae
