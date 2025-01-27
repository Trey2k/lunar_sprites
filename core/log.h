#ifndef LOG_H
#define LOG_H

#include "core/types/bstring.h"
#include "core/types/string.h"

typedef enum {
	LOG_LEVEL_INFO,
	LOG_LEVEL_DEBUG,
	LOG_LEVEL_WARNING,
	LOG_LEVEL_ERROR
} LogLevel;

LS_EXPORT void ls_set_log_level(LogLevel level);
LS_EXPORT LogLevel ls_get_log_level();

// Supports the normal C format specifiers except for %A, plus the following:
// %S - BString
// %v2i - Vector2i
// %v2u - Vector2u
// %v2 - Vector2
// %v3i - Vector3i
// %v3u - Vector3u
// %v3 - Vector3
// %C - Color
// %O - Object
// %R - Resource
// %D - Dictionary
// %A - Array - Replaced normal C specifier %A, %a can be used instead.
// %V - Variant
LS_EXPORT void ls_log(LogLevel level, String message, ...);
LS_EXPORT void ls_logb(LogLevel level, BString message, ...);
LS_EXPORT void ls_log_fatal(String message, ...);
LS_EXPORT void ls_log_fatalb(BString message, ...);

LS_EXPORT void ls_printf(String message, ...);
LS_EXPORT void ls_printfb(BString message, ...);

#endif // LOG_H