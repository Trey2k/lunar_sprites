#ifndef AUDIO_STREAM_H
#define AUDIO_STREAM_H

#include "core/core.h"

typedef struct AudioStream AudioStream;

// TODO: Add support for music streaming
// typedef struct Music Music;

// A sound is a static audio buffer meant for short sound effects ~10 seconds or less
typedef struct Sound Sound;

// Loads a sound from a file
LS_EXPORT Sound *sound_create(String filename);
// Creates a new sound that uses the same source data as the given sound. It does not own the source data.
LS_EXPORT Sound *sound_alias(Sound *alias);
// Destroys the sound
LS_EXPORT void sound_destroy(Sound *sound);

// Check if the sound is ready to be played
LS_EXPORT bool is_sound_ready(Sound *sound);
// Check if the sound is playing
LS_EXPORT bool is_sound_playing(Sound *sound);
// Play the sound
LS_EXPORT void sound_play(Sound *sound);
// Stop the sound
LS_EXPORT void sound_stop(Sound *sound);
// Pause the sound
LS_EXPORT void sound_pause(Sound *sound);
// Resume the sound
LS_EXPORT void sound_resume(Sound *sound);
// Set the volume of the sound
LS_EXPORT void sound_set_volume(Sound *sound, float32 volume);
// Set the pitch of the sound
LS_EXPORT void sound_set_pitch(Sound *sound, float32 pitch);
// Set the pan of the sound
LS_EXPORT void sound_set_pan(Sound *sound, float32 pan);

#endif // AUDIO_STREAM_H