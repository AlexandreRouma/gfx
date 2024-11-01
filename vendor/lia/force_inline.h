#pragma once

#ifdef _MSC_VER
#define LIA_FORCE_INLINE    inline __forceinline
#else
#define LIA_FORCE_INLINE    inline __attribute__((always_inline))
#endif