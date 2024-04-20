#pragma once

#include "hekky/osc/debug.hpp"

#ifndef HEKKYOSC_LOG
#define HEKKYOSC_LOG(x) std::cout << x
#endif

#ifndef HEKKYOSC_ERR
#define HEKKYOSC_ERR(x) std::cerr << x
#endif

#if defined(HEKKYOSC_DOASSERTS)
#include <iostream>
#define HEKKYOSC_ASSERT(condition, message) \
{ \
    if (!(condition)) \
    { \
        std::string msg = std::string("Assert failed:\t") + message + "\nExpected:\t" + #condition + "\nSource:\t\t" + __FILE__ + ", line " + std::to_string(__LINE__) + "\n"; \
        HEKKYOSC_ERR(msg); \
        DEBUGBREAK(); \
    } \
}
#else
#define HEKKYOSC_ASSERT(condition, message) ;
#endif