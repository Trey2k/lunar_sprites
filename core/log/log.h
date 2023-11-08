#ifndef LOG_H
#define LOG_H

#include "core/types/string/string.h"

typedef enum {
	LOG_LEVEL_INFO,
	LOG_LEVEL_DEBUG,
	LOG_LEVEL_WARNING,
	LOG_LEVEL_ERROR,
	LOG_LEVEL_FATAL
} LogLevel;

void ls_set_log_level(LogLevel level);

void ls_log(LogLevel level, String message, ...);
void ls_log_fatal(String message, ...);

LogLevel ls_get_log_level();

#endif // LOG_H