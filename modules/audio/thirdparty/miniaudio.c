#include "core/core.h"

#define MA_MALLOC ls_malloc
#define MA_FREE ls_free

#define MA_NO_WAV
#define MA_NO_FLAC
#define MA_NO_MP3
#define MA_NO_RESOURCE_MANAGER
#define MA_NO_NODE_GRAPH
#define MA_NO_ENGINE
#define MA_NO_GENERATION

// Threading model: Default: [0] COINIT_MULTITHREADED: COM calls objects on any thread (free threading)
#define MA_COINIT_VALUE 2 // [2] COINIT_APARTMENTTHREADED: Each object has its own thread (apartment model)

#define MINIAUDIO_IMPLEMENTATION
#include <miniaudio.h>