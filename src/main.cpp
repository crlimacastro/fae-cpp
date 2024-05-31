#include "fae/main.hpp"

#ifdef FAE_PLATFORM_WEB
#include <emscripten/emscripten.h>
#else
#undef main
#include <SDL3/SDL_main.h>
#endif
#undef main

#ifdef FAE_PLATFORM_WEB
auto main(int argc, char* argv[]) -> int
{
    return fae_main(argc, argv);
}
#else
auto SDL_main(int argc, char* argv[]) -> int
{
    return fae_main(argc, argv);
}
#endif
