#include "fae/main.hpp"

#ifndef FAE_PLATFORM_WEB
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