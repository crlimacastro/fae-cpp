#pragma once

#include <cstdlib>

#include "fae/core/optional_reference.hpp"
#include "fae/core/match.hpp"
#include "fae/core/enum.hpp"
#include "fae/core/vector.hpp"
#include "fae/core/deleter.hpp"
#include "fae/core/inocopy.hpp"
#include "fae/core/inomove.hpp"

#ifdef FAE_PLATFORM_WINDOWS
#ifdef FAE_EXPORTS
#define FAE_API __declspec(dllexport)
#else
#define FAE_API __declspec(dllimport)
#endif
#else
#define FAE_API
#endif

namespace fae
{
    constexpr int exit_success = EXIT_SUCCESS;
    constexpr int exit_failure = EXIT_FAILURE;
}
