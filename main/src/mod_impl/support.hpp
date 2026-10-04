#pragma once

#include "../arch.hpp"

#if defined(DM_X64)
    #define CE6_SUPPORTED
#elif defined(DM_X86)
    #define CE5_SUPPORTED
#else
    #error "Unsupported architecture"
#endif
