#ifndef DEBUG_H

#include "core/log.h"

#ifdef DEBUG_ENABLED
#define DEBUG_METHODS_ENABLED

#define LS_ASSERT_MSG(condition, msg, ...)                   \
	if (!(condition)) {                                      \
		ls_log_fatal("%s\nFunction %s, file %s, line %d.\n", \
                                                             \
				__PRETTY_FUNCTION__,                         \
				__FILE__,                                    \
				__LINE__);                                   \
	}

#define LS_ASSERT(condition) \
	LS_ASSERT_MSG(condition, "Assertion `%s` failed.\n", _MKSTR(condition));

#else // DEBUG_ENABLED

#define LS_ASSERT_MSG(condition, msg, ...) \
	_ALLOW_DISCARD_(condition);            \
	_ALLOW_DISCARD_(msg);                  \
	_ALLOW_DISCARD_(__VA_ARGS__);

#define LS_ASSERT(condition) \
	_ALLOW_DISCARD_(condition);

#endif // DEBUG_ENABLED

#endif // DEBUG_H