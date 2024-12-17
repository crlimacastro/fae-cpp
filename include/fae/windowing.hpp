#pragma once

#include <cstddef>
#include <functional>
#include <string>
#include <string_view>

#include "fae/entity.hpp"

namespace fae
{
    struct application;
    struct asset_manager;
    struct scheduler;
    struct ecs_world;
    struct update_step;

    struct window
    {

        std::function<std::string_view()> get_title;
        std::function<void(std::string_view)> set_title;
        struct size
        {
            std::size_t width;
            std::size_t height;
        };
        std::function<size()> get_size;
        std::function<void(std::size_t width, std::size_t height)> set_size;
        struct position
        {
            int x;
            int y;
        };
        std::function<position()> get_position;
        std::function<void(int x, int y)> set_position;
        std::function<void()> show;
        std::function<void()> hide;
        std::function<void()> update;
        std::function<bool()> should_close;
        std::function<void()> close;
        std::function<bool()> is_fullscreen;
        std::function<void(bool)> set_fullscreen;
        std::function<bool()> is_focused;

        auto toggle_fullscreen() noexcept -> void
        {
            set_fullscreen(!is_fullscreen());
        }
    };

    struct primary_window
    {
        fae::entity window_entity;
    };

    struct first_render_end
    {
        entity_commands& global_entity;
        asset_manager& assets;
        scheduler& scheduler;
        ecs_world& ecs_world;
    };

    struct window_resized
    {
        std::size_t width;
        std::size_t height;

        entity_commands& global_entity;
        asset_manager& assets;
        scheduler& scheduler;
        ecs_world& ecs_world;
    };

    auto show_primary_window_after_first_render(const fae::first_render_end& e) noexcept -> void;
    auto update_windows(const update_step& step) noexcept -> void;

    struct windowing_plugin
    {
        std::string window_title = "";
        std::size_t window_width = 1920;
        std::size_t window_height = 1080;
        bool is_window_resizable = true;
        bool should_hide_window_until_first_render = true;
        bool is_window_fullscreen = false;

        auto init(application& app) const noexcept -> void;
    };
}
