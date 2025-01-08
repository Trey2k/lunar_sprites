#ifndef AUDIO_BUFFER_H
#define AUDIO_BUFFER_H

#include "core/core.h"

#include <miniaudio.h>

#include "modules/audio/audio_server.h"

typedef enum {
	AUDIO_BUFFER_USAGE_STATIC,
	AUDIO_BUFFER_USAGE_STREAM
} AudioBufferUsage;

struct AudioBuffer {
	ma_data_converter converter;

	AudioCallback callback;
	AudioProcessor *processor;

	float32 volume;
	float32 pitch;
	float32 pan;

	bool is_playing;
	bool is_looping;
	bool is_paused;

	AudioBufferUsage usage;

	bool is_sub_buffer_processed[2];

	uint32 frame_count;
	uint32 frame_index;
	uint32 frames_processed;

	uint8 *data;

	AudioBuffer *next;
	AudioBuffer *prev;
};

AudioBuffer *audio_buffer_create(ma_format format, uint32 channels, uint32 sample_rate, uint32 frame_count, AudioBufferUsage usage);
void audio_buffer_destroy(AudioBuffer *buffer);

bool audio_buffer_is_playing(AudioBuffer *buffer);

void audio_buffer_play(AudioBuffer *buffer);
void audio_buffer_stop(AudioBuffer *buffer);
void audio_buffer_pause(AudioBuffer *buffer);
void audio_buffer_resume(AudioBuffer *buffer);
void audio_buffer_set_volume(AudioBuffer *buffer, float32 volume);
void audio_buffer_set_pitch(AudioBuffer *buffer, float32 pitch);
void audio_buffer_set_pan(AudioBuffer *buffer, float32 pan);

#endif // AUDIO_BUFFER_H