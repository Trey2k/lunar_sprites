#ifndef UI_ELEMENT_IMPL_H
#define UI_ELEMENT_IMPL_H

#include "modules/ui/elements.h"

#include "button.h"
#include "horizontal_container.h"
#include "label.h"
#include "vertical_container.h"

typedef enum {
	UI_ELEMENT_TYPE_NONE,
	UI_ELEMENT_TYPE_LABEL,
	UI_ELEMENT_TYPE_BUTTON,
	UI_ELEMENT_TYPE_VERTICAL_CONTAINER,
	UI_ELEMENT_TYPE_HORIZONTAL_CONTAINER,
} UIElementType;

struct UIElement {
	UIElementType type;
	union {
		UILabel label;
		UIVerticalContainer vertical_container;
		UIHorizontalContainer horizontal_container;
		UIButton button;
	};

	Vector2u position;
	Vector2u size;

	Vector2u min_size;
	Vector2u max_size;

	UILayout layout;
};

#endif // UI_ELEMENT_IMPL_H