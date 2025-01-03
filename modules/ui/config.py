#!/usr/bin/env python

def can_build(env, platform):
    env.module_add_dependencies("ui", ["font"], False)
    return True


def configure(env):
    env.api_headers += [
        "modules/ui/theme.h",
        "modules/ui/elements.h",
        "modules/ui/ui.h",
    ]

