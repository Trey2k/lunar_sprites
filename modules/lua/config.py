#!/usr/bin/env python

def can_build(env, platform):
    return True


def configure(env):
    env.Append(CPPDEFINES=["MAKE_LIB"])

    if env['platform'] == 'linuxbsd':
        env.Append(CPPDEFINES=["LUA_USE_POSIX"])
    elif env['platform'] == 'ios':
        env.Append(CPPDEFINES=['LUA_USE_IOS'])

