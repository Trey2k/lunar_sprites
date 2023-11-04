#ifndef VERSION_H
#define VERSION_H

#include "core/version_info.gen.h"

#define VERSION_BRANCH _MKSTR(VERSION_MAJOR) "." _MKSTR(VERSION_MINOR)

#if VERSION_PATCH
#define VERSION_NUMBER VERSION_BRANCH "." _MKSTR(VERSION_PATCH)
#else
#define VERSION_NUMBER VERSION_BRANCH
#endif // VERSION_PATCH

#define VERSION_HEX 0x10000 * VERSION_MAJOR + 0x100 * VERSION_MINOR + VERSION_PATCH

#define VERSION_FULL_CONFIG VERSION_NUMBER "." VERSION_STATUS VERSION_MODULE_CONFIG

#define VERSION_FULL_BUILD VERSION_FULL_CONFIG "." VERSION_BUILD

#define VERSION_FULL_NAME VERSION_NAME " v" VERSION_FULL_BUILD

extern const char *const VERSION_HASH;

#endif // VERSION_H