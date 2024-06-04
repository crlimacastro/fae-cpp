#include "fae/default_plugins.hpp"

namespace fae
{
    auto default_plugins::init(application& app) const noexcept -> void
    {
        app
            .add_plugin(time_plugin)
            .add_plugin(windowing_plugin)
            .add_plugin(input_plugin)
            .add_plugin(rendering_plugin);
    }
}
