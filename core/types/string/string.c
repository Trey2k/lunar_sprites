#include "core/types/string/string.h"
#include "core/debug.h"
#include "core/memory/memory.h"

#include <stdarg.h>
#include <stdio.h>
#include <string.h>

size_t string_length(String string) {
	LS_ASSERT(string);

	return strlen(string);
}

size_t string_hash_djb2(String string) {
	LS_ASSERT(string);

	size_t hash = 0;
	for (size_t i = 0; i < string_length(string); i++) {
		hash = ((hash << 5) + hash) + string[i];
	}

	return hash;
}

bool string_is_empty(String string) {
	LS_ASSERT(string);

	return string_length(string) == 0;
}

bool string_contains(String string, String substring) {
	LS_ASSERT(string);
	LS_ASSERT(substring);

	return strstr(string, substring) != NULL;
}

bool string_equals(String string1, String string2) {
	LS_ASSERT(string1);
	LS_ASSERT(string2);

	return strcmp(string1, string2) == 0;
}

char *string_copy(String string) {
	LS_ASSERT(string);

	size_t length = string_length(string);
	char *copy = ls_malloc(length + 1);
	strncpy(copy, string, length);
	copy[length] = '\0';

	return copy;
}

char *string_concat(String a, ...) {
	LS_ASSERT(a);

	va_list args;
	va_start(args, a);

	size_t length = string_length(a);
	char *string = string_copy(a);

	String arg = va_arg(args, String);
	while (arg) {
		length += string_length(arg);
		string = ls_realloc(string, length + 1);
		strcat(string, arg);
		arg = va_arg(args, String);
	}

	va_end(args);

	return string;
}

char *string_format(String format, ...) {
	LS_ASSERT(format);

	va_list args;
	va_start(args, format);

	size_t length = vsnprintf(NULL, 0, format, args);
	char *string = ls_malloc(length + 1);
	vsnprintf(string, length + 1, format, args);

	va_end(args);

	return string;
}

String string_char(String str, int32 c) {
	LS_ASSERT(str);

	return strchr(str, c);
}

String string_find(String str, String substr) {
	LS_ASSERT(str);
	LS_ASSERT(substr);

	return strstr(str, substr);
}