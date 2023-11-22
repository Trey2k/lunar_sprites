#ifndef LOG_H
#define LOG_H

#include "core/types/string.h"

typedef enum {
	LOG_LEVEL_INFO,
	LOG_LEVEL_DEBUG,
	LOG_LEVEL_WARNING,
	LOG_LEVEL_ERROR
} LogLevel;

LS_EXPORT void ls_set_log_level(LogLevel level);
LS_EXPORT LogLevel ls_get_log_level();

LS_EXPORT void ls_log(LogLevel level, String message, ...);
LS_EXPORT void ls_log_fatal(String message, ...);

LS_EXPORT void ls_printf(String message, ...);

#endif // LOG_H