#ifndef AUDIO_SERVER_H
#define AUDIO_SERVER_H

#include "core/core.h"

typedef struct AudioBuffer AudioBuffer;
typedef struct AudioProcessor AudioProcessor;

typedef void (*AudioCallback)(void *buffer_data, uint32 frames);

void audio_server_init(LSCore *core);
void audio_server_deinit();

void audio_server_track_buffer(AudioBuffer *buffer);
void audio_server_untrack_buffer(AudioBuffer *buffer);

#endif // AUDIO_SERVER_H