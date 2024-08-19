#pragma once

#ifdef FAE_PLATFORM_WINDOWS
#ifdef FAE_EXPORTS
#define FAE_API __declspec(dllexport)
#else
#define FAE_API __declspec(dllimport)
#endif
#else
#define FAE_API
#endif
