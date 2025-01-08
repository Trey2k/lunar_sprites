#include "audio_buffer.h"
#include "audio_server.h"

#include "modules/audio/audio_server.h"

AudioBuffer *audio_buffer_create(ma_format format, uint32 channels, uint32 sample_rate, uint32 frame_count, AudioBufferUsage usage) {
	ma_device device = audio_server_get_device();

	AudioBuffer *buffer = (AudioBuffer *)ls_calloc(1, sizeof(AudioBuffer));
	if (buffer == NULL) {
		ls_log(LOG_LEVEL_ERROR, "Failed to allocate memory for AudioBuffer\n");
		return NULL;
	}
	buffer->data = NULL;
	if (frame_count > 0) {
		buffer->data = ls_calloc(frame_count * channels * ma_get_bytes_per_frame(format, channels), 1);
	}

	ma_data_converter_config config = ma_data_converter_config_init(format, AUDIO_DEVICE_FORMAT, channels, AUDIO_DEVICE_CHANNELS, sample_rate, device.sampleRate);
	config.allowDynamicSampleRate = true;

	ma_result result = ma_data_converter_init(&config, NULL, &buffer->converter);
	if (result != MA_SUCCESS) {
		ls_log(LOG_LEVEL_ERROR, "Failed to initialize data converter\n");
		if (buffer->data) {
			ls_free(buffer->data);
		}

		ls_free(buffer);
		return NULL;
	}

	buffer->volume = 1.0f;
	buffer->pitch = 1.0f;
	buffer->pan = 0.5f;

	buffer->callback = NULL;
	buffer->processor = NULL;

	buffer->is_playing = false;
	buffer->is_looping = false;
	buffer->is_paused = false;

	buffer->usage = usage;
	buffer->frame_index = 0;
	buffer->frame_count = frame_count;

	buffer->is_sub_buffer_processed[0] = true;
	buffer->is_sub_buffer_processed[1] = true;

	audio_server_track_buffer(buffer);

	return buffer;
}

void audio_buffer_destroy(AudioBuffer *buffer) {
	if (buffer == NULL) {
		return;
	}
	ma_data_converter_uninit(&buffer->converter, NULL);
	audio_server_untrack_buffer(buffer);
	if (buffer->data) {
		ls_free(buffer->data);
	}

	ls_free(buffer);
}

bool audio_buffer_is_playing(AudioBuffer *buffer) {
	bool result = false;
	if (buffer != NULL && !buffer->is_paused) {
		result = buffer->is_playing;
	}

	return result;
}

void audio_buffer_play(AudioBuffer *buffer) {
	if (buffer == NULL) {
		return;
	}

	buffer->is_playing = true;
	buffer->is_paused = false;
	buffer->frame_index = 0;
}

void audio_buffer_stop(AudioBuffer *buffer) {
	if (buffer == NULL) {
		return;
	}

	buffer->is_playing = false;
	buffer->is_paused = false;
	buffer->frame_index = 0;
	buffer->frames_processed = 0;
	buffer->is_sub_buffer_processed[0] = true;
	buffer->is_sub_buffer_processed[1] = true;
}

void audio_buffer_pause(AudioBuffer *buffer) {
	if (buffer == NULL) {
		return;
	}

	buffer->is_paused = true;
}

void audio_buffer_resume(AudioBuffer *buffer) {
	if (buffer == NULL) {
		return;
	}

	buffer->is_paused = false;
}

void audio_buffer_set_volume(AudioBuffer *buffer, float32 volume) {
	if (buffer == NULL) {
		return;
	}

	buffer->volume = volume;
}

void audio_buffer_set_pitch(AudioBuffer *buffer, float32 pitch) {
	if (buffer == NULL) {
		return;
	}

	uint32 output_sample_rate = (uint32)((float32)(buffer->converter.sampleRateOut / pitch));
	ma_data_converter_set_rate(&buffer->converter, buffer->converter.sampleRateIn, output_sample_rate);

	buffer->pitch = pitch;
}

void audio_buffer_set_pan(AudioBuffer *buffer, float32 pan) {
	if (buffer == NULL) {
		return;
	}

	if (pan < 0.0f) {
		pan = 0.0f;
	} else if (pan > 1.0f) {
		pan = 1.0f;
	}

	buffer->pan = pan;
}

void audio_buffer_set_next(AudioBuffer *buffer, AudioBuffer *next) {
	if (buffer == NULL) {
		return;
	}

	buffer->next = next;
}

void audio_buffer_set_prev(AudioBuffer *buffer, AudioBuffer *prev) {
	if (buffer == NULL) {
		return;
	}

	buffer->prev = prev;
}

AudioBuffer *audio_buffer_get_next(AudioBuffer *buffer) {
	if (buffer == NULL) {
		return NULL;
	}

	return buffer->next;
}

AudioBuffer *audio_buffer_get_prev(AudioBuffer *buffer) {
	if (buffer == NULL) {
		return NULL;
	}

	return buffer->prev;
}

void *audio_buffer_get_data(AudioBuffer *buffer) {
	if (buffer == NULL) {
		return NULL;
	}

	return buffer->data;
}