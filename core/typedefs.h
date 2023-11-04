#ifndef TYPEDEFS_H
#define TYPEDEFS_H

typedef int bool;
#define true 1
#define false 0

typedef int int32;
typedef unsigned int uint32;
typedef long long int64;
typedef unsigned long long uint64;
typedef __SIZE_TYPE__ size_t;

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

// Windows badly defines a lot of stuff we'll never use. Undefine it.
#ifdef _WIN32
#undef min // override standard definition
#undef max // override standard definition
#undef ERROR // override (really stupid) wingdi.h standard definition
#undef DELETE // override (another really stupid) winnt.h standard definition
#undef MessageBox // override winuser.h standard definition
#undef Error
#undef OK
#undef CONNECT_DEFERRED // override from Windows SDK, clashes with Object enum
#endif

#ifdef DEBUG_ENABLED
#define DEBUG_METHODS_ENABLED

#ifndef CORE_ASSERT_MSG
#define CORE_ASSERT_MSG(condition, msg, ...)           \
	if (!(condition)) {                                \
		core_log(LOG_LEVEL_FATAL, msg, ##__VA_ARGS__); \
		core_fatal("Function %s, file %s, line %d.\n", \
				_MKSTR(condition),                     \
				__PRETTY_FUNCTION__,                   \
				__FILE__,                              \
				__LINE__);                             \
	}

#endif

#ifndef CORE_ASSERT
#define CORE_ASSERT(condition)                                          \
	if (!(condition)) {                                                 \
		CORE_ASSERT_MSG("Assertion `%s` failed.\n", _MKSTR(condition)); \
	}
#endif

#else

#ifndef CORE_ASSERT_MSG
#define CORE_ASSERT_MSG(condition, msg, ...) \
	_ALLOW_DISCARD_(condition);              \
	_ALLOW_DISCARD_(msg);                    \
	_ALLOW_DISCARD_(__VA_ARGS__);
#endif

#ifndef CORE_ASSERT
#define CORE_ASSERT(condition) \
	_ALLOW_DISCARD_(condition);
#endif

#endif

#endif // TYPEDEFS_H
