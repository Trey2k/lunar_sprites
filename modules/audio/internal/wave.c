#include "wave.h"
#include "core/os/os.h"

#if defined(AUDIO_SUPPORT_WAV)
#include <dr_wav.h>
#endif // AUDIO_SUPPORT_WAV

Wave load_wave(String filename) {
	Wave wave = { 0 };

	size_t data_size = 0;
	uint8 *data = (uint8 *)os_read_file(filename, &data_size);
	if (data == NULL) {
		ls_log(LOG_LEVEL_ERROR, "Failed to load wave: %s\n", filename);
		return wave;
	}

	char *file_extension = os_path_get_extension(filename);
	wave = load_wave_from_memory(file_extension, data, data_size);
	ls_free(data);

	return wave;
}

void unload_wave(Wave wave) {
	if (wave.data != NULL) {
		ls_free(wave.data);
	}
}

Wave load_wave_from_memory(String file_extension, const uint8 *data, size_t size) {
	Wave wave = { 0 };

	if (ls_str_equals(file_extension, "wav") || ls_str_equals(file_extension, "WAV")) {
#if defined(AUDIO_SUPPORT_WAV)
		drwav wav = { 0 };
		bool success = drwav_init_memory(&wav, data, size, NULL);
		if (!success) {
			ls_log(LOG_LEVEL_ERROR, "Failed to load wave from memory\n");
			return wave;
		}

		wave.frameCount = (uint32)wav.totalPCMFrameCount;
		wave.sampleRate = wav.sampleRate;
		wave.sampleSize = 16;
		wave.channels = wav.channels;
		wave.data = (int16 *)ls_malloc(wav.totalPCMFrameCount * wav.channels * sizeof(int16));

		drwav_read_pcm_frames_s16(&wav, wav.totalPCMFrameCount, (int16 *)wave.data);
		drwav_uninit(&wav);
#else
		ls_log(LOG_LEVEL_ERROR, "WAV support is not enabled\n");
#endif // AUDIO_SUPPORT_WAV
	} else {
		ls_log(LOG_LEVEL_ERROR, "Unsupported file extension: %s\n", file_extension);
	}

	return wave;
}

bool is_wave_ready(Wave wave) {
	return ((wave.data != NULL) &&
			(wave.frameCount > 0) &&
			(wave.sampleRate > 0) &&
			(wave.sampleSize > 0) &&
			(wave.channels > 0));
}