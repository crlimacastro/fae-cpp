#include "fae/default_plugins.hpp"

#include "fae/application/application.hpp"

namespace fae
{
    auto default_plugins::init(application& app) const noexcept -> void
    {
        app
            .add_plugin(time_plugin)
            .add_plugin(windowing_plugin)
            .add_plugin(input_plugin)
            .add_plugin(rendering_plugin)
            .add_plugin(lighting_plugin)
            .add_plugin(ui_plugin)
            ;
    }
}
