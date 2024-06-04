#include "fae/rendering/sdl_renderer.hpp"

#include <cstdint>

#include "fae/color.hpp"
#include "fae/rendering/renderer.hpp"
#include "fae/resource_manager.hpp"
#include "fae/sdl.hpp"

namespace fae
{
    [[nodiscard]] auto make_sdl_renderer(resource_manager& resources) noexcept -> renderer
    {
        return renderer{
            .get_clear_color = [&]() -> color
            {
                auto clear_color = colors::black;
                resources.use_resource<sdl_renderer>(
                    [&](sdl_renderer& renderer)
                    {
                        std::uint8_t r{}, g{}, b{}, a{};
                        SDL_GetRenderDrawColor(renderer.raw.get(), &r, &g, &b, &a);
                        clear_color = color{r, g, b, a};
                    });
                return clear_color;
            },
            .set_clear_color =
                [&](color value)
            {
                resources.use_resource<sdl_renderer>(
                    [&](sdl_renderer& renderer)
                    {
                        SDL_SetRenderDrawColor(renderer.raw.get(), value.r, value.g,
                            value.b, value.a);
                    });
            },
            .clear =
                [&]()
            {
                resources.use_resource<sdl_renderer>(
                    [&](sdl_renderer& renderer)
                    { SDL_RenderClear(renderer.raw.get()); });
            },
            .begin =
                [&]()
            {
                // do nothing
            },
            .end =
                [&]()
            {
                resources.use_resource<sdl_renderer>(
                    [&](sdl_renderer& renderer)
                    { SDL_RenderPresent(renderer.raw.get()); });
            },
        };
    }
} // namespace fae
