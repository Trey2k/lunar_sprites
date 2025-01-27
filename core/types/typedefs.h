#ifndef TYPEDEFS_H
#define TYPEDEFS_H

#include "core/debug.h"

typedef signed char char8;
typedef signed short int char16;
typedef signed int char32;
typedef unsigned char uchar8;
typedef unsigned short int uchar16;
typedef unsigned int uchar32;

#if defined(WINDOWS_ENABLED)
typedef unsigned short int wchar;
#define WCHAR_IS_CHAR16
#else
typedef int wchar;
#define WCHAR_IS_CHAR32
#endif // WINDOWS_ENABLED

typedef signed char int8;
typedef unsigned char uint8;
typedef signed short int16;
typedef unsigned short uint16;
typedef signed int int32;
typedef unsigned int uint32;
typedef signed long long int64;
typedef unsigned long long uint64;
typedef float float32;
typedef double float64;

LS_STATIC_ASSERT(sizeof(char8) == 1);
LS_STATIC_ASSERT(sizeof(char16) == 2);
LS_STATIC_ASSERT(sizeof(char32) == 4);
LS_STATIC_ASSERT(sizeof(uchar8) == 1);
LS_STATIC_ASSERT(sizeof(uchar16) == 2);
LS_STATIC_ASSERT(sizeof(uchar32) == 4);
LS_STATIC_ASSERT(sizeof(wchar) == 2 || sizeof(wchar) == 4);
LS_STATIC_ASSERT(sizeof(int8) == 1);
LS_STATIC_ASSERT(sizeof(uint8) == 1);
LS_STATIC_ASSERT(sizeof(int16) == 2);
LS_STATIC_ASSERT(sizeof(uint16) == 2);
LS_STATIC_ASSERT(sizeof(int32) == 4);
LS_STATIC_ASSERT(sizeof(uint32) == 4);
LS_STATIC_ASSERT(sizeof(int64) == 8);
LS_STATIC_ASSERT(sizeof(uint64) == 8);
LS_STATIC_ASSERT(sizeof(float32) == 4);
LS_STATIC_ASSERT(sizeof(float64) == 8);

typedef __builtin_va_list va_list;

typedef struct Variant Variant;

#if !defined(WEB_ENABLED)
typedef unsigned char bool;
#define true 1
#define false 0
#else
#include <stdbool.h>
#endif

LS_STATIC_ASSERT(sizeof(bool) == 1);

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
