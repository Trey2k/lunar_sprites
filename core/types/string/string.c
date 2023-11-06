#include "core/types/string.h"

#include "core/core.h"
#include "core/typedefs.h"
#include <stdarg.h>
#include <stdio.h>
#include <string.h>

size_t string_length(String string) {
	CORE_ASSERT(string);

	return strlen(string);
}

size_t string_hash_djb2(String string) {
	CORE_ASSERT(string);

	size_t hash = 0;
	for (size_t i = 0; i < string_length(string); i++) {
		hash = ((hash << 5) + hash) + string[i];
	}

	return hash;
}

bool string_is_empty(String string) {
	CORE_ASSERT(string);

	return string_length(string) == 0;
}

bool string_contains(String string, String substring) {
	CORE_ASSERT(string);
	CORE_ASSERT(substring);

	return strstr(string, substring) != NULL;
}

bool string_equals(String string1, String string2) {
	CORE_ASSERT(string1);
	CORE_ASSERT(string2);

	return strcmp(string1, string2) == 0;
}

char *string_copy(String string) {
	CORE_ASSERT(string);

	size_t length = string_length(string);
	char *copy = core_malloc(length + 1);
	strncpy(copy, string, length);
	copy[length] = '\0';

	return copy;
}

char *string_concat(String a, ...) {
	CORE_ASSERT(a);

	va_list args;
	va_start(args, a);

	size_t length = string_length(a);
	char *string = string_copy(a);

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

char *string_format(String format, ...) {
	CORE_ASSERT(format);

	va_list args;
	va_start(args, format);

	size_t length = vsnprintf(NULL, 0, format, args);
	char *string = core_malloc(length + 1);
	vsnprintf(string, length + 1, format, args);

	va_end(args);

	return string;
}

String string_char(String str, int32 c) {
	CORE_ASSERT(str);

	return strchr(str, c);
}

String string_find(String str, String substr) {
	CORE_ASSERT(str);
	CORE_ASSERT(substr);

	return strstr(str, substr);
}