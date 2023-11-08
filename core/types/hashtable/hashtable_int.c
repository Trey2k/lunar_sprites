#include "core/debug.h"
#include "core/log.h"
#include "core/memory.h"
#include "core/types/hashtable/hashtable_string.h"

typedef struct HashtableIntEntry {
	int32 key;
	void *value;
	struct HashtableIntEntry *next;
} HashtableIntEntry;

typedef struct HashtableInt {
	size_t size;
	size_t capacity;
	HashtableIntEntry **entries;
} HashtableInt;

static size_t hashtable_int_index(const HashtableInt *hashtable, int32 key) {
	LS_ASSERT(hashtable);
	LS_ASSERT(key);

	return key % hashtable->capacity;
}

HashtableInt *hashtable_int_create(size_t capacity) {
	HashtableInt *hashtable = ls_malloc(sizeof(HashtableInt));
	hashtable->capacity = capacity;
	hashtable->size = 0;
	hashtable->entries = ls_calloc(capacity, sizeof(HashtableIntEntry *));

	return hashtable;
}

void hashtable_int_destroy(HashtableInt *hashtable) {
	for (size_t i = 0; i < hashtable->capacity; i++) {
		HashtableIntEntry *entry = hashtable->entries[i];
		while (entry) {
			HashtableIntEntry *next = entry->next;
			ls_free(entry);
			entry = next;
		}
	}

	ls_free(hashtable->entries);
	ls_free(hashtable);
}

void hashtable_int_set(HashtableInt *hashtable, int32 key, void *value) {
	LS_ASSERT(hashtable);
	LS_ASSERT(key);

	const size_t index = hashtable_int_index(hashtable, key);
	HashtableIntEntry *entry = hashtable->entries[index];
	while (entry) {
		if (entry->key == key) {
			entry->value = value;
			return;
		}
		entry = entry->next;
	}

	entry = ls_malloc(sizeof(HashtableIntEntry));
	entry->key = key;
	entry->value = value;
	entry->next = hashtable->entries[index];
	hashtable->entries[index] = entry;
	hashtable->size++;
}

void *hashtable_int_get(const HashtableInt *hashtable, int32 key) {
	LS_ASSERT(hashtable);
	LS_ASSERT(key);

	const size_t index = hashtable_int_index(hashtable, key);
	HashtableIntEntry *entry = hashtable->entries[index];
	while (entry) {
		if (entry->key == key) {
			return entry->value;
		}
		entry = entry->next;
	}

	return NULL;
}

bool hashtable_int_has(const HashtableInt *hashtable, int32 key) {
	return hashtable_int_get(hashtable, key) != NULL;
}

bool hashtable_int_remove(HashtableInt *hashtable, int32 key) {
	LS_ASSERT(hashtable);
	LS_ASSERT(key);

	const size_t index = hashtable_int_index(hashtable, key);
	HashtableIntEntry *entry = hashtable->entries[index];
	while (entry) {
		if (entry->key == key) {
			HashtableIntEntry *next = entry->next;
			ls_free(entry);
			hashtable->entries[index] = next;
			hashtable->size--;
			return true;
		}
		HashtableIntEntry *prev = entry;
		entry = entry->next;
		if (entry && entry->key == key) {
			HashtableIntEntry *next = entry->next;
			ls_free(entry);
			prev->next = next;
			hashtable->size--;
			return true;
		}
	}

	return false;
}