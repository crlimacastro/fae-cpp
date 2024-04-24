module;
#include <SDL3/SDL_main.h>

auto fae_main(int argc, char *argv[]) -> int;

int SDL_main(int argc, char *argv[])
{
	return fae_main(argc, argv);
}

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
