#ifndef STRING_H
#define STRING_H

#include "core/types/typedefs.h"

typedef const char *String;

size_t ls_str_length(String string);
size_t ls_str_hash_djb2(String string);

bool ls_str_is_empty(String string);
bool ls_str_contains(String string, String substring);
bool ls_str_equals(String a, String b);

char *ls_str_copy(String string);
char *ls_str_concat(String a, ...);
char *ls_str_format(String format, ...);

String ls_str_char(String str, int32 c);
String ls_str_find(String str, String substr);

bool ls_str_to_int32(String string, int32 *out_value);
bool ls_str_to_float32(String string, float32 *out_value);
bool ls_str_to_bool(String string, bool *out_value);

void ls_str_to_lower(char *string);
void ls_str_to_upper(char *string);

#endif // STRING_H