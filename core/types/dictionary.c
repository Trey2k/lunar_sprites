#include "core/types/dictionary.h"

#include "core/core.h"
#include "core/types/bstring.h"
#include "core/types/variant.h"

#define DICTIONARY_SHRINK_FACTOR 0.5f
#define DICTIONARY_GROWTH_FACTOR 1.5f
#define DICTIONARY_MIN_LOAD_FACTOR 0.25f
#define DICTIONARY_MAX_LOAD_FACTOR 0.75f
#define DICTIONARY_MAX_COLLISIONS 5

typedef struct DictionaryEntry DictionaryEntry;

struct DictionaryEntry {
	Variant key;
	Variant value;

	DictionaryEntry *next;
};

struct Dictionary {
	size_t size;
	size_t capacity;
	size_t initial_capacity;

	size_t ref_count;

	uint16 collision_count;

	DictionaryEntry *entries;
};

_FORCE_INLINE_ size_t dictionary_index(const Dictionary *dictionary, Variant key) {
	return variant_hash(key) % dictionary->capacity;
}

static void clear_children(DictionaryEntry *entry) {
	variant_unref(entry->key);
	variant_unref(entry->value);

	entry = entry->next;
	while (entry) {
		variant_unref(entry->key);
		variant_unref(entry->value);
		DictionaryEntry *next = entry->next;
		ls_free(entry);
		entry = next;
	}
}

static void dictionary_resize(Dictionary *dictionary, size_t new_capacity) {
	DictionaryEntry *old_entries = dictionary->entries;
	size_t old_capacity = dictionary->capacity;

	dictionary->capacity = new_capacity;
	dictionary->size = 0;
	dictionary->collision_count = 0;
	dictionary->entries = ls_calloc(new_capacity, sizeof(DictionaryEntry));

	for (size_t i = 0; i < old_capacity; i++) {
		DictionaryEntry *entry = &old_entries[i];
		bool is_first = true;
		while (entry) {
			DictionaryEntry *next = entry->next;
			dictionary_set(dictionary, entry->key, entry->value);
			if (!is_first) {
				ls_free(entry);
			}
			entry = next;
			is_first = false;
		}
	}

	ls_free(old_entries);
}

static DictionaryEntry *dictionary_get_entry_create(Dictionary *dictionary, Variant key) {
	const size_t index = dictionary_index(dictionary, key);
	DictionaryEntry *entry = &dictionary->entries[index];

	if (entry->key.type == VARIANT_TYPE_NIL || variant_equals(entry->key, key)) {
		return entry;
	}

	while (entry->next) {
		if (variant_equals(entry->key, key)) {
			return entry;
		}

		entry = entry->next;
	}

	entry->next = ls_calloc(1, sizeof(DictionaryEntry));
	dictionary->collision_count++;
	return entry->next;
}

static DictionaryEntry *dictionary_get_entry(const Dictionary *dictionary, Variant key) {
	const size_t index = dictionary_index(dictionary, key);
	DictionaryEntry *entry = &dictionary->entries[index];

	if (entry->key.type == VARIANT_TYPE_NIL) {
		return NULL;
	}

	while (entry) {
		if (variant_equals(entry->key, key)) {
			return entry;
		}

		entry = entry->next;
	}

	return NULL;
}

Dictionary *dictionary_create(size_t initial_capacity) {
	Dictionary *dictionary = ls_malloc(sizeof(Dictionary));
	dictionary->capacity = initial_capacity;
	dictionary->initial_capacity = initial_capacity;
	dictionary->size = 0;
	dictionary->collision_count = 0;
	dictionary->ref_count = 1;
	dictionary->entries = ls_calloc(initial_capacity, sizeof(DictionaryEntry));

	return dictionary;
}

void dictionary_ref(Dictionary *dictionary) {
	LS_ASSERT(dictionary);

	dictionary->ref_count++;
}

void dictionary_unref(Dictionary *dictionary) {
	LS_ASSERT(dictionary);

	dictionary->ref_count--;

	if (dictionary->ref_count != 0) {
		return;
	}

	for (size_t i = 0; i < dictionary->capacity; i++) {
		DictionaryEntry *entry = &dictionary->entries[i];
		clear_children(entry);
	}

	ls_free(dictionary->entries);
	ls_free(dictionary);
}

void dictionary_set(Dictionary *dictionary, Variant key, Variant value) {
	LS_ASSERT(dictionary);

	if (dictionary->size >= dictionary->capacity * DICTIONARY_MAX_LOAD_FACTOR) {
		dictionary_resize(dictionary, dictionary->capacity * DICTIONARY_GROWTH_FACTOR);
	}

	DictionaryEntry *entry = dictionary_get_entry_create(dictionary, key);

	if (entry->key.type != VARIANT_TYPE_NIL) {
		variant_ref(value);
		variant_unref(entry->value);
		entry->value = value;
		return;
	}

	variant_ref(key);
	variant_ref(value);
	entry->key = key;
	entry->value = value;
	entry->next = NULL;

	dictionary->size++;

	if (dictionary->collision_count > DICTIONARY_MAX_COLLISIONS) {
		dictionary_resize(dictionary, dictionary->capacity * DICTIONARY_GROWTH_FACTOR);
	}
}

bool dictionary_remove(Dictionary *dictionary, Variant key) {
	LS_ASSERT(dictionary);

	const size_t index = dictionary_index(dictionary, key);
	DictionaryEntry *entry = &dictionary->entries[index];

	if (entry->key.type == VARIANT_TYPE_NIL) {
		return false;
	}

	while (entry) {
		if (variant_equals(entry->key, key)) {
			DictionaryEntry *next = entry->next;

			variant_unref(entry->key);
			variant_unref(entry->value);

			dictionary->size--;

			if (next) {
				dictionary->entries[index] = *next;
				ls_free(next);
			}

			if (dictionary->size < dictionary->capacity * DICTIONARY_MIN_LOAD_FACTOR) {
				size_t new_capacity = dictionary->capacity * DICTIONARY_SHRINK_FACTOR;
				if (new_capacity < dictionary->initial_capacity) {
					new_capacity = dictionary->initial_capacity;
				}

				dictionary_resize(dictionary, new_capacity);
			}

			return true;
		}
		entry = entry->next;
	}

	return false;
}

void dictionary_clear(Dictionary *dictionary) {
	LS_ASSERT(dictionary);

	for (size_t i = 0; i < dictionary->capacity; i++) {
		DictionaryEntry *entry = &dictionary->entries[i];
		clear_children(entry);
	}

	dictionary->size = 0;
	dictionary_resize(dictionary, dictionary->initial_capacity);
}

bool dictionary_contains(const Dictionary *dictionary, Variant key) {
	LS_ASSERT(dictionary);

	DictionaryEntry *entry = dictionary_get_entry(dictionary, key);
	return entry && entry->key.type != VARIANT_TYPE_NIL;
}

Variant dictionary_get(const Dictionary *dictionary, Variant key) {
	LS_ASSERT(dictionary);

	DictionaryEntry *entry = dictionary_get_entry(dictionary, key);
	return entry->value;
}

uint64 dictionary_hash(const Dictionary *dictionary) {
	LS_ASSERT(dictionary);

	uint64 hash = 0;
	for (size_t i = 0; i < dictionary->capacity; i++) {
		DictionaryEntry *entry = &dictionary->entries[i];
		while (entry) {
			if (entry->key.type != VARIANT_TYPE_NIL) {
				hash += variant_hash(entry->key);
			}
			entry = entry->next;
		}
	}

	return hash;
}

Array *dictionary_keys(const Dictionary *dictionary) {
	LS_ASSERT(dictionary);

	Array *keys = array_create(dictionary->size);
	for (size_t i = 0; i < dictionary->capacity; i++) {
		DictionaryEntry *entry = &dictionary->entries[i];
		while (entry) {
			if (entry->key.type != VARIANT_TYPE_NIL) {
				array_append(keys, entry->key);
			}
			entry = entry->next;
		}
	}

	return keys;
}

BString dictionary_to_string(const Dictionary *dictionary) {
	LS_ASSERT(dictionary);

	BString str = BSC("Dictionary {");
	Array *keys = dictionary_keys(dictionary);
	size_t size = array_get_size(keys);
	for (size_t i = 0; i < size; i++) {
		Variant key = array_get(keys, i);
		Variant value = dictionary_get(dictionary, key);

		BString line;

		// Avoid infinite recursion
		if (value.type == VARIANT_TYPE_DICTIONARY) {
			if (dictionary == value.DICTIONARY) {
				line = bstring_format("%V = Dictionary(SELF_REFERENCE)", key);
			} else {
				line = dictionary_to_string(value.DICTIONARY);
			}
		} else {
			line = bstring_format("%V = %V", key, value);
		}

		if (i != size - 1) {
			bstring_append(&line, BSC(", "));
		}

		bstring_append(&str, line);
		bstring_unref(line);
	}
	bstring_append(&str, BSC("}"));

	array_unref(keys);

	return str;
}
