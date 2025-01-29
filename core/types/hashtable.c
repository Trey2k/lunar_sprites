#include "core/types/hashtable.h"

#include "core/debug.h"
#include "core/log.h"
#include "core/memory.h"

// https://en.wikipedia.org/wiki/Universal_hashing#Hashing_integers
#define GOLDEN_RATIO_HASH 0x9E3779B9

typedef struct HashtableEntry {
	HashtableKey key;
	HashtableValue value;

	// Used for when collisions occur
	struct HashtableEntry *next;
} HashtableEntry;

struct Hashtable {
	size_t size;
	size_t capacity;
	size_t initial_size;

	int32 collision_count;
	bool should_free;

	HashtableKeyType key_type;

	HashtableEntry *entries;
};

static size_t hashtable_index(const Hashtable *hashtable, HashtableKey key) {
	LS_ASSERT(hashtable);

	switch (hashtable->key_type) {
		case HASHTABLE_KEY_INT32:
			return (size_t)key.i32 * GOLDEN_RATIO_HASH % hashtable->capacity;
		case HASHTABLE_KEY_UINT32:
			return (size_t)key.u32 * GOLDEN_RATIO_HASH % hashtable->capacity;
		case HASHTABLE_KEY_FLOAT32:
			return (size_t)key.f32 * GOLDEN_RATIO_HASH % hashtable->capacity;
		case HASHTABLE_KEY_INT64:
			return (size_t)key.i64 * GOLDEN_RATIO_HASH % hashtable->capacity;
		case HASHTABLE_KEY_UINT64:
			return (size_t)key.u64 * GOLDEN_RATIO_HASH % hashtable->capacity;
		case HASHTABLE_KEY_FLOAT64:
			return (size_t)key.f64 * GOLDEN_RATIO_HASH % hashtable->capacity;
		case HASHTABLE_KEY_STRING:
			return ls_str_hash_djb2(key.str) % hashtable->capacity;
		default:
			ls_log_fatal("Unknown hashtable key type: %d\n", hashtable->key_type);
			return 0;
	}
}

static void hashtable_resize(Hashtable *hashtable, size_t new_capacity) {
	LS_ASSERT(hashtable);

	HashtableEntry *old_entries = hashtable->entries;
	size_t old_capacity = hashtable->capacity;

	hashtable->capacity = new_capacity;
	hashtable->size = 0;
	hashtable->collision_count = 0;
	hashtable->entries = ls_calloc(new_capacity, sizeof(HashtableEntry));

	for (size_t i = 0; i < old_capacity; i++) {
		HashtableEntry *entry = &old_entries[i];
		while (entry) {
			if (entry->value.ptr) {
				hashtable_set(hashtable, entry->key, entry->value);
			}
			entry = entry->next;
		}
	}

	ls_free(old_entries);
}

static void clear_childern(HashtableEntry *entry, bool should_free) {
	HashtableEntry *next = entry->next;
	while (next) {
		HashtableEntry *temp = next->next;
		if (should_free && next->value.ptr) {
			ls_free(next->value.ptr);
		}
		ls_free(next);
		next = temp;
	}
}

static bool keys_match(HashtableKeyType key_type, HashtableKey a, HashtableKey b) {
	switch (key_type) {
		case HASHTABLE_KEY_INT32:
			return a.i32 == b.i32;
		case HASHTABLE_KEY_UINT32:
			return a.u32 == b.u32;
		case HASHTABLE_KEY_FLOAT32:
			return a.f32 == b.f32;
		case HASHTABLE_KEY_INT64:
			return a.i64 == b.i64;
		case HASHTABLE_KEY_UINT64:
			return a.u64 == b.u64;
		case HASHTABLE_KEY_FLOAT64:
			return a.f64 == b.f64;
		case HASHTABLE_KEY_STRING:
			return ls_str_equals(a.str, b.str);
		default:
			ls_log_fatal("Unknown hashtable key type: %d\n", key_type);
			return false;
	}
}

// Will always return a valid entry. If the entry does not exist, it will be created.
static HashtableEntry *hashtable_get_entry_create(Hashtable *hashtable, HashtableKey key) {
	LS_ASSERT(hashtable);

	const size_t index = hashtable_index(hashtable, key);
	HashtableEntry *entry = &hashtable->entries[index];

	if (!entry->key.u64 || keys_match(hashtable->key_type, entry->key, key)) {
		return entry;
	}

	while (entry->next) {
		if (keys_match(hashtable->key_type, entry->key, key)) {
			return entry;
		}
		entry = entry->next;
	}

	entry->next = ls_calloc(1, sizeof(HashtableEntry));
	entry = entry->next;
	hashtable->collision_count++;

	return entry;
}

static HashtableEntry *hashtable_get_entry(const Hashtable *hashtable, HashtableKey key) {
	LS_ASSERT(hashtable);

	const size_t index = hashtable_index(hashtable, key);
	HashtableEntry *entry = &hashtable->entries[index];
	if (!entry->key.u64) {
		return NULL;
	}

	while (entry) {
		if (keys_match(hashtable->key_type, entry->key, key)) {
			return entry;
		}
		entry = entry->next;
	}

	return NULL;
}

Hashtable *hashtable_create(HashtableKeyType key_type, size_t initial_size, bool should_free) {
	Hashtable *hashtable = ls_malloc(sizeof(Hashtable));
	hashtable->key_type = key_type;
	hashtable->capacity = initial_size;
	hashtable->initial_size = initial_size;
	hashtable->size = 0;
	hashtable->collision_count = 0;
	hashtable->should_free = should_free;
	hashtable->entries = ls_calloc(initial_size, sizeof(HashtableEntry));

	return hashtable;
}

void hashtable_destroy(Hashtable *hashtable) {
	LS_ASSERT(hashtable);

	for (size_t i = 0; i < hashtable->capacity; i++) {
		HashtableEntry *entry = &hashtable->entries[i];
		clear_childern(entry, hashtable->should_free);
		if (hashtable->should_free && entry->value.ptr) {
			ls_free(entry->value.ptr);
		}
	}

	ls_free(hashtable->entries);
	ls_free(hashtable);
}

void hashtable_set(Hashtable *hashtable, HashtableKey key, HashtableValue value) {
	LS_ASSERT(hashtable);

	if (hashtable->size >= hashtable->capacity * 0.75f) {
		hashtable_resize(hashtable, hashtable->capacity * 2);
	}

	HashtableEntry *entry = hashtable_get_entry_create(hashtable, key);

	if (entry->key.u64) {
		entry->value = value;
		return;
	}

	entry->key = key;
	entry->value = value;
	entry->next = NULL;

	hashtable->size++;
}

void hashtable_clear(Hashtable *hashtable) {
	LS_ASSERT(hashtable);

	for (size_t i = 0; i < hashtable->capacity; i++) {
		HashtableEntry *entry = &hashtable->entries[i];
		clear_childern(entry, hashtable->should_free);

		if (hashtable->should_free && entry->value.ptr) {
			ls_free(entry->value.ptr);
		}

		entry->key.u64 = 0;
		entry->value.ptr = NULL;
		entry->next = NULL;
	}

	hashtable->size = 0;

	hashtable_resize(hashtable, hashtable->initial_size);
}

bool hashtable_remove(Hashtable *hashtable, HashtableKey key) {
	LS_ASSERT(hashtable);

	const size_t index = hashtable_index(hashtable, key);
	HashtableEntry *entry = &hashtable->entries[index];
	if (!entry->key.u64) {
		return false;
	}

	while (entry) {
		if (keys_match(hashtable->key_type, entry->key, key)) {
			HashtableEntry *next = entry->next;

			if (hashtable->should_free && entry->value.ptr) {
				ls_free(entry->value.ptr);
			}

			hashtable->size--;

			if (next) {
				hashtable->entries[index] = *next;
				ls_free(next);
			}

			if (hashtable->size < hashtable->capacity * 0.25f) {
				hashtable_resize(hashtable, hashtable->capacity / 2);
			}

			return true;
		}
		entry = entry->next;
	}

	return false;
}

bool hashtable_contains(const Hashtable *hashtable, HashtableKey key) {
	LS_ASSERT(hashtable);

	HashtableEntry *entry = hashtable_get_entry(hashtable, key);
	return entry && entry->key.u64;
}

HashtableValue hashtable_get(const Hashtable *hashtable, HashtableKey key) {
	LS_ASSERT(hashtable);

	HashtableEntry *entry = hashtable_get_entry(hashtable, key);
	if (!entry) {
		return HASH_VAL(u64, 0);
	}

	return entry->value;
}

size_t hashtable_get_size(const Hashtable *hashtable) {
	LS_ASSERT(hashtable);

	return hashtable->size;
}

size_t hashtable_get_capacity(const Hashtable *hashtable) {
	LS_ASSERT(hashtable);

	return hashtable->capacity;
}

int32 hashtable_get_collisions(const Hashtable *hashtable) {
	LS_ASSERT(hashtable);

	return hashtable->collision_count;
}

Slice64 *hashtable_get_keys(const Hashtable *table) {
	LS_ASSERT(table);

	Slice64 *slice = slice64_create(table->size, false);

	for (size_t i = 0; i < table->capacity; i++) {
		HashtableEntry *entry = &table->entries[i];
		while (entry) {
			if (entry->key.u64) {
				slice64_append(slice, SLICE_VAL64(ptr, &entry->key));
			}
			entry = entry->next;
		}
	}

	return slice;
}