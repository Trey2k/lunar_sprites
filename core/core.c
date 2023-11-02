#include "core/core.h"

#include "core/typedefs.h"
#include "core/version.h"

#include <stdio.h>

void initialize_core() {
	printf("Starting %s\n", VERSION_FULL_NAME);
}

void uninitialize_core() {
	printf("Exiting %s\n", VERSION_FULL_NAME);
}
