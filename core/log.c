#include "core/log.h"

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>

static LogLevel log_level = LOG_LEVEL_INFO;

void ls_set_log_level(LogLevel level) {
	log_level = level;
}

LogLevel ls_get_log_level() {
	return log_level;
}

void ls_log(LogLevel level, String message, ...) {
	if (level < log_level) {
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
	};

	va_end(args);
}

void ls_log_fatal(String message, ...) {
	va_list args;
	va_start(args, message);

	fprintf(stderr, "%s", "[FATAL] ");
	vfprintf(stderr, message, args);

	va_end(args);

	exit(EXIT_FAILURE);
}

void ls_printf(String message, ...) {
	va_list args;
	va_start(args, message);

	vfprintf(stdout, message, args);

	va_end(args);
}