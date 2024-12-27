#ifndef OPENGL_UTILS_H
#define OPENGL_UTILS_H

#include "core/core.h"

static inline void flip_data(uint32 width, uint32 height, uint8 *data) {
	uint32 row_size = width * 4;
	uint8 *row = ls_malloc(row_size);

	for (uint32 i = 0; i < height / 2; i++) {
		uint8 *top = data + i * row_size;
		uint8 *bottom = data + (height - i - 1) * row_size;

		ls_memcpy(row, top, row_size);
		ls_memcpy(top, bottom, row_size);
		ls_memcpy(bottom, row, row_size);
	}

	ls_free(row);
}

#endif // OPENGL_UTILS_H