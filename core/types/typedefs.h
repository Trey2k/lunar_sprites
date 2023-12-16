#ifndef TYPEDEFS_H
#define TYPEDEFS_H

#define true 1
#define false 0

typedef int int32;
typedef int32 bool;
typedef unsigned char uint8;
typedef unsigned int uint32;
typedef long long int64;
typedef unsigned long long uint64;
typedef float float32;
typedef double float64;

typedef void *LSFile;

#if defined(__SIZE_TYPE__)
typedef __SIZE_TYPE__ size_t;
#else // __SIZE_TYPE__
typedef unsigned long long size_t;
#endif // __SIZE_TYPE__

#ifndef NULL
#define NULL ((void *)0)
#endif

#ifndef _STR
#define _STR(m_x) #m_x
#define _MKSTR(m_x) _STR(m_x)
#endif

// Should always inline no matter what.
#ifndef _ALWAYS_INLINE_
#if defined(__GNUC__)
#define _ALWAYS_INLINE_ __attribute__((always_inline)) inline static
#elif defined(_MSC_VER)
#define _ALWAYS_INLINE_ __forceinline static
#else
#define _ALWAYS_INLINE_ inline static
#endif
#endif

// Should always inline, except in dev builds because it makes debugging harder.
#ifndef _FORCE_INLINE_
#ifdef DEV_ENABLED
#define _FORCE_INLINE_ inline static
#else
#define _FORCE_INLINE_ _ALWAYS_INLINE_
#endif
#endif

#ifndef _NO_DISCARD_
#define _NO_DISCARD_ [[nodiscard]]
#endif

#ifndef _ALLOW_DISCARD_
#define _ALLOW_DISCARD_ (void)
#endif

#endif // TYPEDEFS_H
