#include "core/core.h"

#define DRWAV_MALLOC ls_malloc
#define DRWAV_REALLOC ls_realloc
#define DRWAV_FREE ls_free

#define DR_WAV_IMPLEMENTATION
#include <dr_wav.h>