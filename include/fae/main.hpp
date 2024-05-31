#pragma once

auto fae_main(int argc, char* argv[]) -> int;

#undef main
#define main fae_main

// e.g.
// #include "fae/fae.hpp"

// auto main(int argc, char *argv[]) -> int
// {
// 	fae::application{}
// 		.add_plugin(fae::default_plugins{})
// 		.run();
// 	return fae::exit_success;
// }
