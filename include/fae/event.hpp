#pragma once

#include <algorithm>
#include <functional>
#include <vector>

namespace fae
{
    template <typename... t_args>
    struct event
    {
        using t_listener = std::function<void(const t_args&...)>;
        /* t_listener in function pointer form */
        using t_listener_fptr = void (*)(const t_args&...);

        [[maybe_unused]] inline constexpr auto add_listener(const t_listener& listener) noexcept -> event&
        {
            m_listeners.push_back(listener);
            return *this;
        }

        [[maybe_unused]] inline constexpr auto operator+=(const t_listener& listener) noexcept -> event&
        {
            return add_listener(listener);
        }

        [[maybe_unused]] inline constexpr auto remove_listener(const t_listener& listener) noexcept -> event&
        {
            m_listeners.erase(
                std::remove_if(
                    m_listeners.begin(),
                    m_listeners.end(),
                    [&](const t_listener& l)
                    {
                        const auto l_fptr = l.template target<t_listener_fptr>();
                        const auto listener_fptr = listener.template target<t_listener_fptr>();
                        const auto are_same_fptr = l_fptr && listener_fptr && *l_fptr == *listener_fptr;
                        return are_same_fptr;
                    }),
                m_listeners.end());
            return *this;
        }

        [[maybe_unused]] inline constexpr auto operator-=(const t_listener& listener) noexcept -> event&
        {
            return remove_listener(listener);
        }

        [[maybe_unused]] inline constexpr auto clear() noexcept -> event&
        {
            m_listeners.clear();
            return *this;
        }

        [[maybe_unused]] inline constexpr auto invoke(const t_args&... args) -> event&
        {
            for (const auto& listener : m_listeners)
            {
                listener(std::forward<const t_args&>(args)...);
            }
            return *this;
        }

        [[maybe_unused]] inline constexpr auto invoke(const t_args&... args) const -> const event&
        {
            for (const auto& listener : m_listeners)
            {
                listener(std::forward<const t_args&>(args)...);
            }
            return *this;
        }

        [[maybe_unused]] inline constexpr auto operator()(const t_args&... args) -> event&
        {
            return invoke(std::forward<const t_args&>(args)...);
        }

        [[maybe_unused]] inline constexpr auto operator()(const t_args&... args) const -> const event&
        {
            return invoke(std::forward<const t_args&>(args)...);
        }

        /* reverse invoke */
        [[maybe_unused]] inline constexpr auto rinvoke(const t_args&... args) -> event&
        {
            for (auto it = m_listeners.rbegin(); it != m_listeners.rend(); ++it)
            {
                (*it)(std::forward<const t_args&>(args)...);
            }
            return *this;
        }

        /* const reverse invoke */
        [[maybe_unused]] inline constexpr auto rinvoke(const t_args&... args) const -> const event&
        {
            for (auto it = m_listeners.rbegin(); it != m_listeners.rend(); ++it)
            {
                (*it)(std::forward<const t_args&>(args)...);
            }
            return *this;
        }

      private:
        std::vector<t_listener> m_listeners{};
    };
} // namespace fae
