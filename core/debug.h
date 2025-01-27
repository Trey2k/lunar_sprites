#ifndef DEBUG_H

#include <signal.h>

#ifndef ls_str_format
char *ls_str_format(const char *format, ...);
#endif // ls_str_format

#ifndef ls_printf
void ls_printf(const char *format, ...);
#endif // ls_printf

#ifndef ls_free
void ls_free(void *ptr);
#endif // ls_free

#ifdef DEBUG_ENABLED
#define DEBUG_METHODS_ENABLED

#if defined(__GNUC__)

#if defined(__clang__) && __has_builtin(__builtin_debugtrap)
#define LS_BREAKPOINT() __builtin_debugtrap()
#else // __clang__

#if __has_builtin(__builtin_trap)
#define LS_BREAKPOINT() __builtin_trap()
#else // __has_builtin(__builtin_trap)
#define LS_BREAKPOINT() raise(SIGTRAP)
#endif // __has_builtin(__builtin_trap)

#endif // __clang__

#elif defined(_MSC_VER) // __GNUC__
#define LS_BREAKPOINT() __debugbreak()
#elif defined(__unix__) // __GNUC__
#define LS_BREAKPOINT() raise(SIGTRAP)
#else // __GNUC__
#define LS_BREAKPOINT() raise(SIGABRT)
#endif // __GNUC__

#define LS_ASSERT_MSG(condition, msg, ...)                   \
	if (!(condition)) {                                      \
		char *full_msg = ls_str_format(msg, __VA_ARGS__);    \
		ls_printf("%s\n\tFunction %s, file %s, line %d.\n",  \
				full_msg, __FUNCTION__, __FILE__, __LINE__); \
		ls_free(full_msg);                                   \
		LS_BREAKPOINT();                                     \
	}

#define LS_ASSERT(condition) \
	LS_ASSERT_MSG(condition, "Assertion `%s` failed.", _MKSTR(condition))

#define LS_STATIC_ASSERT(condition) \
	typedef char static_assertion[(condition) ? 1 : -1]

#else // DEBUG_ENABLED

#define LS_BREAKPOINT()

#define LS_ASSERT_MSG(condition, msg, ...) \
	_ALLOW_DISCARD_(condition);            \
	_ALLOW_DISCARD_(msg);                  \
	_ALLOW_DISCARD_(__VA_ARGS__)

#define LS_ASSERT(condition) \
	_ALLOW_DISCARD_(condition)

#define LS_STATIC_ASSERT(condition) \
	_ALLOW_DISCARD_(condition)

#endif // DEBUG_ENABLED

#endif // DEBUG_H