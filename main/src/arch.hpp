#pragma once

#if defined(__x86_64__) || defined(_M_AMD64)
    #define DM_X64
#elif defined(__i386__) || defined(_M_IX86)
    #define DM_X86
#else
    #error "Unsupported architecture"
#endif
