#ifndef UI_ELEMENT_IMPL_H
#define UI_ELEMENT_IMPL_H

#include "../elements.h"

#include "label.h"

typedef enum {
	UI_ELEMENT_TYPE_NONE,
	UI_ELEMENT_TYPE_LABEL,
} UIElementType;

struct UIElement {
	UIElementType type;
	union {
		UILabel label;
	};

	Vector2u position;
	Vector2u size;

	uint32 anchors;
};

#endif // UI_ELEMENT_IMPL_H