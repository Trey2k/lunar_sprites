#ifndef STRING_H
#define STRING_H

#include "core/api.h"

#include "core/types/typedefs.h"

typedef const char *String;

LS_EXPORT size_t ls_str_length(String string);
LS_EXPORT size_t ls_str_hash_djb2(String string);

LS_EXPORT bool ls_str_is_empty(String string);
LS_EXPORT bool ls_str_contains(String string, String substring);
LS_EXPORT bool ls_str_equals(String a, String b);
LS_EXPORT bool ls_str_ends_with(String string, String substring);
LS_EXPORT bool ls_str_starts_with(String string, String substring);

LS_EXPORT char *ls_str_copy(String string);
LS_EXPORT void ls_str_copy_to(char *buffer, String string, size_t buffer_size);
LS_EXPORT char *ls_str_concat(String a, ...);
LS_EXPORT char *ls_str_format(String format, ...);
LS_EXPORT char *ls_str_replace(String string, String old, String new);

LS_EXPORT char *ls_str_token(String string, String delimiters, String *context);
LS_EXPORT char *ls_str_token_r(String string, String delimiters, char **context);

LS_EXPORT String ls_str_char(String str, int32 c);
LS_EXPORT String ls_str_find(String str, String substr);

LS_EXPORT bool ls_str_to_int32(String string, int32 *out_value);
LS_EXPORT bool ls_str_to_float32(String string, float32 *out_value);
LS_EXPORT bool ls_str_to_bool(String string, bool *out_value);

LS_EXPORT void ls_str_to_lower(char *string);
LS_EXPORT void ls_str_to_upper(char *string);

#endif // STRING_H