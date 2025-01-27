#ifndef DICTIONARY_H
#define DICTIONARY_H

#include "core/types/array.h"
#include "core/types/bstring.h"
#include "core/types/typedefs.h"

#include "core/api.h"

typedef struct Dictionary Dictionary;

LS_EXPORT Dictionary *dictionary_create(size_t initial_capacity);
// Increments the reference count of the dictionary.
LS_EXPORT void dictionary_ref(Dictionary *dictionary);
// Decrements the reference count of the dictionary. If the reference count reaches 0, the dictionary is destroyed.
LS_EXPORT void dictionary_unref(Dictionary *dictionary);

LS_EXPORT void dictionary_set(Dictionary *dictionary, Variant key, Variant value);
LS_EXPORT bool dictionary_remove(Dictionary *dictionary, Variant key);
LS_EXPORT void dictionary_clear(Dictionary *dictionary);

LS_EXPORT Variant dictionary_get(const Dictionary *dictionary, Variant key);
LS_EXPORT bool dictionary_contains(const Dictionary *dictionary, Variant key);

LS_EXPORT uint64 dictionary_hash(const Dictionary *dictionary);

LS_EXPORT Array *dictionary_keys(const Dictionary *dictionary);

LS_EXPORT BString dictionary_to_string(const Dictionary *dictionary);

#endif // DICTIONARY_H