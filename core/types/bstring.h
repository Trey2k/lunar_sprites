#ifndef BSTRING_H
#define BSTRING_H

#include "core/api.h"
#include "core/types/string.h"
#include "core/types/typedefs.h"

#include "core/types/cow_data.h"

// This will replace the use of C strings in the engine.
// BStrings are UTF-8 backed bounded strings.
typedef struct {
	bool is_const;
	uint32 length;
	union {
		const char *c_const;
		COWData cow_data;
	};
} BString;

// Creates a BString which does not own the string.
// The string must be valid for the lifetime of the BString, unless the BString is modified, in which case the string will be copied.
#define BSTRING_CONST(a_str) \
	(BString) { .c_const = a_str, .length = ls_str_length(a_str), .is_const = true }
// Creates a BString which does not own the string.
// The string must be valid for the lifetime of the BString, unless the BString is modified, in which case the string will be copied.
#define BSTRING_CONST_LENGTH(a_str, a_length) \
	(BString) { .c_const = a_str, .length = a_length, .is_const = true }

// Short hand alias for BSTRING_CONST.
#define BSC(a_str) BSTRING_CONST(a_str)
// const BString Variant.
#define BSCV(a_str) VARIANT_STRING(BSTRING_CONST(a_str))
// Compile time constant BString variant.
#define BSCVL(a_str, a_length) VARIANT_STRING(BSTRING_CONST_LENGTH(a_str, a_length))

// Creates a BString copy of the string.
#define BSTRING(a_str) bstring_encode_utf8(BSTRING_CONST(a_str))
// Creates a new empty BString.
#define BSTRING_EMPTY \
	(BString) { .c_const = NULL, .length = 0, .is_const = true }

#define BSTRING_IS_EMPTY(a_str) (a_str.length == 0)

// Creates a copy of the BString.
LS_EXPORT BString bstring_copy(const BString string);
// Creates a sub-string of the BString which directly references the original string.
LS_EXPORT BString bstring_substring(const BString string, uint32 start, uint32 end);
// Creates a djb2 hash of the string.
LS_EXPORT uint64 bstring_hash(const BString string);

LS_EXPORT void bstring_ref(BString str);
LS_EXPORT void bstring_unref(BString str);

LS_EXPORT char bstring_get(const BString string, uint32 index);

LS_EXPORT void bstring_append(BString *string, const BString append);
LS_EXPORT void bstring_append_cstr(BString *string, const char *append, uint32 length);
LS_EXPORT void bstring_append_char(BString *string, char append);

LS_EXPORT void bstring_insert(BString *string, const BString insert, uint32 index);
LS_EXPORT void bstring_insert_cstr(BString *string, const char *insert, uint32 index);
LS_EXPORT void bstring_insert_char(BString *string, char insert, uint32 index);

LS_EXPORT void bstring_remove(BString *string, uint32 start, uint32 end);

// Creates a BString from a C format string.
// Supports the normal printf format specifiers except for %A, plus the following:
// %S - BString
// %v2i - Vector2i
// %v2u - Vector2u
// %v2 - Vector2
// %v3i - Vector3i
// %v3u - Vector3u
// %v3 - Vector3
// %C - Color
// %O - Object
// %R - Resource
// %D - Dictionary
// %A - Array - Replaced normal C specifier %A, %a can be used instead.
// %V - Variant
LS_EXPORT BString bstring_format(const char *format, ...);
// Same as bstring_format but takes a va_list.
LS_EXPORT BString bstring_format_va(const char *format, va_list args);
// Same as bstring_format but takes a BString for the format.
LS_EXPORT BString bstring_formatb(const BString format, ...);
// Same as bstring_formatb but takes a va_list.
LS_EXPORT BString bstring_formatb_va(const BString format, va_list args);

// Writes the BString to a file.
LS_EXPORT void bstring_fwrite(LSFile file, const BString string);
// Writes the BString to a file using a format string.
// When writing out a format string rather than to another BString we can avoid doing any memory allocations.
LS_EXPORT void bstring_fwritef(LSFile file, const char *format, ...);
LS_EXPORT void bstring_fwritefb(LSFile file, const BString format, ...);
LS_EXPORT void bstring_fwrite_va(LSFile file, const BString format, va_list args);

LS_EXPORT BString bstring_read_file(BString path);

// Encodes a ASCII copy of the string.
LS_EXPORT char *bstring_encode_ascii(const BString string);
// Creates a BString from a ASCII encoded string.
LS_EXPORT BString bstring_decode_ascii(const char *string);

// Encodes a UTF-8 copy of the string.
LS_EXPORT char *bstring_encode_utf8(const BString string);
// Creates a BString from a UTF-8 encoded string.
LS_EXPORT BString bstring_decode_utf8(const char *string);

// Encodes a UTF-16 copy of the string.
LS_EXPORT char16 *bstring_encode_utf16(const BString string);
// Creates a BString from a UTF-16 encoded string.
LS_EXPORT BString bstring_decode_utf16(const char16 *string);

// Encodes a UTF-32 copy of the string.
LS_EXPORT char32 *bstring_encode_utf32(const BString string);
// Creates a BString from a UTF-32 encoded string.
LS_EXPORT BString bstring_decode_utf32(const char32 *string);

// Encodes a wchar copy of the string.
// The size of wchar is platform dependent.
#if defined(WCHAR_IS_CHAR32)
#define bstring_encode_wchar (wchar *)bstring_encode_utf32
#define bstring_decode_wchar(arg) bstring_decode_utf32((char32 *)arg)
#elif defined(WCHAR_IS_CHAR16)
#define bstring_encode_wchar (wchar *)bstring_encode_utf16
#define bstring_decode_wchar(arg) bstring_decode_utf16((char16 *)arg)
#else
#error "Unknown wchar size?????"
#endif // WCHAR_IS_CHAR32

LS_EXPORT bool bstring_equals(const BString a, const BString b);
LS_EXPORT bool bstring_starts_with(const BString string, const BString substring);
LS_EXPORT bool bstring_ends_with(const BString string, const BString substring);
LS_EXPORT bool bstring_contains(const BString string, const BString substring);
// Returns true if the string is a number.
LS_EXPORT bool bstring_is_number(const BString string);

// Parses an integer from the string.
LS_EXPORT int64 bstring_to_int(const BString string);
// Parses a float from the string.
LS_EXPORT float64 bstring_to_float(const BString string);
// Parses a boolean from the string.
LS_EXPORT bool bstring_to_bool(const BString string);

#if defined(DEBUG_ENABLED)
void bstring_tracker_init();
void bstring_tracker_deinit();
#endif // DEBUG_ENABLED

#endif // BSTRING_H