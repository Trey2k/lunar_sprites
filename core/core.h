#ifndef CORE_H
#define CORE_H

#include "core/api.h"

/* --------------TYPES-------------- */
#include "core/object/object.h"
#include "core/resource/resource.h"
#include "core/types/array.h"
#include "core/types/bstring.h"
#include "core/types/color.h"
#include "core/types/dictionary.h"
#include "core/types/hashtable.h"
#include "core/types/slice.h"
#include "core/types/string.h"
#include "core/types/variant.h"

/* --------------------------------- */

/* --------------MANAGERS-------------- */
#include "core/input/input_manager.h"
/* ------------------------------------ */

/* --------------COMPONENTS-------------- */
#include "core/config.h"
#include "core/debug.h"
#include "core/flags.h"
#include "core/log.h"
#include "core/math/math.h"
#include "core/math/matrix.h"
#include "core/math/vector.h"
#include "core/memory.h"
#include "core/os/os.h"
#include "core/os/thread.h"
#include "core/version.h"
/* -------------------------------------- */

void core_init();
void core_deinit();

void core_check_flags();

#endif // CORE_H
