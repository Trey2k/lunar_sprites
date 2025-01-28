#include "core/types/cow_data.h"

#include "core/memory.h"

static size_t *get_ref_count(const COWData *cow_data) {
	return (size_t *)(cow_data->data + REF_COUNT_OFFSET);
}

static uint64 *get_data_size(const COWData *cow_data) {
	return (uint64 *)(cow_data->data + DATA_SIZE_OFFSET);
}

uint64 cowdata_get_data_size(const COWData *cow_data) {
	if (!cow_data->data) {
		return 0;
	}

	return *get_data_size(cow_data);
}

static void copy_on_write(COWData *cow_data) {
	if (!cow_data->data) {
		return;
	}

	size_t *ref_count = get_ref_count(cow_data);

	if (*ref_count == 1) {
		return;
	}

	uint64 data_size = *get_data_size(cow_data);
	uint8 *new_data = ls_malloc(data_size + DATA_OFFSET);
	ls_memcpy(new_data, cow_data->data, data_size + DATA_OFFSET);
	(*ref_count)--;
	*get_ref_count(cow_data) = 1;

	cow_data->data = new_data;
}

void *cowdata_get_data_ptrw(COWData *cow_data) {
	if (!cow_data->data) {
		return NULL;
	}

	copy_on_write(cow_data);
	return (void *)(cow_data->data + DATA_OFFSET);
}

const void *cowdata_get_data_ptr(const COWData *cow_data) {
	if (!cow_data->data) {
		return NULL;
	}
	return (const void *)(cow_data->data + DATA_OFFSET);
}

void cowdata_ref(COWData *cow_data) {
	if (!cow_data->data) {
		return;
	}

	(*get_ref_count(cow_data))++;
}

void cowdata_unref(COWData *cow_data) {
	if (!cow_data->data) {
		return;
	}

	size_t *ref_count = get_ref_count(cow_data);
	(*ref_count)--;

	if (*ref_count == 0) {
		ls_free(cow_data->data);
		cow_data->data = NULL;
	}
}

void cowdata_resize(COWData *cow_data, size_t size, size_t count) {
	if (!cow_data->data) {
		cow_data->data = ls_malloc(size * count + DATA_OFFSET);
		*get_ref_count(cow_data) = 1;
		*get_data_size(cow_data) = size * count;
		return;
	}

	copy_on_write(cow_data);

	uint64 *data_size = get_data_size(cow_data);
	*data_size = size * count;

	cow_data->data = ls_realloc(cow_data->data, size * count + DATA_OFFSET);
}

void cowdata_set(COWData *cow_data, const void *data, size_t size, size_t count) {
	cowdata_resize(cow_data, size, count);

	ls_memcpy(cow_data->data + DATA_OFFSET, data, size * count);
}

uint8 cowdata_get_byte(const COWData *cow_data, size_t index) {
	if (!cow_data->data) {
		return 0;
	}
	LS_ASSERT(index < cowdata_get_data_size(cow_data));

	return cow_data->data[DATA_OFFSET + index];
}

void cowdata_set_byte(COWData *cow_data, size_t index, uint8 byte) {
	if (!cow_data->data) {
		return;
	}
	LS_ASSERT(index < cowdata_get_data_size(cow_data));

	copy_on_write(cow_data);
	cow_data->data[DATA_OFFSET + index] = byte;
}

void cowdata_append_byte(COWData *cow_data, uint8 byte) {
	uint64 data_size = cowdata_get_data_size(cow_data);
	cowdata_resize(cow_data, 1, cowdata_get_data_size(cow_data) + 1);

	cow_data->data[DATA_OFFSET + data_size - 1] = byte;
}

void cowdata_append(COWData *cow_data, const void *data, size_t size, size_t count) {
	uint64 cur_data_size = cowdata_get_data_size(cow_data);
	uint64 new_size = cur_data_size + size * count;

	cowdata_resize(cow_data, 1, new_size);

	void *dest = cowdata_get_data_ptrw(cow_data);
	ls_memcpy(dest + cur_data_size, data, size * count);
}