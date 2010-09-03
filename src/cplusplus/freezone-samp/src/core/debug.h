#ifndef DEBUG_H
#define DEBUG_H

#ifndef NDEBUG
#  define DEBUG_FLAG0 " [debug]"
#else
#  define DEBUG_FLAG0
#endif

#ifdef DEVELOP
#  define DEBUG_FLAG DEBUG_FLAG0 " [dev]"
#else
#  define DEBUG_FLAG DEBUG_FLAG0
#endif

#if defined(_DEBUG) && defined(_MSC_VER)
#include <crtdbg.h>
#endif

inline void debug_init() {
#   if defined(_DEBUG) && defined(_MSC_VER)
    // В отладчике прописывать:
    // {,,msvcr90d.dll}_crtBreakAlloc

    // Get current flag
    int tmpFlag = _CrtSetDbgFlag( _CRTDBG_REPORT_FLAG );

    // Turn on leak-checking bit.
    tmpFlag |= _CRTDBG_LEAK_CHECK_DF;

    // Turn off CRT block checking bit.
    tmpFlag &= ~_CRTDBG_CHECK_CRT_DF;

    // Set flag to the new value.
    _CrtSetDbgFlag( tmpFlag );
#   endif // #if defined(_DEBUG) && defined(_MSC_VER)
}

#endif // DEBUG_H
