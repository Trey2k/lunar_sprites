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
			bstring_fwritef(stdout, "%s", "[INFO] ");
			bstring_fwrite_va(stdout, BSC(message), args);
		} break;

		case LOG_LEVEL_DEBUG: {
			bstring_fwritef(stdout, "%s", "[DEBUG] ");
			bstring_fwrite_va(stdout, BSC(message), args);
		} break;

		case LOG_LEVEL_WARNING: {
			bstring_fwritef(stdout, "%s", "[WARNING] ");
			bstring_fwrite_va(stdout, BSC(message), args);
		} break;

		case LOG_LEVEL_ERROR: {
			bstring_fwritef(stderr, "%s", "[ERROR] ");
			bstring_fwrite_va(stderr, BSC(message), args);
		} break;
	};

	va_end(args);
}

void ls_logb(LogLevel level, BString message, ...) {
	if (level < log_level) {
		return;
	}

	va_list args;
	va_start(args, message);

	switch (level) {
		case LOG_LEVEL_INFO: {
			bstring_fwritef(stdout, "%s", "[INFO] ");
			bstring_fwrite_va(stdout, message, args);
		} break;

		case LOG_LEVEL_DEBUG: {
			bstring_fwritef(stdout, "%s", "[DEBUG] ");
			bstring_fwrite_va(stdout, message, args);
		} break;

		case LOG_LEVEL_WARNING: {
			bstring_fwritef(stdout, "%s", "[WARNING] ");
			bstring_fwrite_va(stdout, message, args);
		} break;

		case LOG_LEVEL_ERROR: {
			bstring_fwritef(stderr, "%s", "[ERROR] ");
			bstring_fwrite_va(stderr, message, args);
		} break;
	};

	va_end(args);
}

void ls_log_fatal(String message, ...) {
	va_list args;
	va_start(args, message);

	bstring_fwritef(stderr, "%s", "[FATAL] ");
	bstring_fwrite_va(stderr, BSC(message), args);

	va_end(args);

	exit(EXIT_FAILURE);
}

void ls_log_fatalb(BString message, ...) {
	va_list args;
	va_start(args, message);

	bstring_fwritef(stderr, "%s", "[FATAL] ");
	bstring_fwrite_va(stderr, message, args);

	va_end(args);

	exit(EXIT_FAILURE);
}

void ls_printf(String message, ...) {
	va_list args;
	va_start(args, message);

	bstring_fwrite_va(stdout, BSC(message), args);

	va_end(args);
}

void ls_printfb(BString message, ...) {
	va_list args;
	va_start(args, message);

	bstring_fwrite_va(stdout, message, args);

	va_end(args);
}