#ifndef HASHTABLE_STRING_H
#define HASHTABLE_STRING_H

#include "core/types/string/string.h"
#include "core/types/typedefs.h"

typedef struct HashtableString HashtableString;

HashtableString *hashtable_string_create(size_t capacity);
void hashtable_string_destroy(HashtableString *hashtable);

void hashtable_string_set(HashtableString *hashtable, String key, void *data);
void *hashtable_string_get(const HashtableString *hashtable, String key);
bool hashtable_string_has(const HashtableString *hashtable, String key);
bool hashtable_string_remove(HashtableString *hashtable, String key);

#endif // HASHTABLE_STRING_H