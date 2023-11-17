#ifndef HASHTABLE_H
#define HASHTABLE_H

#include "core/types/string.h"
#include "core/types/typedefs.h"

#define HASH_KEY(type, val) \
	(HashtableKey) { .type = val }
#define HASH_VAL(type, val) \
	(HashtableValue) { .type = val }

typedef enum {
	HASHTABLE_KEY_INT32,
	HASHTABLE_KEY_UINT32,
	HASHTABLE_KEY_FLOAT32,
	HASHTABLE_KEY_STRING
} HashtableKeyType;

typedef union {
	int32 i32;
	uint32 u32;
	float32 f32;
	String str;
} HashtableKey;

typedef union {
	int32 i32;
	uint32 u32;
	float32 f32;
	String str;
	void *ptr;
} HashtableValue;

typedef struct Hashtable Hashtable;

// key_type must always be the same for a given hash table.
// initial_size is the initial number of elements the hash table can hold.
// should_free determines whether the hash table should free the value. Assumes value is a pointer.
LS_EXPORT Hashtable *hashtable_create(HashtableKeyType key_type, size_t initial_size, bool should_free);
LS_EXPORT void hashtable_destroy(Hashtable *table);

LS_EXPORT void hashtable_set(Hashtable *table, HashtableKey key, HashtableValue value);
LS_EXPORT void hashtable_clear(Hashtable *table);

LS_EXPORT bool hashtable_remove(Hashtable *table, HashtableKey key);
LS_EXPORT bool hashtable_contains(const Hashtable *table, HashtableKey key);

LS_EXPORT HashtableValue hashtable_get(const Hashtable *table, HashtableKey key);

LS_EXPORT size_t hashtable_get_size(const Hashtable *table);
LS_EXPORT size_t hashtable_get_capacity(const Hashtable *table);

LS_EXPORT int32 hashtable_get_collisions(const Hashtable *table);

#endif // HASHTABLE_H