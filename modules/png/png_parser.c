#include "png_parser.h"

#include <spng.h>

void parse_png_texture(String path, uint32 *width, uint32 *height, uint8 **data) {
	spng_ctx *ctx = spng_ctx_new(0);
	if (ctx == NULL) {
		ls_log(LOG_LEVEL_ERROR, "Failed to create spng context\n");
		return;
	}

	size_t f_size = 0;
	char *f_data = os_read_file(path, &f_size);
	if (f_data == NULL) {
		ls_log(LOG_LEVEL_ERROR, "Failed to read file %s\n", path);
		return;
	}

	spng_set_png_buffer(ctx, f_data, f_size);

	size_t png_size = 0;

	spng_decoded_image_size(ctx, SPNG_FMT_RGBA8, &png_size);

	uint8 *png_data = ls_malloc(png_size);

	spng_decode_image(ctx, png_data, png_size, SPNG_FMT_RGBA8, 0);

	struct spng_ihdr ihdr;
	spng_get_ihdr(ctx, &ihdr);

	spng_ctx_free(ctx);
	ls_free(f_data);

	*width = ihdr.width;
	*height = ihdr.height;
	*data = png_data;
}