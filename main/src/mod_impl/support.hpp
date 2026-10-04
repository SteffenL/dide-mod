#pragma once

#if defined(__x86_64__) || defined(_M_AMD64)
    #define CE6_SUPPORTED
#elif defined(__i386__) || defined(_M_IX86)
    #define CE5_SUPPORTED
#else
    #error "Unsupported architecture"
#endif
