#include "core/core_log.h"

#include "core/core.h"

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>

static LogLevel log_level = LOG_LEVEL_INFO;

void core_log_set_level(LogLevel level) {
	log_level = level;
}

LogLevel core_log_get_level() {
	return log_level;
}

void core_log(LogLevel level, String message, ...) {
	if (level < log_level) {
		printf("test");
		return;
	}

	va_list args;
	va_start(args, message);

	switch (level) {
		case LOG_LEVEL_INFO: {
			fprintf(stdout, "%s", "[INFO] ");
			vfprintf(stdout, message, args);
		} break;

		case LOG_LEVEL_DEBUG: {
			fprintf(stdout, "%s", "[DEBUG] ");
			vfprintf(stdout, message, args);
		} break;

		case LOG_LEVEL_WARNING: {
			fprintf(stdout, "%s", "[WARNING] ");
			vfprintf(stdout, message, args);
		} break;

		case LOG_LEVEL_ERROR: {
			fprintf(stderr, "%s", "[ERROR] ");
			vfprintf(stderr, message, args);
		} break;

		case LOG_LEVEL_FATAL: {
			fprintf(stderr, "%s", "[FATAL] ");
			vfprintf(stderr, message, args);
		} break;
	};

	va_end(args);
}