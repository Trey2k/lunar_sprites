#ifndef COW_DATA_H
#define COW_DATA_H

//LS_API:UNI_HEADER

#include "core/types/typedefs.h"

#define COWDATA_MAX_SIZE INT64_MAX

// Data allignment
// size_t               uint64              T[]
// ref_count            data_size           data
// REF_COUNT_OFFSET     DATA_SIZE_OFFSET    DATA_OFFSET

#define COW_DATA_REF_COUNT_OFFSET 0
#define COW_DATA_SIZE_OFFSET sizeof(size_t)
#define COW_DATA_DATA_OFFSET (COW_DATA_SIZE_OFFSET + sizeof(uint64))

typedef struct {
	uint8 *data;
} COWData;

uint64 cowdata_get_data_size(const COWData *cow_data);
const void *cowdata_get_data_ptr(const COWData *cow_data);
void *cowdata_get_data_ptrw(COWData *cow_data);

void cowdata_ref(COWData *cow_data);
void cowdata_unref(COWData *cow_data);

void cowdata_resize(COWData *cow_data, size_t size, size_t count);
void cowdata_set(COWData *cow_data, const void *data, size_t size, size_t count);

uint8 cowdata_get_byte(const COWData *cow_data, size_t index);
void cowdata_set_byte(COWData *cow_data, size_t index, uint8 byte);
void cowdata_append_byte(COWData *cow_data, uint8 byte);
void cowdata_append(COWData *cow_data, const void *data, size_t size, size_t count);

#endif // COW_DATA-H