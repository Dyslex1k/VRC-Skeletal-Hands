#pragma once

#include "hekky/osc/platform.hpp"

#if _DEBUG
#if defined(HEKKYOSC_WINDOWS)
#define DEBUGBREAK() __debugbreak()
#elif defined(HEKKYOSC_LINUX)
#include <signal.h>
#define DEBUGBREAK() raise(SIGTRAP)
#else
#pragma message(": warning HEKKYOSC: Debug break not available. Falling back to abort()...")
#define DEBUGBREAK() abort()
#endif
#else
#define DEBUGBREAK() ;
#endif