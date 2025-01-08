#ifndef AUDIO_WAVE_H
#define AUDIO_WAVE_H

#include "core/core.h"

typedef struct Wave {
	uint32 frameCount; // Total number of frames (considering channels)
	uint32 sampleRate; // Frequency (samples per second)
	uint32 sampleSize; // Bit depth (bits per sample): 8, 16, 32 (24 not supported)
	uint32 channels; // Number of channels (1-mono, 2-stereo, ...)
	void *data; // Buffer data pointer
} Wave;

Wave load_wave(String filename);
Wave load_wave_from_memory(String file_extension, const uint8 *data, size_t size);
void unload_wave(Wave wave);

bool is_wave_ready(Wave wave);

#endif // AUDIO_WAVE_H