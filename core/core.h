#ifndef CORE_H
#define CORE_H

#include "core/api.h"

/* --------------TYPES-------------- */
#include "core/types/color.h"
#include "core/types/hashtable.h"
#include "core/types/slice.h"
#include "core/types/string.h"
/* --------------------------------- */

/* --------------MANAGERS-------------- */
#include "core/events/event_manager.h"
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

typedef struct LSCore LSCore;

LSCore *core_create(FlagManager *flag_manager);
void core_destroy(LSCore *core);

void core_start(const LSCore *core);
void core_poll(const LSCore *core);

LS_EXPORT FlagManager *core_get_flag_manager(const LSCore *core);
LS_EXPORT InputManager *core_get_input_manager(const LSCore *core);
LS_EXPORT EventManager *core_get_event_manager(const LSCore *core);
LS_EXPORT const OS *core_get_os(const LSCore *core);

#endif // CORE_H
