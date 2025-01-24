#!/usr/bin/env python

# TODO: Fix this module to work with the new API.
def can_build(env, platform):
    return False


def configure(env):
    env.Append(CPPDEFINES=["MAKE_LIB"])

    if env['platform'] == 'linuxbsd':
        env.Append(CPPDEFINES=["LUA_USE_POSIX"])
    elif env['platform'] == 'ios':
        env.Append(CPPDEFINES=['LUA_USE_IOS'])

    env.api_headers += [
        "modules/lua/types/lua_state.h",
        "modules/lua/lua_state.h",
        "modules/lua/types/lua_camera.h",
        "modules/lua/types/lua_core.h",
        "modules/lua/types/lua_event_manager.h",
        "modules/lua/types/lua_input_manager.h",
        "modules/lua/types/lua_color.h",
        "modules/lua/types/lua_matrix.h",
        "modules/lua/types/lua_renderer.h",
        "modules/lua/types/lua_sprite.h",
        "modules/lua/types/lua_vector.h",
        "modules/lua/types/lua_window.h",
    ]