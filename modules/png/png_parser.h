#ifndef PNG_PARSER_H
#define PNG_PARSER_H

#include "core/core.h"

void parse_png_texture(String path, uint32 *width, uint32 *height, uint8 **data);

#endif // PNG_PARSER_H