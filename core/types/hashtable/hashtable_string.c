#include "core/core.h"
#include "core/core_log.h"
#include "core/types/hashtable/hashtable.h"

struct HashtableString {
	size_t size;
	size_t capacity;
	struct HashtableStringEntry *entries;
};

struct HashtableStringEntry {
	String key;
	void *value;
};

static size_t hashtable_string_index(const HashtableString *hashtable, const String key) {
	CORE_ASSERT(hashtable);
	CORE_ASSERT(key);

	return string_hash_djb2(key) % hashtable->capacity;
}

HashtableString *hashtable_string_create(size_t capacity) {
	HashtableString *hashtable = core_malloc(sizeof(struct HashtableString));
	hashtable->capacity = capacity;
	hashtable->size = 0;
	hashtable->entries = core_malloc(sizeof(struct HashtableStringEntry) * capacity);
	for (size_t i = 0; i < capacity; i++) {
		hashtable->entries[i].value = NULL;
	}

	return hashtable;
}

void hashtable_string_destroy(HashtableString *hashtable) {
	for (size_t i = 0; i < hashtable->capacity; i++) {
		if (hashtable->entries[i].key) {
			core_free(hashtable->entries[i].key);
		}
	}

	core_free(hashtable->entries);
	core_free(hashtable);
}

void hashtable_string_set(HashtableString *hashtable, const String key, void *value) {
	CORE_ASSERT(hashtable);
	CORE_ASSERT(key);

	const size_t index = hashtable_string_index(hashtable, key);
	struct HashtableStringEntry *entry = &hashtable->entries[index];
	if (entry->key == NULL) {
		entry->key = string_copy(key);
		entry->value = value;
		hashtable->size++;
	} else {
		entry->value = value;
		core_log(LOG_LEVEL_WARNING, "HashtableString: Collision detected for key %s\n", key);
	}
}

void *hashtable_string_get(const HashtableString *hashtable, const String key) {
	CORE_ASSERT(hashtable);
	CORE_ASSERT(key);

	const size_t index = hashtable_string_index(hashtable, key);
	const struct HashtableStringEntry *entry = &hashtable->entries[index];

	return entry->value;
}

bool hashtable_string_contains(const HashtableString *hashtable, const String key) {
	CORE_ASSERT(hashtable);
	CORE_ASSERT(key);

	const size_t index = hashtable_string_index(hashtable, key);
	return hashtable->entries[index].value != NULL;
}

bool hashtable_string_remove(HashtableString *hashtable, const String key) {
	CORE_ASSERT(hashtable);
	CORE_ASSERT(key);

	const size_t index = hashtable_string_index(hashtable, key);
	struct HashtableStringEntry *entry = &hashtable->entries[index];
	if (!entry->key || !string_equals(entry->key, key)) {
		return false;
	}
	core_free(entry->key);
	entry->key = NULL;
	entry->value = NULL;
	hashtable->size--;
	return true;
}