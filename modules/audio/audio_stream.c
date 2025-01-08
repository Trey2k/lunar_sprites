#include "audio_stream.h"

#include "internal/audio_buffer.h"
#include "internal/audio_server.h"
#include "internal/wave.h"

struct AudioStream {
	AudioBuffer *buffer;
	AudioProcessor *processor;

	uint32 sampleRate;
	uint32 sampleSize;
	uint32 channels;
};

struct Sound {
	AudioStream stream;
	uint32 frame_count;
};

static Sound *sound_create_from_wave(Wave wave);

Sound *sound_create(String filename) {
	Wave wave = load_wave(filename);

	Sound *sound = sound_create_from_wave(wave);

	unload_wave(wave);

	return sound;
}

Sound *sound_alias(Sound *alias) {
	Sound *sound = (Sound *)ls_calloc(1, sizeof(Sound));
	if (alias->stream.buffer->data == NULL) {
		return sound;
	}

	ma_device device = audio_server_get_device();

	AudioBuffer *buffer = audio_buffer_create(AUDIO_DEVICE_FORMAT, AUDIO_DEVICE_CHANNELS,
			alias->stream.sampleRate, alias->frame_count, AUDIO_BUFFER_USAGE_STATIC);
	if (buffer == NULL) {
		ls_log(LOG_LEVEL_ERROR, "Failed to create AudioBuffer\n");
		ls_free(sound);
		return NULL;
	}

	buffer->frame_count = alias->stream.buffer->frame_count;
	buffer->volume = alias->stream.buffer->volume;
	buffer->data = alias->stream.buffer->data;

	sound->frame_count = alias->frame_count;
	sound->stream.sampleRate = device.sampleRate;
	sound->stream.sampleSize = 32;
	sound->stream.channels = AUDIO_DEVICE_CHANNELS;
	sound->stream.buffer = buffer;

	return sound;
}

void sound_destroy(Sound *sound) {
	if (sound == NULL) {
		return;
	}

	audio_buffer_destroy(sound->stream.buffer);
	ls_free(sound);
}

bool is_sound_ready(Sound *sound) {
	return ((sound->frame_count > 0) &&
			(sound->stream.buffer != NULL) &&
			(sound->stream.sampleRate > 0) &&
			(sound->stream.sampleSize > 0) &&
			(sound->stream.channels > 0));
}

bool is_sound_playing(Sound *sound) {
	return audio_buffer_is_playing(sound->stream.buffer);
}

void sound_play(Sound *sound) {
	audio_buffer_play(sound->stream.buffer);
}

void sound_stop(Sound *sound) {
	audio_buffer_stop(sound->stream.buffer);
}

void sound_pause(Sound *sound) {
	audio_buffer_pause(sound->stream.buffer);
}

void sound_resume(Sound *sound) {
	audio_buffer_resume(sound->stream.buffer);
}

void sound_set_volume(Sound *sound, float32 volume) {
	audio_buffer_set_volume(sound->stream.buffer, volume);
}

void sound_set_pitch(Sound *sound, float32 pitch) {
	audio_buffer_set_pitch(sound->stream.buffer, pitch);
}

void sound_set_pan(Sound *sound, float32 pan) {
	audio_buffer_set_pan(sound->stream.buffer, pan);
}

// Static functions

static Sound *sound_create_from_wave(Wave wave) {
	Sound *sound = (Sound *)ls_calloc(1, sizeof(Sound));
	if (sound == NULL) {
		ls_log(LOG_LEVEL_ERROR, "Failed to allocate memory for Sound\n");
		return NULL;
	}

	ma_format format_in = ((wave.sampleSize == 8) ? ma_format_u8 : ((wave.sampleSize == 16) ? ma_format_s16 : ma_format_f32));

	ma_device device = audio_server_get_device();
	uint32 frame_count_in = wave.frameCount;

	uint32 frame_count = (uint32)ma_convert_frames(NULL, 0, AUDIO_DEVICE_FORMAT, AUDIO_DEVICE_CHANNELS,
			device.sampleRate, NULL, frame_count_in, format_in, wave.channels, wave.sampleRate);

	AudioBuffer *buffer = audio_buffer_create(AUDIO_DEVICE_FORMAT, AUDIO_DEVICE_CHANNELS, device.sampleRate, frame_count, AUDIO_BUFFER_USAGE_STATIC);
	if (buffer == NULL) {
		ls_log(LOG_LEVEL_ERROR, "Failed to create AudioBuffer\n");
		ls_free(sound);
		return NULL;
	}

	frame_count = (uint32)ma_convert_frames(buffer->data, frame_count, AUDIO_DEVICE_FORMAT, AUDIO_DEVICE_CHANNELS,
			device.sampleRate, wave.data, frame_count_in, format_in, wave.channels, wave.sampleRate);

	if (frame_count == 0) {
		ls_log(LOG_LEVEL_ERROR, "Failed to convert frames\n");
		audio_buffer_destroy(buffer);
		ls_free(sound);
		return NULL;
	}

	sound->frame_count = frame_count;
	sound->stream.sampleRate = device.sampleRate;
	sound->stream.sampleSize = 32;
	sound->stream.channels = AUDIO_DEVICE_CHANNELS;
	sound->stream.buffer = buffer;

	return sound;
}