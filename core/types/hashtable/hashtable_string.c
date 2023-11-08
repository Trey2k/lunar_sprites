#include "core/types/hashtable/hashtable_string.h"
#include "core/debug.h"
#include "core/log.h"
#include "core/memory.h"

typedef struct HashtableStringEntry {
	char *key;
	void *value;
	struct HashtableStringEntry *next;
} HashtableStringEntry;

typedef struct HashtableString {
	size_t size;
	size_t capacity;
	HashtableStringEntry **entries;
} HashtableString;

static size_t hashtable_string_index(const HashtableString *hashtable, String key) {
	LS_ASSERT(hashtable);
	LS_ASSERT(key);

	return ls_str_hash_djb2(key) % hashtable->capacity;
}

HashtableString *hashtable_string_create(size_t capacity) {
	HashtableString *hashtable = ls_malloc(sizeof(HashtableString));
	hashtable->capacity = capacity;
	hashtable->size = 0;
	hashtable->entries = ls_calloc(capacity, sizeof(HashtableStringEntry *));

	return hashtable;
}

void hashtable_string_destroy(HashtableString *hashtable) {
	for (size_t i = 0; i < hashtable->capacity; i++) {
		HashtableStringEntry *entry = hashtable->entries[i];
		while (entry) {
			HashtableStringEntry *next = entry->next;
			ls_free(entry->key);
			ls_free(entry);
			entry = next;
		}
	}

	ls_free(hashtable->entries);
	ls_free(hashtable);
}

void hashtable_string_set(HashtableString *hashtable, String key, void *value) {
	LS_ASSERT(hashtable);
	LS_ASSERT(key);

	const size_t index = hashtable_string_index(hashtable, key);
	HashtableStringEntry *entry = hashtable->entries[index];
	while (entry) {
		if (ls_str_equals(entry->key, key)) {
			entry->value = value;
			return;
		}
		entry = entry->next;
	}

	entry = ls_malloc(sizeof(HashtableStringEntry));
	entry->key = ls_str_copy(key);
	entry->value = value;
	entry->next = hashtable->entries[index];
	hashtable->entries[index] = entry;
	hashtable->size++;
}

void *hashtable_string_get(const HashtableString *hashtable, String key) {
	LS_ASSERT(hashtable);
	LS_ASSERT(key);

	const size_t index = hashtable_string_index(hashtable, key);
	HashtableStringEntry *entry = hashtable->entries[index];
	while (entry) {
		if (ls_str_equals(entry->key, key)) {
			return entry->value;
		}
		entry = entry->next;
	}

	return NULL;
}

String *hashtable_string_get_keys(const HashtableString *hashtable) {
	LS_ASSERT(hashtable);

	String *keys = ls_calloc(hashtable->size + 1, sizeof(String));
	size_t keyIndex = 0;

	for (size_t i = 0; i < hashtable->capacity; i++) {
		HashtableStringEntry *entry = hashtable->entries[i];
		while (entry) {
			keys[keyIndex++] = ls_str_copy(entry->key);
			entry = entry->next;
		}
	}

	LS_ASSERT(keys[hashtable->size] == NULL);

	return keys;
}

bool hashtable_string_has(const HashtableString *hashtable, String key) {
	return hashtable_string_get(hashtable, key) != NULL;
}

bool hashtable_string_remove(HashtableString *hashtable, String key) {
	LS_ASSERT(hashtable);
	LS_ASSERT(key);

	const size_t index = hashtable_string_index(hashtable, key);
	HashtableStringEntry *entry = hashtable->entries[index];
	while (entry) {
		if (ls_str_equals(entry->key, key)) {
			HashtableStringEntry *next = entry->next;
			ls_free(entry->key);
			ls_free(entry);
			hashtable->entries[index] = next;
			hashtable->size--;
			return true;
		}
		HashtableStringEntry *prev = entry;
		entry = entry->next;
		if (entry && ls_str_equals(entry->key, key)) {
			HashtableStringEntry *next = entry->next;
			ls_free(entry->key);
			ls_free(entry);
			prev->next = next;
			hashtable->size--;
			return true;
		}
	}

	return false;
}