#ifndef AUDIO_SERVER_INTERNAL_H
#define AUDIO_SERVER_INTERNAL_H

#include <miniaudio.h>

// TODO: make these a scons flags
#ifndef AUDIO_DEVICE_FORMAT
#define AUDIO_DEVICE_FORMAT ma_format_f32
#endif // AUDIO_DEVICE_FORMAT

#ifndef AUDIO_DEVICE_CHANNELS
#define AUDIO_DEVICE_CHANNELS 2
#endif // AUDIO_DEVICE_CHANNELS

#ifndef AUDIO_DEVICE_SAMPLE_RATE
#define AUDIO_DEVICE_SAMPLE_RATE 0 // 0 means use the default sample rate
#endif // AUDIO_DEVICE_SAMPLE_RATE

ma_device audio_server_get_device();

#endif // AUDIO_SERVER_INTERNAL_H