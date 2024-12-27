#!/usr/bin/env python
import api_builder


def can_build(env, platform):
    # Bit of a hack
    env.api_headers = []
    return True


def configure(env):
    from SCons.Script import BoolVariable, Variables

    env_vars = Variables()

    env_vars.AddVariables(
        BoolVariable(
            "dynamic_module_test",
            "Build the API Test Client.",
            True,
        ),
    )

    env_vars.Update(env)

    env.api_headers += [
        'core/version_info.gen.h',
        'core/version.h',
        'core/types/typedefs.h',
        'core/types/color.h',
        'core/types/string.h',
        'core/log.h',
        'core/memory.h',
        'core/debug.h',
        'core/math/math.h',
        'core/math/vector.h',
        'core/math/matrix.h',
        'core/types/slice.h',
        'core/types/hashtable.h',
        'core/flags.h',
        'core/window.h',
        'core/input/keycodes.h',
        'core/input/input_manager.h',
        'core/events/events.h',
        'core/events/event_manager.h',
        'core/os.h',
        'core/config.h',
        'core/core.h',
        
        'renderer/window.h',
        'renderer/renderer.h',
        'renderer/shader.h',
        'renderer/buffers.h',
        'renderer/vertex_array.h',
        'renderer/texture.h',
        'renderer/camera.h',
        'renderer/sprite.h',
    
        'main/application.h',
        'main/lunar_sprites.h',

        'modules/initialize_modules.h',
    
        'modules/dynamic_modules/dynamic_modules.h',
        ]