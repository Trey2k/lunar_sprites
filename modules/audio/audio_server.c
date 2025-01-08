#include "audio_server.h"

#include "internal/audio_buffer.h"
#include "internal/audio_server.h"

#include <miniaudio.h>

struct AudioProcessor {
	AudioCallback process;
	AudioProcessor *next;
	AudioProcessor *prev;
};

typedef struct {
	struct {
		ma_context context;
		ma_device device;
		ma_mutex lock;
		bool is_ready;
		size_t pcm_buffer_size;
		void *pcm_buffer;
	} Server;

	struct {
		AudioBuffer *first;
		AudioBuffer *last;
		uint32 default_size;
	} Buffer;

	AudioProcessor *mixed_processor;
} Audio;

static Audio AUDIO = {
	.Buffer.default_size = 0,
	.mixed_processor = NULL
};

static void on_log(void *p_user_data, uint32 level, String message);
static void on_send_audio_data(ma_device *device, void *output, const void *input, uint32 frame_count);
static void mix_audio_frames(float32 *frames_out, const float *frames_in, uint32 frame_count, AudioBuffer *buffer);

void audio_server_init(LSCore *p_core) {
	ma_context_config ctx_config = ma_context_config_init();
	ma_log_callback_init(on_log, NULL);

	ma_result result = ma_context_init(NULL, 0, &ctx_config, &AUDIO.Server.context);
	if (result != MA_SUCCESS) {
		ls_log(LOG_LEVEL_ERROR, "Failed to initialize audio context\n");
		return;
	}

	ma_device_config config = ma_device_config_init(ma_device_type_playback);
	config.playback.pDeviceID = NULL; // Use the default playback device
	config.playback.format = AUDIO_DEVICE_FORMAT;
	config.playback.channels = AUDIO_DEVICE_CHANNELS;
	config.capture.pDeviceID = NULL; // Use the default capture device
	config.capture.format = ma_format_s16;
	config.capture.channels = 1;
	config.sampleRate = AUDIO_DEVICE_SAMPLE_RATE;
	config.dataCallback = on_send_audio_data;
	config.pUserData = NULL;

	result = ma_device_init(NULL, &config, &AUDIO.Server.device);
	if (result != MA_SUCCESS) {
		ls_log(LOG_LEVEL_ERROR, "Failed to initialize audio device\n");
		ma_context_uninit(&AUDIO.Server.context);
		return;
	}

	result = ma_device_start(&AUDIO.Server.device);
	if (result != MA_SUCCESS) {
		ls_log(LOG_LEVEL_ERROR, "Failed to start audio device\n");
		ma_device_uninit(&AUDIO.Server.device);
		ma_context_uninit(&AUDIO.Server.context);
		return;
	}

	ls_log(LOG_LEVEL_INFO, "Audio Server initialized successfully\n");
	ls_log(LOG_LEVEL_INFO, "Audio Server backend:		miniaudio / %s\n", ma_get_backend_name(AUDIO.Server.context.backend));
	ls_log(LOG_LEVEL_INFO, "Audio Server format:		%s -> %s\n", ma_get_format_name(AUDIO.Server.device.playback.format), ma_get_format_name(AUDIO.Server.device.playback.internalFormat));
	ls_log(LOG_LEVEL_INFO, "Audio Server channels:		%d -> %d\n", AUDIO.Server.device.playback.channels, AUDIO.Server.device.playback.internalChannels);
	ls_log(LOG_LEVEL_INFO, "Audio Server sample rate:	%d -> %d\n", AUDIO.Server.device.sampleRate, AUDIO.Server.device.playback.internalSampleRate);
	ls_log(LOG_LEVEL_INFO, "Audio Server period size:	%d\n", AUDIO.Server.device.playback.internalPeriodSizeInFrames * AUDIO.Server.device.playback.internalPeriods);

	AUDIO.Server.is_ready = true;
}

void audio_server_deinit() {
	if (!AUDIO.Server.is_ready) {
		ls_log(LOG_LEVEL_WARNING, "Audio Server is not initialized\n");
		return;
	}

	ma_mutex_uninit(&AUDIO.Server.lock);
	ma_device_uninit(&AUDIO.Server.device);
	ma_context_uninit(&AUDIO.Server.context);

	AUDIO.Server.is_ready = false;

	ls_log(LOG_LEVEL_INFO, "Audio Server deinitialized successfully\n");
}

void audio_server_track_buffer(AudioBuffer *p_buffer) {
	ma_mutex_lock(&AUDIO.Server.lock);
	{
		if (AUDIO.Buffer.first == NULL) {
			AUDIO.Buffer.first = p_buffer;
		} else {
			AUDIO.Buffer.last->next = p_buffer;
			p_buffer->prev = AUDIO.Buffer.last;
		}

		AUDIO.Buffer.last = p_buffer;
	}
	ma_mutex_unlock(&AUDIO.Server.lock);
}

void audio_server_untrack_buffer(AudioBuffer *p_buffer) {
	ma_mutex_lock(&AUDIO.Server.lock);
	{
		if (p_buffer->prev == NULL) {
			AUDIO.Buffer.first = p_buffer->next;
		} else {
			p_buffer->prev->next = p_buffer->next;
		}

		if (p_buffer->next == NULL) {
			AUDIO.Buffer.last = p_buffer->prev;
		} else {
			p_buffer->next->prev = p_buffer->prev;
		}

		p_buffer->next = NULL;
		p_buffer->prev = NULL;
	}
	ma_mutex_unlock(&AUDIO.Server.lock);
}

// Internal functions

ma_device audio_server_get_device() {
	return AUDIO.Server.device;
}

// Static functions

static void on_log(void *p_user_data, uint32 level, String message) {
	switch (level) {
		case MA_LOG_LEVEL_ERROR: {
			ls_log(LOG_LEVEL_ERROR, "miniaudio: %s\n", message);
		} break;
		case MA_LOG_LEVEL_WARNING: {
			ls_log(LOG_LEVEL_WARNING, "miniaudio: %s\n", message);
		} break;
		case MA_LOG_LEVEL_INFO: {
			ls_log(LOG_LEVEL_INFO, "miniaudio: %s\n", message);
		} break;
		case MA_LOG_LEVEL_DEBUG: {
			ls_log(LOG_LEVEL_DEBUG, "miniaudio: %s\n", message);
		} break;

		default: {
			ls_log(LOG_LEVEL_INFO, "miniaudio: %s\n", message);
		} break;
	}
}

static uint32 read_buffer_internal(AudioBuffer *buffer, uint8 *p_output, uint32 frame_count) {
	if (buffer->callback) {
		buffer->callback(buffer->data, frame_count);
		buffer->frames_processed += frame_count;

		return frame_count;
	}

	uint32 sub_buffer_frame_count = (buffer->frame_count > 1) ? buffer->frame_count / 2 : buffer->frame_count;
	uint32 current_sub_buffer_index = buffer->frame_index / sub_buffer_frame_count;

	if (current_sub_buffer_index > 1) {
		return 0;
	}

	bool is_sub_buffer_processed[2] = { 0 };
	is_sub_buffer_processed[0] = buffer->is_sub_buffer_processed[0];
	is_sub_buffer_processed[1] = buffer->is_sub_buffer_processed[1];

	uint32 frame_size_bytes = ma_get_bytes_per_frame(buffer->converter.formatIn, buffer->converter.channelsIn);

	uint32 frames_read = 0;
	while (true) {
		if (buffer->usage == AUDIO_BUFFER_USAGE_STATIC) {
			if (frames_read >= frame_count) {
				break;
			}
		} else {
			if (is_sub_buffer_processed[current_sub_buffer_index]) {
				break;
			}
		}

		uint32 total_frames_remaining = frame_count - frames_read;
		if (total_frames_remaining == 0) {
			break;
		}

		uint32 frames_remaining_in_output;
		if (buffer->usage == AUDIO_BUFFER_USAGE_STATIC) {
			frames_remaining_in_output = buffer->frame_count - buffer->frame_index;
		} else {
			uint32 first_frame_index = sub_buffer_frame_count * current_sub_buffer_index;
			frames_remaining_in_output = sub_buffer_frame_count - (buffer->frame_count - first_frame_index);
		}

		uint32 frames_to_read = total_frames_remaining;
		if (frames_to_read > frames_remaining_in_output) {
			frames_to_read = frames_remaining_in_output;
		}

		ls_memcpy((uint8 *)p_output + (frames_read * frame_size_bytes), buffer->data + (buffer->frame_index * frame_size_bytes), frames_to_read * frame_size_bytes);
		buffer->frame_index = (buffer->frame_index + frames_to_read) % buffer->frame_count;
		frames_read += frames_to_read;

		if (frames_to_read == frames_remaining_in_output) {
			buffer->is_sub_buffer_processed[current_sub_buffer_index] = true;
			is_sub_buffer_processed[current_sub_buffer_index] = true;

			current_sub_buffer_index = (current_sub_buffer_index + 1) % 2;

			if (!buffer->is_looping) {
				audio_buffer_stop(buffer);
				break;
			}
		}
	}

	uint32 total_frames_remaining = (frame_count - frames_read);

	if (total_frames_remaining > 0) {
		ls_memset((uint8 *)p_output + (frames_read * frame_size_bytes), 0, total_frames_remaining * frame_size_bytes);

		if (buffer->usage != AUDIO_BUFFER_USAGE_STATIC) {
			frames_read += total_frames_remaining;
		}
	}

	return frames_read;
}

static uint32 read_buffer_frames_mixing(AudioBuffer *buffer, float32 *p_output, uint32 frame_count) {
	uint8 input_buffer[4096] = { 0 };
	uint32 input_buffer_frame_cap = sizeof(input_buffer) / ma_get_bytes_per_frame(buffer->converter.formatIn, buffer->converter.channelsIn);

	uint32 total_output_frames_processed = 0;
	while (total_output_frames_processed < frame_count) {
		uint64 output_frames_to_process = frame_count - total_output_frames_processed;
		uint64 input_frames_to_process = 0;

		ma_data_converter_get_required_input_frame_count(&buffer->converter, output_frames_to_process, &input_frames_to_process);
		if (input_frames_to_process > input_buffer_frame_cap) {
			input_frames_to_process = input_buffer_frame_cap;
		}

		float32 *running_frames_out = p_output + (total_output_frames_processed * buffer->converter.channelsOut);

		uint64 input_frames_processed = read_buffer_internal(buffer, input_buffer, (uint32)input_frames_to_process);
		uint64 output_frames_processed = output_frames_to_process;
		ma_data_converter_process_pcm_frames(&buffer->converter, input_buffer, &input_frames_processed, running_frames_out, &output_frames_processed);

		total_output_frames_processed += (uint32)output_frames_processed;

		if (input_frames_processed < input_frames_to_process) {
			break;
		}

		if (input_frames_processed == 0 && output_frames_processed == 0) {
			break;
		}
	}

	return total_output_frames_processed;
}

static void on_send_audio_data(ma_device *p_device, void *p_output, const void *p_input, uint32 frame_count) {
	ls_memset(p_output, 0, frame_count * p_device->playback.internalChannels * ma_get_bytes_per_sample(p_device->playback.internalFormat));

	ma_mutex_lock(&AUDIO.Server.lock);
	{
		for (AudioBuffer *buffer = AUDIO.Buffer.first; buffer != NULL; buffer = buffer->next) {
			if (!buffer->is_playing || buffer->is_paused) {
				continue;
			}

			uint32 frames_read = 0;

			while (frame_count > frames_read) {
				uint32 frames_to_read = (frame_count - frames_read);

				while (frames_to_read > 0) {
					float32 temp_buffer[1024] = { 0 };

					uint32 frames_to_read_now = frames_to_read;
					if (frames_to_read_now > sizeof(temp_buffer) / sizeof(temp_buffer[0]) / AUDIO_DEVICE_CHANNELS) {
						frames_to_read_now = sizeof(temp_buffer) / sizeof(temp_buffer[0]) / AUDIO_DEVICE_CHANNELS;
					}

					uint32 frames_just_read = read_buffer_frames_mixing(buffer, temp_buffer, frames_to_read_now);
					if (frames_just_read >= 0) {
						float32 *frames_out = (float32 *)p_output + (frames_read * AUDIO.Server.device.playback.channels);
						float32 *frames_in = temp_buffer;

						AudioProcessor *processor = buffer->processor;
						while (processor) {
							processor->process(frames_in, frames_just_read);
							processor = processor->next;
						}

						mix_audio_frames(frames_out, frames_in, frames_just_read, buffer);

						frames_to_read -= frames_just_read;
						frames_read += frames_just_read;
					}

					if (!buffer->is_playing) {
						frames_read = frame_count;
						break;
					}

					if (frames_just_read < frames_to_read_now) {
						if (!buffer->is_looping) {
							audio_buffer_stop(buffer);
							break;
						} else {
							buffer->frame_index = 0;
							continue;
						}
					}
				}

				if (frames_to_read == 0) {
					break;
				}
			}
		}
	}

	AudioProcessor *processor = AUDIO.mixed_processor;
	while (processor) {
		processor->process(p_output, frame_count);
		processor = processor->next;
	}

	ma_mutex_unlock(&AUDIO.Server.lock);
}

static void mix_audio_frames(float32 *frames_out, const float *frames_in, uint32 frame_count, AudioBuffer *buffer) {
	const float32 local_volume = buffer->volume;
	const uint32 channels = AUDIO.Server.device.playback.channels;

	if (channels != 2) { // We consider panning only for stereo
		for (uint32 frame = 0; frame < frame_count; frame++) {
			for (uint32 c = 0; c < channels; c++) {
				float32 *frame_out = frames_out + (frame * channels);
				const float32 *frame_in = frames_in + (frame * channels);

				frame_out[c] += (frame_in[c] * local_volume);
			}
		}

		return;
	}

	const float32 left = buffer->pan;
	const float32 right = 1.0f - left;

	const float32 levels[2] = { local_volume * 0.5f * left * (3.0f - left * left), local_volume * 0.5f * right * (3.0f - right * right) };

	float32 *frame_out = frames_out;
	const float32 *frame_in = frames_in;

	for (uint32 frame = 0; frame < frame_count; frame++) {
		frame_out[0] += (frame_in[0] * levels[0]);
		frame_out[1] += (frame_in[1] * levels[1]);

		frame_out += 2;
		frame_in += 2;
	}
}

//