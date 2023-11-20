#ifndef CORE_H
#define CORE_H

#include "core/api.h"

/* --------------TYPES-------------- */
#include "core/types/hashtable.h"
#include "core/types/slice.h"
#include "core/types/string.h"
#include "core/types/vector/vector2.h"
/* --------------------------------- */

/* --------------MANAGERS-------------- */
#include "core/events/event_manager.h"
#include "core/input/input_manager.h"
/* ------------------------------------ */

/* --------------COMPONENTS-------------- */
#include "core/debug.h"
#include "core/flags.h"
#include "core/log.h"
#include "core/memory.h"
#include "core/os/os.h"
#include "core/path.h"
#include "core/time.h"
#include "core/version.h"
/* -------------------------------------- */

typedef struct LSCore LSCore;

LSCore *core_create();
void core_destroy(LSCore *core);

void core_start(const LSCore *core, int32 argc, char *argv[]);
void core_poll(const LSCore *core);

LS_EXPORT FlagManager *core_get_flag_manager(const LSCore *core);
LS_EXPORT InputManager *core_get_input_manager(const LSCore *core);
LS_EXPORT const EventManager *core_get_event_manager(const LSCore *core);
LS_EXPORT const OS *core_get_os(const LSCore *core);

LS_EXPORT void core_add_event_handler(const LSCore *core, EventHandler handler, void *user_data);

#endif // CORE_H
