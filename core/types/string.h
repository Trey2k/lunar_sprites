#ifndef STRING_H
#define STRING_H

#include "core/typedefs.h"

typedef const char *String;

size_t string_length(String string);

size_t string_hash_djb2(String string);

bool string_is_empty(String string);
bool string_contains(String string, String substring);
bool string_equals(String a, String b);

char *string_copy(String string);
char *string_concat(String a, ...);
char *string_format(String format, ...);

String string_char(String str, int32 c);
String string_find(String str, String substr);

#endif // STRING_H