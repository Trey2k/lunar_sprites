#!/usr/bin/env python

def can_build(env, platform):
    return True


def configure(env):
    env.api_headers += [
        "modules/font_renderer/font_renderer.h",
    ]
    print("Configuring font_renderer")
