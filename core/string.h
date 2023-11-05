#ifndef STRING_H
#define STRING_H

#include "core/typedefs.h"

typedef char *String;

size_t string_length(const String string);

size_t string_hash_djb2(const String string);

bool string_is_empty(const String string);
bool string_contains(const String string, const String substring);
bool string_equals(const String a, const String b);

String string_copy(const String string);
String string_concat(const String a, ...);
String string_format(const String format, ...);

#endif // STRING_H