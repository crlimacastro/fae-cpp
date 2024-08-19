#include "fae/time.hpp"

#include "fae/application/application.hpp"

namespace fae
{
    auto update_time(const update_step& step) noexcept -> void
    {
        auto& time = step.resources.use_resource<fae::time>(
            [](fae::time& time)
            {
                static auto last_time = std::chrono::high_resolution_clock::now();
                auto current_time = std::chrono::high_resolution_clock::now();
                time.unscaled_delta = current_time - last_time;
                time.unscaled_elapsed += time.unscaled_delta;
                last_time = current_time;
            });
    }

    auto time_plugin::init(application& app) const noexcept -> void
    {
        app
            .emplace_resource<time>(time{})
            .add_system<update_step>(update_time);
    }

}
