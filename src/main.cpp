module;

#ifndef __EMSCRIPTEN__
#include <SDL3/SDL_main.h>
#endif

auto fae_main(int argc, char *argv[]) -> int;

#ifndef __EMSCRIPTEN__
int SDL_main(int argc, char *argv[])
{
	return fae_main(argc, argv);
}
#else
int main(int argc, char *argv[])
{
	return fae_main(argc, argv);
}
#endif

#undef main
export module fae:main;

// e.g.
// import fae;

// auto fae_main(int argc, char *argv[]) -> int
// {
// 	fae::application{}
// 		.add_plugin(fae::default_plugins{})
// 		.run();
// 	return fae::exit_success;
// }
