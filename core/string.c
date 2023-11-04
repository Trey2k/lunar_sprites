#include "core/string.h"

#include "core/core.h"
#include "core/typedefs.h"
#include <stdarg.h>
#include <stdio.h>
#include <string.h>

size_t string_length(const String string) {
	CORE_ASSERT(string);

	return strlen(string);
}

bool string_is_empty(const String string) {
	CORE_ASSERT(string);

	return string_length(string) == 0;
}

bool string_contains(const String string, const String substring) {
	CORE_ASSERT(string);
	CORE_ASSERT(substring);

	return strstr(string, substring) != NULL;
}

bool string_equals(const String string1, const String string2) {
	CORE_ASSERT(string1);
	CORE_ASSERT(string2);

	return strcmp(string1, string2) == 0;
}

String string_copy(const String string) {
	CORE_ASSERT(string);

	size_t length = string_length(string);
	char *copy = core_malloc(length + 1);
	strncpy(copy, string, length);
	copy[length] = '\0';

	return copy;
}

String string_concat(const String a, ...) {
	CORE_ASSERT(a);

	va_list args;
	va_start(args, a);

	size_t length = string_length(a);
	String string = a;

	String arg = va_arg(args, String);
	while (arg) {
		length += string_length(arg);
		string = core_realloc(string, length + 1);
		strcat(string, arg);
		arg = va_arg(args, String);
	}

	va_end(args);

	return string;
}

String string_format(const String format, ...) {
	CORE_ASSERT(format);

	va_list args;
	va_start(args, format);

	size_t length = vsnprintf(NULL, 0, format, args);
	String string = core_malloc(length + 1);
	vsnprintf(string, length + 1, format, args);

	va_end(args);

	return string;
}