#ifndef CORE_LOG_H
#define CORE_LOG_H

#include "core/typedefs.h"

typedef enum {
	LOG_LEVEL_DEBUG,
	LOG_LEVEL_INFO,
	LOG_LEVEL_WARNING,
	LOG_LEVEL_ERROR,
	LOG_LEVEL_FATAL
} LogLevel;

void core_log(LogLevel level, String message, ...);
void core_set_log_level(LogLevel level);

LogLevel core_get_log_level();

#endif // CORE_LOG_H