#include "core/types/string.h"
#include "core/debug.h"
#include "core/memory.h"

#include <stdarg.h>
#include <stdio.h>
#include <string.h>

int ls_sprintf(char *buffer, size_t buffer_size, String format, ...) {
	LS_ASSERT(buffer);
	LS_ASSERT(format);

	va_list args;
	va_start(args, format);

	int ret = vsnprintf(buffer, buffer_size, format, args);

	va_end(args);

	return ret;
}

int ls_fprintf(LSFile *stream, String format, ...) {
	LS_ASSERT(stream);
	LS_ASSERT(format);

	va_list args;
	va_start(args, format);

	int ret = vfprintf(stream, format, args);

	va_end(args);

	return ret;
}

size_t ls_str_length(String string) {
	LS_ASSERT(string);

	return strlen(string);
}

size_t ls_wstr_length(const wchar *string) {
	LS_ASSERT(string);

	size_t length = 0;
	while (string[length] != L'\0') {
		length++;
	}

	return length;
}

size_t ls_str_hash_djb2(String string) {
	LS_ASSERT(string);

	size_t hash = 0;
	for (size_t i = 0; i < ls_str_length(string); i++) {
		hash = ((hash << 5) + hash) + string[i];
	}

	return hash;
}

bool ls_str_is_empty(String string) {
	LS_ASSERT(string);

	return ls_str_length(string) == 0;
}

bool ls_str_contains(String string, String substring) {
	LS_ASSERT(string);
	LS_ASSERT(substring);

	return strstr(string, substring) != NULL;
}

bool ls_str_equals(String string1, String string2) {
	LS_ASSERT(string1);
	LS_ASSERT(string2);

	return strcmp(string1, string2) == 0;
}

bool ls_str_ends_with(String string, String substring) {
	LS_ASSERT(string);
	LS_ASSERT(substring);

	size_t string_length = ls_str_length(string);
	size_t substring_length = ls_str_length(substring);

	if (substring_length > string_length) {
		return false;
	}

	return strcmp(string + string_length - substring_length, substring) == 0;
}

bool ls_str_starts_with(String string, String substring) {
	LS_ASSERT(string);
	LS_ASSERT(substring);

	size_t string_length = ls_str_length(string);
	size_t substring_length = ls_str_length(substring);

	if (substring_length > string_length) {
		return false;
	}

	return strncmp(string, substring, substring_length) == 0;
}

char *ls_str_copy(String string) {
	LS_ASSERT(string);

	size_t length = ls_str_length(string);
	char *copy = ls_malloc(length + 1);
	strncpy(copy, string, length + 1);
	copy[length] = '\0';

	return copy;
}

void ls_str_copy_to(char *buffer, String string, size_t buffer_size) {
	LS_ASSERT(string);
	LS_ASSERT(buffer);

	strncpy(buffer, string, buffer_size);
	buffer[buffer_size - 1] = '\0';
}

char *ls_str_concat(String a, ...) {
	LS_ASSERT(a);

	va_list args;
	va_start(args, a);

	size_t length = ls_str_length(a);
	char *string = ls_str_copy(a);

	String arg = va_arg(args, String);
	while (arg) {
		length += ls_str_length(arg);
		string = ls_realloc(string, length + 1);
		strcat(string, arg);
		arg = va_arg(args, String);
	}

	va_end(args);

	return string;
}

char *ls_str_format(String format, ...) {
	LS_ASSERT(format);

	va_list args;
	va_start(args, format);

	size_t length = vsnprintf(NULL, 0, format, args);
	va_end(args); // End the va_list before starting it again

	char *string = ls_malloc(length + 1);

	va_start(args, format); // Start the va_list again before using it
	vsnprintf(string, length + 1, format, args);
	va_end(args);

	return string;
}

char *ls_str_replace(String string, String old, String new) {
	LS_ASSERT(string);
	LS_ASSERT(old);
	LS_ASSERT(new);

	size_t string_length = ls_str_length(string);
	size_t old_length = ls_str_length(old);
	size_t new_length = ls_str_length(new);

	size_t count = 0;
	for (size_t i = 0; i < string_length; i++) {
		if (strncmp(string + i, old, old_length) == 0) {
			count++;
			i += old_length - 1;
		}
	}

	size_t new_string_length = string_length + count * (new_length - old_length);
	char *new_string = ls_malloc(new_string_length + 1);

	size_t j = 0;
	for (size_t i = 0; i < string_length; i++) {
		if (strncmp(string + i, old, old_length) == 0) {
			strncpy(new_string + j, new, new_length);
			j += new_length;
			i += old_length - 1;
		} else {
			new_string[j] = string[i];
			j++;
		}
	}

	new_string[new_string_length] = '\0';

	return new_string;
}

String ls_str_char(String str, int32 c) {
	LS_ASSERT(str);

	return strchr(str, c);
}

String ls_str_find(String str, String substr) {
	LS_ASSERT(str);
	LS_ASSERT(substr);

	return strstr(str, substr);
}

bool ls_str_to_int32(String str, int32 *out_value) {
	LS_ASSERT(str);

	int32 value = 0;
	for (size_t i = 0; i < ls_str_length(str); i++) {
		if (str[i] < '0' || str[i] > '9') {
			return false;
		}

		value = value * 10 + (str[i] - '0');
	}

	if (out_value) {
		*out_value = value;
	}

	return true;
}

bool ls_str_to_float32(String str, float32 *out_value) {
	LS_ASSERT(str);

	float32 value = 0.0f;

	bool decimal = false;

	size_t len = ls_str_length(str);

	for (size_t i = 0; i < len; i++) {
		if (str[i] == '.') {
			if (decimal) {
				return false;
			}

			decimal = true;
			continue;
		}

		if ((str[i] == 'f' || str[i] == 'F') && i == len - 1) {
			break;
		}

		if (str[i] < '0' || str[i] > '9') {
			return false;
		}

		if (decimal) {
			value += (str[i] - '0') / 10.0f;
		} else {
			value = value * 10.0f + (str[i] - '0');
		}
	}

	if (out_value) {
		*out_value = value;
	}

	return true;
}

bool ls_str_to_bool(String str, bool *out_value) {
	LS_ASSERT(str);

	bool ret = false;
	char *lower_str = ls_str_copy(str);
	ls_str_to_lower(lower_str);

	if (ls_str_equals(lower_str, "true") ||
			ls_str_equals(lower_str, "yes")) {
		if (out_value) {
			*out_value = true;
		}

		ret = true;
	} else if (ls_str_equals(lower_str, "false") ||
			ls_str_equals(lower_str, "no")) {
		if (out_value) {
			*out_value = false;
		}

		ret = true;
	}

	ls_free(lower_str);
	return ret;
}

void ls_str_to_lower(char *string) {
	LS_ASSERT(string);

	for (size_t i = 0; i < ls_str_length(string); i++) {
		if (string[i] >= 'A' && string[i] <= 'Z') {
			string[i] += 'a' - 'A';
		}
	}
}

void ls_str_to_upper(char *string) {
	LS_ASSERT(string);

	for (size_t i = 0; i < ls_str_length(string); i++) {
		if (string[i] >= 'a' && string[i] <= 'z') {
			string[i] -= 'a' - 'A';
		}
	}
}