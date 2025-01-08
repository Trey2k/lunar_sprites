#!/usr/bin/env python

def can_build(env, platform):
    return True


def configure(env):
    from SCons.Script import BoolVariable, Variables
    env_vars = Variables()

    env_vars.AddVariables(
        BoolVariable(
            "audio_support_mp3",
            "Enable MP3 File support",
            True,
        ),
        BoolVariable(
            "audio_support_wav",
            "Enable WAV File support",
            True,
        ),
        BoolVariable(
            "audio_support_flac",
            "Enable FLAC File support",
            True,
        ),
        BoolVariable(
            "audio_support_ogg",
            "Enable OGG File support",
            True,
        ),
    )

    env_vars.Update(env)
    
    env.api_headers += [
        "modules/audio/audio_server.h",
        "modules/audio/audio_stream.h",
    ]

