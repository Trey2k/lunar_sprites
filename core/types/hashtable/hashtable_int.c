#include "core/types/hashtable/hashtable.h"

#include "core/core.h"

struct HashtableInt {
	size_t size;
	size_t capacity;
	struct HashtableIntEntry *entries;
};

struct HashtableIntEntry {
	int32 key;
	void *value;
};

static size_t hashtable_int_index(const HashtableInt *hashtable, int32 key) {
	CORE_ASSERT(hashtable);

	return key % hashtable->capacity;
}

HashtableInt *hashtable_int_create(size_t capacity) {
	HashtableInt *hashtable = core_malloc(sizeof(struct HashtableInt));
	hashtable->capacity = capacity;
	hashtable->size = 0;
	hashtable->entries = core_malloc(sizeof(struct HashtableIntEntry) * capacity);
	for (size_t i = 0; i < capacity; i++) {
		hashtable->entries[i].value = NULL;
	}

	return hashtable;
}

void hashtable_int_destroy(HashtableInt *hashtable) {
	for (size_t i = 0; i < hashtable->capacity; i++) {
		if (hashtable->entries[i].value) {
			core_free(hashtable->entries[i].value);
		}
	}

	core_free(hashtable->entries);
	core_free(hashtable);
}

void hashtable_int_set(HashtableInt *hashtable, int32 key, void *value) {
	CORE_ASSERT(hashtable);

	const size_t index = hashtable_int_index(hashtable, key);
	struct HashtableIntEntry *entry = &hashtable->entries[index];
	if (entry->value == NULL) {
		entry->key = key;
		entry->value = value;
		hashtable->size++;
	} else {
		entry->value = value;
		core_log(LOG_LEVEL_WARNING, "HashtableInt: Collision detected for key %d\n", key);
	}
}

void *hashtable_int_get(const HashtableInt *hashtable, int32 key) {
	CORE_ASSERT(hashtable);

	const size_t index = hashtable_int_index(hashtable, key);
	const struct HashtableIntEntry *entry = &hashtable->entries[index];
	if (entry->value == NULL) {
		return NULL;
	}

	return entry->value;
}

bool hashtable_int_contains(const HashtableInt *hashtable, int32 key) {
	CORE_ASSERT(hashtable);

	const size_t index = hashtable_int_index(hashtable, key);
	return hashtable->entries[index].value != NULL;
}

bool hashtable_int_remove(HashtableInt *hashtable, int32 key) {
	CORE_ASSERT(hashtable);

	const size_t index = hashtable_int_index(hashtable, key);
	struct HashtableIntEntry *entry = &hashtable->entries[index];
	if (entry->key != key) {
		return false;
	}

	core_free(entry->value);
	entry->value = NULL;
	hashtable->size--;

	return true;
}