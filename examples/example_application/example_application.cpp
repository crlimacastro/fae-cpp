#include <format>
#include <type_traits>

#include "fae/fae.hpp"
#include "fae/main.hpp"

auto start(const fae::start_step& step) noexcept -> void
{
    step.resources.use_resource<fae::renderer>([&](fae::renderer& renderer)
        { renderer.set_clear_color(fae::colors::cornflower_blue); });
        auto test = *step.assets.load<fae::mesh>("Stanford_Bunny.stl");
}

auto hue_shift_clear_color(const fae::update_step& step) noexcept -> void
{
    auto& time = step.resources.get_or_emplace<fae::time>(fae::time{});
    step.resources.use_resource<fae::renderer>(
        [&](fae::renderer& renderer)
        {
            const auto speed = 200.f;
            const auto delta_h = speed * time.delta().seconds_f32();
            auto clear_color_hsva = renderer.get_clear_color().to_hsva();
            clear_color_hsva.h = static_cast<int>((clear_color_hsva.h + delta_h)) % 360;
            auto new_clear_color = clear_color_hsva.to_rgba();
            renderer.set_clear_color(new_clear_color);
        });
}

auto update(const fae::update_step& step) noexcept -> void
{
}

auto render(const fae::render_step& step) noexcept -> void
{
    step.resources.use_resource<fae::renderer>(
        [&](fae::renderer& renderer)
        {
            auto time = step.resources.get_or_emplace<fae::time>(fae::time{});
            auto dt = time.delta().seconds_f32();
            auto t = time.elapsed().seconds_f32();

            static auto hsva = fae::color_hsva::from_rgba(fae::colors::red);
            hsva.h = static_cast<float>(static_cast<int>(hsva.h + dt * 120) % 360);
            auto tint = hsva.to_rgba();

            static auto rotation = fae::quat(0.f, 0.f, 0.f, 1.f);
            rotation *= fae::math::angleAxis(fae::math::radians(60.f) * dt, fae::vec3(0.0f, 1.0f, 0.0f));

            renderer.render_cube(fae::renderer::draw_cube_args{
                .position = { 1.2f * std::cos(t), 1.2f * std::sinf(t * 2), -5.f },
                .rotation = rotation,
                .tint = tint,
            });

            renderer.render_cube(fae::renderer::draw_cube_args{
                .position = { 1.2f * std::cos(t * 2), 1.2f * std::sinf(t * 4), -8.f },
                .rotation = rotation,
                .scale = fae::vec3{1.f} * (std::sin(t) + 1.5f) * 0.5f,
            });
        });
}

auto fae_main(int argc, char* argv[]) -> int
{
    fae::application{}
        .add_plugin(fae::default_plugins{})
        .add_system<fae::start_step>(start)
        .add_system<fae::update_step>(fae::quit_on_esc)
        // .add_system<fae::update_step>(hue_shift_clear_color)
        .add_system<fae::update_step>(update)
        .add_system<fae::render_step>(render)
        .run();
    return fae::exit_success;
}
