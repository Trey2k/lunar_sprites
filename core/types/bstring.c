#include "core/types/bstring.h"

#include "core/core.h"
#include "core/types/cow_data.h"
#include "core/types/string.h"

#include <stdarg.h>
#include <stdio.h>

static uint32 utf16_strlen(const char16 *utf16);
static uint32 utf32_strlen(const char32 *utf32);
static char *utf16_to_utf8(const char16 *utf16, uint32 length, uint32 *out_length);
static char16 *utf8_to_utf16(const char *utf8, uint32 length, uint32 *out_length);
static char *utf32_to_utf8(const char32 *utf32, uint32 length, uint32 *out_length);
static char32 *utf8_to_utf32(const char *utf8, uint32 length, uint32 *out_length);

static void bstring_make_mutable(BString *string) {
	if (string->is_const) {
		string->is_const = false;
		const char *old_string = string->c_const;
		string->c_const = NULL;
		cowdata_set(&string->cow_data, old_string, 1, string->length);
	}
}

static void bstring_resize(BString *string, uint32 new_length) {
	bstring_make_mutable(string);
	cowdata_resize(&string->cow_data, 1, new_length);
}

BString bstring_copy(const BString string) {
	BString copy = BSTRING_EMPTY;
	copy.length = string.length;
	if (string.is_const) {
		copy.c_const = string.c_const;
		copy.is_const = true;
		return copy;
	}

	copy.cow_data = string.cow_data;
	copy.is_const = false;
	cowdata_ref(&copy.cow_data);

	return copy;
}

BString bstring_substring(const BString string, uint32 start, uint32 end) {
	LS_ASSERT(start <= end);
	LS_ASSERT(end <= string.length);

	BString sub = BSTRING_EMPTY;
	sub.length = end - start;
	if (string.is_const) {
		sub.c_const = string.c_const + start;
		sub.is_const = true;
		return sub;
	}

	sub.c_const = cowdata_get_data_ptr(&string.cow_data) + start;
	sub.is_const = true;

	return sub;
}

void bstring_ref(BString str) {
	if (str.is_const) {
		return;
	}

	cowdata_ref(&str.cow_data);
}

void bstring_unref(BString str) {
	if (str.is_const) {
		return;
	}

	cowdata_unref(&str.cow_data);
}

char bstring_get(const BString string, uint32 index) {
	LS_ASSERT(index < string.length);

	if (string.is_const) {
		return string.c_const[index];
	}

	return cowdata_get_byte(&string.cow_data, index);
}

uint64 bstring_hash(const BString string) {
	uint64 hash = 5381;
	for (uint32 i = 0; i < string.length; i++) {
		hash = ((hash << 5) + hash) + bstring_get(string, i);
	}

	return hash;
}

void bstring_append(BString *string, const BString append) {
	LS_ASSERT(string);

	uint32 new_length = string->length + append.length;

	if (new_length == 0)
		return;

	bstring_make_mutable(string);

	const char *append_string = append.is_const ? append.c_const : cowdata_get_data_ptr(&append.cow_data);
	cowdata_append(&string->cow_data, append_string, 1, append.length);
	string->length = new_length;
}

void bstring_append_cstr(BString *string, const char *append, uint32 length) {
	LS_ASSERT(string);
	LS_ASSERT(append);

	uint32 new_length = string->length + length;

	if (new_length == 0)
		return;

	bstring_make_mutable(string);

	cowdata_append(&string->cow_data, append, 1, length);
	string->length = new_length;
}

void bstring_append_char(BString *string, char append) {
	LS_ASSERT(string);

	uint32 new_length = string->length + 1;

	if (new_length == 0)
		return;

	bstring_make_mutable(string);

	cowdata_append_byte(&string->cow_data, append);
	string->length = new_length;
}

void bstring_insert(BString *string, const BString insert, uint32 index) {
	LS_ASSERT(string);
	uint64 new_length = index + insert.length;
	bstring_resize(string, new_length);

	char *data = cowdata_get_data_ptrw(&string->cow_data);
	const char *src = insert.is_const ? insert.c_const : cowdata_get_data_ptr(&insert.cow_data);
	ls_memmove(data + index + insert.length, data + index, string->length - index);
	ls_memcpy(data + index, src, insert.length);

	string->length = new_length;
}

void bstring_insert_cstr(BString *string, const char *insert, uint32 index) {
	LS_ASSERT(string);
	LS_ASSERT(insert);
	uint64 insert_length = ls_str_length(insert);
	uint64 new_length = index + insert_length;
	bstring_resize(string, new_length);

	char *data = cowdata_get_data_ptrw(&string->cow_data);
	ls_memmove(data + index + insert_length, data + index, string->length - index);
	ls_memcpy(data + index, insert, insert_length);

	string->length = new_length;
}

void bstring_insert_char(BString *string, char insert, uint32 index) {
	LS_ASSERT(string);
	LS_ASSERT(index <= string->length);
	bstring_make_mutable(string);
	cowdata_set_byte(&string->cow_data, index, insert);
}

void bstring_remove(BString *string, uint32 start, uint32 end) {
	LS_ASSERT(string);
	LS_ASSERT(start <= string->length);
	LS_ASSERT(end <= string->length);
	LS_ASSERT(start <= end);

	if (string->is_const) {
		string->is_const = false;
		string->c_const = NULL;
	}

	char *data = cowdata_get_data_ptrw(&string->cow_data);
	ls_memmove(data + start, data + end, string->length - end);
	bstring_resize(string, string->length - (end - start));
	string->length -= (end - start);
}

BString bstring_format(const char *format, ...) {
	va_list args;
	va_start(args, format);
	BString formatted = bstring_format_va(format, args);
	va_end(args);

	return formatted;
}

BString bstring_format_va(const char *format, va_list args) {
	return bstring_formatb_va(BSC(format), args);
}

BString bstring_formatb(const BString format, ...) {
	va_list args;
	va_start(args, format);
	BString formatted = bstring_formatb_va(format, args);
	va_end(args);

	return formatted;
}

void bstring_fwrite(LSFile file, const BString string) {
	LS_ASSERT(file);
	const char *data = string.is_const ? string.c_const : cowdata_get_data_ptr(&string.cow_data);
	fwrite(data, 1, string.length, file);
}

void bstring_fwritef(LSFile file, const char *format, ...) {
	va_list args;
	va_start(args, format);
	bstring_fwrite_va(file, BSC(format), args);
	va_end(args);
}

void bstring_fwritefb(LSFile file, const BString format, ...) {
	va_list args;
	va_start(args, format);
	bstring_fwrite_va(file, format, args);
	va_end(args);
}

char *bstring_encode_ascii(const BString string) {
	char *encoded = ls_malloc(string.length + 1);
	const char *data = string.is_const ? string.c_const : cowdata_get_data_ptr(&string.cow_data);
	ls_memcpy(encoded, data, string.length);
	encoded[string.length] = '\0';

	return encoded;
}

BString bstring_decode_ascii(const char *string) {
	LS_ASSERT(string);

	uint32 length = ls_str_length(string);

	BString decoded = BSTRING_EMPTY;
	cowdata_set(&decoded.cow_data, string, 1, length);

	decoded.length = length;

	return decoded;
}

char *bstring_encode_utf8(const BString string) {
	char *encoded = ls_malloc((string.length + 1) * sizeof(char));
	const char *data = string.is_const ? string.c_const : cowdata_get_data_ptr(&string.cow_data);
	ls_memcpy(encoded, data, string.length);
	encoded[string.length] = '\0';

	return encoded;
}

BString bstring_decode_utf8(const char *string) {
	LS_ASSERT(string);

	uint32 length = ls_str_length(string);

	BString decoded = BSTRING_EMPTY;
	cowdata_set(&decoded.cow_data, string, 1, length);

	decoded.length = length;

	return decoded;
}

char16 *bstring_encode_utf16(const BString string) {
	const char *data = string.is_const ? string.c_const : cowdata_get_data_ptr(&string.cow_data);
	return utf8_to_utf16(data, string.length, NULL);
}

BString bstring_decode_utf16(const char16 *string) {
	LS_ASSERT(string);

	BString decoded = BSTRING_EMPTY;
	char *c_str = utf16_to_utf8(string, utf16_strlen(string), &decoded.length);
	cowdata_set(&decoded.cow_data, c_str, 1, decoded.length);

	return decoded;
}

char32 *bstring_encode_utf32(const BString string) {
	const char *data = string.is_const ? string.c_const : cowdata_get_data_ptr(&string.cow_data);
	return utf8_to_utf32(data, string.length, NULL);
}

BString bstring_decode_utf32(const char32 *string) {
	LS_ASSERT(string);

	BString decoded = BSTRING_EMPTY;
	char *c_str = utf32_to_utf8(string, utf32_strlen(string), &decoded.length);
	cowdata_set(&decoded.cow_data, c_str, 1, decoded.length);

	ls_free(c_str);

	return decoded;
}

bool bstring_is_empty(const BString *string) {
	LS_ASSERT(string);

	return string->length == 0;
}

bool bstring_equals(const BString a, const BString b) {
	if (a.length != b.length) {
		return false;
	}

	for (uint32 i = 0; i < a.length; i++) {
		if (bstring_get(a, i) != bstring_get(b, i)) {
			return false;
		}
	}

	return true;
}

bool bstring_starts_with(const BString string, const BString substring) {
	if (substring.length > string.length) {
		return false;
	}

	for (uint32 i = 0; i < substring.length; i++) {
		if (bstring_get(string, i) != bstring_get(substring, i)) {
			return false;
		}
	}

	return true;
}

bool bstring_ends_with(const BString string, const BString substring) {
	if (substring.length > string.length) {
		return false;
	}

	uint32 offset = string.length - substring.length;
	for (uint32 i = 0; i < substring.length; i++) {
		if (bstring_get(string, offset + i) != bstring_get(substring, i)) {
			return false;
		}
	}

	return true;
}

bool bstring_contains(const BString string, const BString substring) {
	if (substring.length > string.length) {
		return false;
	}

	for (uint32 i = 0; i < string.length - substring.length; i++) {
		if (bstring_starts_with(bstring_substring(string, i, i + substring.length), substring)) {
			return true;
		}
	}

	return false;
}

bool bstring_is_number(const BString string) {
	if (string.length == 0) {
		return false;
	}

	bool found_dot = false;
	for (uint32 i = 0; i < string.length; i++) {
		if (bstring_get(string, i) < '0' || bstring_get(string, i) > '9') {
			if (bstring_get(string, i) == '.' && !found_dot) {
				found_dot = true;
			} else {
				return false;
			}
		}
	}

	return true;
}

int64 bstring_to_int(const BString string) {
	int64 result = 0;
	int64 sign = 1;
	uint32 i = 0;

	if (bstring_get(string, 0) == '-') {
		sign = -1;
		i++;
	}

	for (; i < string.length; i++) {
		result = result * 10 + bstring_get(string, i) - '0';
	}

	return result * sign;
}

float64 bstring_to_float(const BString string) {
	float64 result = 0.0;
	float64 sign = 1.0;
	uint32 i = 0;

	if (bstring_get(string, 0) == '-') {
		sign = -1.0;
		i++;
	}

	for (; i < string.length; i++) {
		if (bstring_get(string, i) == '.') {
			break;
		}

		result = result * 10.0 + bstring_get(string, i) - '0';
	}

	float64 fraction = 0.0;
	float64 multiplier = 0.1;
	for (i++; i < string.length; i++) {
		fraction += (bstring_get(string, i) - '0') * multiplier;
		multiplier *= 0.1;
	}

	return (result + fraction) * sign;
}

bool bstring_to_bool(const BString string) {
	if (bstring_equals(string, BSC("true"))) {
		return true;
	} else if (bstring_equals(string, BSC("false"))) {
		return false;
	}

	ls_log(LOG_LEVEL_ERROR, "Invalid boolean string: %S\n", string);
	return false;
}

static char *utf16_to_utf8(const char16 *utf16, uint32 length, uint32 *out_length) {
	LS_ASSERT(utf16);

	// Calculate the length of the UTF-8 string
	uint32 utf8_length = 0;
	for (uint32 i = 0; i < length; i++) {
		if (utf16[i] < 0x80) {
			utf8_length++;
		} else if (utf16[i] < 0x800) {
			utf8_length += 2;
		} else {
			utf8_length += 3;
		}
	}

	char *utf8 = ls_malloc(utf8_length + 1); // +1 for null terminator
	if (!utf8) {
		return NULL;
	}

	uint32 utf8_index = 0;
	for (uint32 i = 0; i < length; i++) {
		if (utf16[i] < 0x80) {
			utf8[utf8_index++] = utf16[i];
		} else if (utf16[i] < 0x800) {
			utf8[utf8_index++] = 0xC0 | (utf16[i] >> 6);
			utf8[utf8_index++] = 0x80 | (utf16[i] & 0x3F);
		} else {
			utf8[utf8_index++] = 0xE0 | (utf16[i] >> 12);
			utf8[utf8_index++] = 0x80 | ((utf16[i] >> 6) & 0x3F);
			utf8[utf8_index++] = 0x80 | (utf16[i] & 0x3F);
		}
	}

	utf8[utf8_index] = 0; // Null terminator
	if (out_length) {
		*out_length = utf8_length;
	}
	return utf8;
}

static char16 *utf8_to_utf16(const char *utf8, uint32 length, uint32 *out_length) {
	LS_ASSERT(utf8);

	// Calculate the length of the UTF-16 string
	uint32 utf16_length = 0;
	for (uint32 i = 0; i < length;) {
		if ((utf8[i] & 0x80) == 0) {
			// 1-byte sequence
			utf16_length++;
			i++;
		} else if ((utf8[i] & 0xE0) == 0xC0) {
			// 2-byte sequence
			utf16_length++;
			i += 2;
		} else if ((utf8[i] & 0xF0) == 0xE0) {
			// 3-byte sequence
			utf16_length++;
			i += 3;
		} else if ((utf8[i] & 0xF8) == 0xF0) {
			// 4-byte sequence
			utf16_length += 2; // Surrogate pair
			i += 4;
		} else {
			// Invalid UTF-8 sequence
			return NULL;
		}
	}

	char16 *utf16 = ls_malloc((utf16_length + 1) * sizeof(char16)); // +1 for null terminator
	if (!utf16) {
		return NULL;
	}

	uint32 utf16_index = 0;
	for (uint32 i = 0; i < length;) {
		if ((utf8[i] & 0x80) == 0) {
			// 1-byte sequence
			utf16[utf16_index++] = utf8[i];
			i++;
		} else if ((utf8[i] & 0xE0) == 0xC0) {
			// 2-byte sequence
			utf16[utf16_index] = ((utf8[i] & 0x1F) << 6) | (utf8[i + 1] & 0x3F);
			utf16_index++;
			i += 2;
		} else if ((utf8[i] & 0xF0) == 0xE0) {
			// 3-byte sequence
			utf16[utf16_index] = ((utf8[i] & 0x0F) << 12) | ((utf8[i + 1] & 0x3F) << 6) | (utf8[i + 2] & 0x3F);
			utf16_index++;
			i += 3;
		} else if ((utf8[i] & 0xF8) == 0xF0) {
			// 4-byte sequence
			uint32 codepoint = ((utf8[i] & 0x07) << 18) | ((utf8[i + 1] & 0x3F) << 12) | ((utf8[i + 2] & 0x3F) << 6) | (utf8[i + 3] & 0x3F);
			codepoint -= 0x10000;
			utf16[utf16_index++] = 0xD800 | (codepoint >> 10);
			utf16[utf16_index++] = 0xDC00 | (codepoint & 0x3FF);
			i += 4;
		} else {
			// Invalid UTF-8 sequence
			ls_free(utf16);
			return NULL;
		}
	}

	utf16[utf16_index] = 0; // Null terminator
	if (out_length) {
		*out_length = utf16_length;
	}

	return utf16;
}

static char *utf32_to_utf8(const char32 *utf32, uint32 length, uint32 *out_length) {
	LS_ASSERT(utf32);

	// Calculate the length of the UTF-8 string
	uint32 utf8_length = 0;
	for (uint32 i = 0; i < length; i++) {
		if (utf32[i] < 0x80) {
			utf8_length++;
		} else if (utf32[i] < 0x800) {
			utf8_length += 2;
		} else if (utf32[i] < 0x10000) {
			utf8_length += 3;
		} else {
			utf8_length += 4;
		}
	}

	char *utf8 = ls_malloc(utf8_length + 1); // +1 for null terminator
	if (!utf8) {
		return NULL;
	}

	uint32 utf8_index = 0;
	for (uint32 i = 0; i < length; i++) {
		if (utf32[i] < 0x80) {
			utf8[utf8_index++] = utf32[i];
		} else if (utf32[i] < 0x800) {
			utf8[utf8_index++] = 0xC0 | (utf32[i] >> 6);
			utf8[utf8_index++] = 0x80 | (utf32[i] & 0x3F);
		} else if (utf32[i] < 0x10000) {
			utf8[utf8_index++] = 0xE0 | (utf32[i] >> 12);
			utf8[utf8_index++] = 0x80 | ((utf32[i] >> 6) & 0x3F);
			utf8[utf8_index++] = 0x80 | (utf32[i] & 0x3F);
		} else {
			utf8[utf8_index++] = 0xF0 | (utf32[i] >> 18);
			utf8[utf8_index++] = 0x80 | ((utf32[i] >> 12) & 0x3F);
			utf8[utf8_index++] = 0x80 | ((utf32[i] >> 6) & 0x3F);
			utf8[utf8_index++] = 0x80 | (utf32[i] & 0x3F);
		}
	}

	utf8[utf8_index] = 0; // Null terminator

	if (out_length) {
		*out_length = utf8_length;
	}

	return utf8;
}

static char32 *utf8_to_utf32(const char *utf8, uint32 length, uint32 *out_length) {
	LS_ASSERT(utf8);

	// Calculate the length of the UTF-32 string
	uint32 utf32_length = 0;
	for (uint32 i = 0; i < length;) {
		if ((utf8[i] & 0x80) == 0) {
			// 1-byte sequence
			utf32_length++;
			i++;
		} else if ((utf8[i] & 0xE0) == 0xC0) {
			// 2-byte sequence
			utf32_length++;
			i += 2;
		} else if ((utf8[i] & 0xF0) == 0xE0) {
			// 3-byte sequence
			utf32_length++;
			i += 3;
		} else if ((utf8[i] & 0xF8) == 0xF0) {
			// 4-byte sequence
			utf32_length++;
			i += 4;
		} else {
			// Invalid UTF-8 sequence
			return NULL;
		}
	}

	char32 *utf32 = ls_malloc((utf32_length + 1) * sizeof(char32)); // +1 for null terminator
	if (!utf32) {
		return NULL;
	}

	uint32 utf32_index = 0;
	for (uint32 i = 0; i < length;) {
		if ((utf8[i] & 0x80) == 0) {
			// 1-byte sequence
			utf32[utf32_index++] = utf8[i];
			i++;
		} else if ((utf8[i] & 0xE0) == 0xC0) {
			// 2-byte sequence
			utf32[utf32_index] = ((utf8[i] & 0x1F) << 6) | (utf8[i + 1] & 0x3F);
			utf32_index++;
			i += 2;
		} else if ((utf8[i] & 0xF0) == 0xE0) {
			// 3-byte sequence
			utf32[utf32_index] = ((utf8[i] & 0x0F) << 12) | ((utf8[i + 1] & 0x3F) << 6) | (utf8[i + 2] & 0x3F);
			utf32_index++;
			i += 3;
		} else if ((utf8[i] & 0xF8) == 0xF0) {
			// 4-byte sequence
			utf32[utf32_index] = ((utf8[i] & 0x07) << 18) | ((utf8[i + 1] & 0x3F) << 12) | ((utf8[i + 2] & 0x3F) << 6) | (utf8[i + 3] & 0x3F);
			utf32_index++;
			i += 4;
		} else {
			// Invalid UTF-8 sequence
			ls_free(utf32);
			return NULL;
		}
	}

	utf32[utf32_index] = 0; // Null terminator
	if (out_length) {
		*out_length = utf32_length;
	}

	return utf32;
}

static uint32 utf16_strlen(const char16 *utf16) {
	LS_ASSERT(utf16);

	uint32 length = 0;
	while (utf16[length]) {
		length++;
	}

	return length;
}

static uint32 utf32_strlen(const char32 *utf32) {
	LS_ASSERT(utf32);

	uint32 length = 0;
	while (utf32[length]) {
		length++;
	}

	return length;
}

BString bstring_formatb_va(const BString format, va_list args) {
	BString formatted = BSTRING_EMPTY;

	uint32 start = 0;
	for (uint32 i = 0; i < format.length; i++) {
		if (bstring_get(format, i) == '%') {
			bstring_append(&formatted, bstring_substring(format, start, i));
			i++;
			if (i > format.length) {
				ls_log(LOG_LEVEL_ERROR, "Invalid format string: %S\n", format);
				return BSTRING_EMPTY;
			}

			switch (bstring_get(format, i)) {
				// Custom specifiers
				case 'S': {
					BString arg = va_arg(args, BString);
					bstring_append(&formatted, arg);
				} break;
				case 'v': {
					i++;
					if (i > format.length) {
						ls_log(LOG_LEVEL_ERROR, "Invalid format string: %S\n", format);
						return BSTRING_EMPTY;
					}

					if (bstring_get(format, i) == '2') {
						i++;
						if (i > format.length) {
							ls_log(LOG_LEVEL_ERROR, "Invalid format string: %S\n", format);
							return BSTRING_EMPTY;
						}

						if (bstring_get(format, i) == 'i') {
							Vector2i arg = va_arg(args, Vector2i);
							BString arg_string = bstring_format("Vector2i(%d, %d)", arg.x, arg.y);
							bstring_append(&formatted, arg_string);
							bstring_unref(arg_string);
						} else if (bstring_get(format, i) == 'u') {
							Vector2u arg = va_arg(args, Vector2u);
							BString arg_string = bstring_format("Vector2u(%u, %u)", arg.x, arg.y);
							bstring_append(&formatted, arg_string);
							bstring_unref(arg_string);
						} else if (bstring_get(format, i) == 'f') {
							Vector2 arg = va_arg(args, Vector2);
							BString arg_string = bstring_format("Vector2(%f, %f)", arg.x, arg.y);
							bstring_append(&formatted, arg_string);
							bstring_unref(arg_string);
						}
					} else if (bstring_get(format, i) == '3') {
						i++;
						if (i > format.length) {
							ls_log(LOG_LEVEL_ERROR, "Invalid format string: %S\n", format);
							return BSTRING_EMPTY;
						}

						if (bstring_get(format, i) == 'i') {
							Vector3i arg = va_arg(args, Vector3i);
							BString arg_string = bstring_format("Vector3i(%d, %d, %d)", arg.x, arg.y, arg.z);
							bstring_append(&formatted, arg_string);
							bstring_unref(arg_string);
						} else if (bstring_get(format, i) == 'u') {
							Vector3u arg = va_arg(args, Vector3u);
							BString arg_string = bstring_format("Vector3u(%u, %u, %u)", arg.x, arg.y, arg.z);
							bstring_append(&formatted, arg_string);
							bstring_unref(arg_string);
						} else if (bstring_get(format, i) == 'f') {
							Vector3 arg = va_arg(args, Vector3);
							BString arg_string = bstring_format("Vector3(%f, %f, %f)", arg.x, arg.y, arg.z);
							bstring_append(&formatted, arg_string);
							bstring_unref(arg_string);
						}

					} else {
						ls_log(LOG_LEVEL_ERROR, "Invalid format string: %S\n", format);
						return BSTRING_EMPTY;
					}

				} break;
				case 'C': {
					Color arg = va_arg(args, Color);
					BString arg_string = bstring_format("Color(%f, %f, %f, %f)", arg.r, arg.g, arg.b, arg.a);
					bstring_append(&formatted, arg_string);
					bstring_unref(arg_string);
				} break;

				case 'O': {
					Object *arg = va_arg(args, Object *);
					BString arg_string = object_to_string(arg);
					bstring_append(&formatted, arg_string);
					bstring_unref(arg_string);
				} break;

				case 'R': {
					Resource *arg = va_arg(args, Resource *);
					BString arg_string = resource_to_string(arg);
					bstring_append(&formatted, arg_string);
					bstring_unref(arg_string);
				} break;

				case 'D': {
					Dictionary *arg = va_arg(args, Dictionary *);
					BString arg_string = dictionary_to_string(arg);
					bstring_append(&formatted, arg_string);
					bstring_unref(arg_string);
				} break;

				case 'A': {
					Array *arg = va_arg(args, Array *);
					BString arg_string = array_to_string(arg);
					bstring_append(&formatted, arg_string);
					bstring_unref(arg_string);
				} break;

				case 'V': {
					Variant arg = va_arg(args, Variant);
					BString arg_string = variant_to_string(arg);
					bstring_append(&formatted, arg_string);
					bstring_unref(arg_string);
				} break;

					// Normal specifiers

				case 'c': {
					char arg = va_arg(args, int);
					bstring_append_char(&formatted, arg);
				} break;

				case 'd': {
					int32 arg = va_arg(args, int32);
					char buffer[32];
					int32 length = 0;
					ls_sprintf(buffer, 32, "%d%n", arg, &length);
					bstring_append_cstr(&formatted, buffer, length);

				} break;

				case 'E': {
					float64 arg = va_arg(args, float64);
					char buffer[32];
					int32 length = 0;
					ls_sprintf(buffer, 32, "%E%n", arg, &length);
					bstring_append_cstr(&formatted, buffer, length);
				} break;

				case 'e': {
					float64 arg = va_arg(args, float64);
					char buffer[32];
					int32 length = 0;
					ls_sprintf(buffer, 32, "%e%n", arg, &length);
					bstring_append_cstr(&formatted, buffer, length);
				} break;

				case 'f': {
					float64 arg = va_arg(args, float64);
					char buffer[32];
					int32 length = 0;
					ls_sprintf(buffer, 32, "%f%n", arg, &length);
					bstring_append_cstr(&formatted, buffer, length);
				} break;

				case 'g': {
					float64 arg = va_arg(args, float64);
					char buffer[32];
					int32 length = 0;
					ls_sprintf(buffer, 32, "%g%n", arg, &length);
					bstring_append_cstr(&formatted, buffer, length);
				} break;

				case 'G': {
					float64 arg = va_arg(args, float64);
					char buffer[32];
					int32 length = 0;
					ls_sprintf(buffer, 32, "%G%n", arg, &length);
					bstring_append_cstr(&formatted, buffer, length);
				} break;

				case 'i': {
					int32 arg = va_arg(args, int32);
					char buffer[32];
					int32 length = 0;
					ls_sprintf(buffer, 32, "%i%n", arg, &length);
					bstring_append_cstr(&formatted, buffer, length);
				} break;

				case 'u': {
					uint32 arg = va_arg(args, uint32);
					char buffer[32];
					int32 length = 0;
					ls_sprintf(buffer, 32, "%u%n", arg, &length);
					bstring_append_cstr(&formatted, buffer, length);
				} break;

				case 'x': {
					uint32 arg = va_arg(args, uint32);
					char buffer[32];
					int32 length = 0;
					ls_sprintf(buffer, 32, "%x%n", arg, &length);
					bstring_append_cstr(&formatted, buffer, length);
				} break;

				case 'X': {
					uint32 arg = va_arg(args, uint32);
					char buffer[32];
					int32 length = 0;
					ls_sprintf(buffer, 32, "%X%n", arg, &length);
					bstring_append_cstr(&formatted, buffer, length);
				} break;

				case 'p': {
					void *arg = va_arg(args, void *);
					char buffer[32];
					int32 length = 0;
					ls_sprintf(buffer, 32, "%p%n", arg, &length);
					bstring_append_cstr(&formatted, buffer, length);
				} break;

				case 's': {
					const char *arg = va_arg(args, const char *);
					bstring_append_cstr(&formatted, arg, ls_str_length(arg));
				} break;

				case 'n': {
					int32 *arg = va_arg(args, int32 *);
					*arg = formatted.length;
				} break;

				case 'l': {
					i++;
					if (i > format.length) {
						ls_log(LOG_LEVEL_ERROR, "Invalid format string: %S\n", format);
						return BSTRING_EMPTY;
					}

					switch (bstring_get(format, i)) {
						case 'i':
						case 'd': {
							int32 arg = va_arg(args, int32);
							char buffer[32];
							int32 length = 0;
							ls_sprintf(buffer, 32, "%ld%n", arg, &length);
							bstring_append_cstr(&formatted, buffer, length);
						} break;

						case 'u': {
							uint32 arg = va_arg(args, uint32);
							char buffer[32];
							int32 length = 0;
							ls_sprintf(buffer, 32, "%lu%n", arg, &length);
							bstring_append_cstr(&formatted, buffer, length);
						} break;

						case 'f': {
							float64 arg = va_arg(args, float64);
							char buffer[32];
							int32 length = 0;
							ls_sprintf(buffer, 32, "%lf%n", arg, &length);
							bstring_append_cstr(&formatted, buffer, length);
						} break;

						case 's': {
							const wchar *arg = va_arg(args, const wchar *);
							BString utf8 = bstring_decode_wchar(arg);
							bstring_append(&formatted, utf8);
							bstring_unref(utf8);
						} break;

						case 'l': {
							i++;
							if (i > format.length) {
								ls_log(LOG_LEVEL_ERROR, "Invalid format string: %S\n", format);
								return BSTRING_EMPTY;
							}

							switch (bstring_get(format, i)) {
								case 'i':
								case 'd': {
									int64 arg = va_arg(args, int64);
									char buffer[32];
									int32 length = 0;
									ls_sprintf(buffer, 32, "%lld%n", arg, &length);
									bstring_append_cstr(&formatted, buffer, length);
								} break;

								case 'u': {
									uint64 arg = va_arg(args, uint64);
									char buffer[32];
									int32 length = 0;
									ls_sprintf(buffer, 32, "%llu%n", arg, &length);
									bstring_append_cstr(&formatted, buffer, length);
								} break;

								default: {
									ls_log(LOG_LEVEL_ERROR, "Invalid format string: %S\n", format);
									return BSTRING_EMPTY;
								} break;
							}
						} break;
					}
				} break;
			}
			i++;

			start = i;
			if (i > format.length) {
				break;
			}
		}
	}

	bstring_append(&formatted, bstring_substring(format, start, format.length));

	return formatted;
}

void bstring_fwrite_va(LSFile file, const BString format, va_list args) {
	uint32 start = 0;

	for (uint32 i = 0; i < format.length; i++) {
		if (bstring_get(format, i) == '%') {
			bstring_fwrite(file, bstring_substring(format, start, i));
			i++;
			if (i > format.length) {
				ls_log(LOG_LEVEL_ERROR, "Invalid format string: %S\n", format);
				return;
			}

			switch (bstring_get(format, i)) {
				// Custom specifiers
				case 'S': {
					BString arg = va_arg(args, BString);
					bstring_fwrite(file, arg);
				} break;

				case 'v': {
					i++;
					if (i > format.length) {
						ls_log(LOG_LEVEL_ERROR, "Invalid format string: %S\n", format);
						return;
					}

					if (bstring_get(format, i) == '2') {
						i++;
						if (i > format.length) {
							ls_log(LOG_LEVEL_ERROR, "Invalid format string: %S\n", format);
							return;
						}

						if (bstring_get(format, i) == 'i') {
							Vector2i arg = va_arg(args, Vector2i);
							bstring_fwritef(file, "Vector2i(%d, %d)", arg.x, arg.y);
						} else if (bstring_get(format, i) == 'u') {
							Vector2u arg = va_arg(args, Vector2u);
							bstring_fwritef(file, "Vector2u(%u, %u)", arg.x, arg.y);
						} else if (bstring_get(format, i) == 'f') {
							Vector2 arg = va_arg(args, Vector2);
							bstring_fwritef(file, "Vector2(%f, %f)", arg.x, arg.y);
						}

					} else if (bstring_get(format, i) == '3') {
						i++;
						if (i > format.length) {
							ls_log(LOG_LEVEL_ERROR, "Invalid format string: %S\n", format);
							return;
						}

						if (bstring_get(format, i) == 'i') {
							Vector3i arg = va_arg(args, Vector3i);
							bstring_fwritef(file, "Vector3i(%d, %d, %d)", arg.x, arg.y, arg.z);
						} else if (bstring_get(format, i) == 'u') {
							Vector3u arg = va_arg(args, Vector3u);
							bstring_fwritef(file, "Vector3u(%u, %u, %u)", arg.x, arg.y, arg.z);
						} else if (bstring_get(format, i) == 'f') {
							Vector3 arg = va_arg(args, Vector3);
							bstring_fwritef(file, "Vector3(%f, %f, %f)", arg.x, arg.y, arg.z);
						}

					} else {
						ls_log(LOG_LEVEL_ERROR, "Invalid format string: %S\n", format);
						return;
					}
				} break;

				case 'C': {
					Color arg = va_arg(args, Color);
					bstring_fwritef(file, "Color(%f, %f, %f, %f)", arg.r, arg.g, arg.b, arg.a);
				} break;

				case 'O': {
					Object *arg = va_arg(args, Object *);
					BString arg_str = object_to_string(arg);
					bstring_fwrite(file, arg_str);
					bstring_unref(arg_str);
				} break;

				case 'R': {
					Resource *arg = va_arg(args, Resource *);
					BString arg_str = resource_to_string(arg);
					bstring_fwrite(file, arg_str);
					bstring_unref(arg_str);
				} break;

				case 'D': {
					Dictionary *arg = va_arg(args, Dictionary *);
					BString arg_str = dictionary_to_string(arg);
					bstring_fwrite(file, arg_str);
					bstring_unref(arg_str);
				} break;

				case 'A': {
					Array *arg = va_arg(args, Array *);
					BString arg_str = array_to_string(arg);
					bstring_fwrite(file, arg_str);
					bstring_unref(arg_str);
				} break;

				case 'V': {
					Variant arg = va_arg(args, Variant);
					BString arg_str = variant_to_string(arg);
					bstring_fwrite(file, arg_str);
					bstring_unref(arg_str);
				} break;

					// Normal specifiers

				case 'c': {
					char arg = va_arg(args, int);
					fputc(arg, file);
				} break;

				case 'd': {
					int32 arg = va_arg(args, int32);
					ls_fprintf(file, "%d", arg);
				} break;

				case 'E': {
					float64 arg = va_arg(args, float64);
					ls_fprintf(file, "%E", arg);
				} break;

				case 'e': {
					float64 arg = va_arg(args, float64);
					ls_fprintf(file, "%e", arg);
				} break;

				case 'f': {
					float64 arg = va_arg(args, float64);
					ls_fprintf(file, "%f", arg);
				} break;

				case 'g': {
					float64 arg = va_arg(args, float64);
					ls_fprintf(file, "%g", arg);
				} break;

				case 'G': {
					float64 arg = va_arg(args, float64);
					ls_fprintf(file, "%G", arg);
				} break;

				case 'i': {
					int32 arg = va_arg(args, int32);
					ls_fprintf(file, "%i", arg);
				} break;

				case 'u': {
					uint32 arg = va_arg(args, uint32);
					ls_fprintf(file, "%u", arg);
				} break;

				case 'x': {
					uint32 arg = va_arg(args, uint32);
					ls_fprintf(file, "%x", arg);
				} break;

				case 'X': {
					uint32 arg = va_arg(args, uint32);
					ls_fprintf(file, "%X", arg);
				} break;

				case 'p': {
					void *arg = va_arg(args, void *);
					ls_fprintf(file, "%p", arg);
				} break;

				case 's': {
					const char *arg = va_arg(args, const char *);
					fputs(arg, file);
				} break;

				case 'n': {
					int32 *arg = va_arg(args, int32 *);
					*arg = ftell(file);
				} break;

				case 'l': {
					i++;
					if (i > format.length) {
						ls_log(LOG_LEVEL_ERROR, "Invalid format string: %S\n", format);
						return;
					}

					switch (bstring_get(format, i)) {
						case 'i':
						case 'd': {
							int32 arg = va_arg(args, int32);
							ls_fprintf(file, "%ld", arg);
						} break;

						case 'u': {
							uint32 arg = va_arg(args, uint32);
							ls_fprintf(file, "%lu", arg);
						} break;

						case 's': {
							const wchar *arg = va_arg(args, const wchar *);
							BString utf8 = bstring_decode_wchar(arg);
							bstring_fwrite(file, utf8);
							bstring_unref(utf8);
						} break;

						case 'f': {
							float64 arg = va_arg(args, float64);
							ls_fprintf(file, "%lf", arg);
						} break;

						case 'l': {
							i++;
							if (i > format.length) {
								ls_log(LOG_LEVEL_ERROR, "Invalid format string: %S\n", format);
								return;
							}

							switch (bstring_get(format, i)) {
								case 'i':
								case 'd': {
									int64 arg = va_arg(args, int64);
									ls_fprintf(file, "%lld", arg);
								} break;

								case 'u': {
									uint64 arg = va_arg(args, uint64);
									ls_fprintf(file, "%llu", arg);
								} break;

								default: {
									ls_log(LOG_LEVEL_ERROR, "Invalid format string: %S\n", format);
									return;
								} break;
							}
						} break;
					}
				} break;
			}
			i++;

			start = i;
			if (i > format.length) {
				break;
			}
		}
	}

	bstring_fwrite(file, bstring_substring(format, start, format.length));
}