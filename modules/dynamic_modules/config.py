#!/usr/bin/env python
import api_builder


def can_build(env, platform):
    # TODO: This is a hack to make sure the audio module is built before the dynamic_modules module.
    # Should add a way to force build position in the future.
    env.module_add_dependencies("dynamic_modules", ["audio"], True)
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


    pre_headers = env.api_headers
    
    env.api_headers = [
        'core/version_info.gen.h',
        'core/version.h',
        'core/types/typedefs.h',
        'core/types/color.h',
        'core/types/string.h',
        'core/log.h',
        'core/memory.h',
        'core/debug.h',
        'core/types/cow_data.h',
        'core/math/math.h',
        'core/math/vector.h',
        'core/math/matrix.h',
        'core/math/quaternion.h',
        'core/types/slice.h',
        'core/types/hashtable.h',
        'core/types/bstring.h',
        'core/types/dictionary.h',
        'core/types/array.h',
        'core/object/object.h',
        'core/resource/resource.h',
        'core/types/variant.h',
        'core/object/object_db.h',
        'core/resource/resource_db.h',
        'core/flags.h',
        'core/window.h',
        'core/input/keycodes.h',
        'core/input/input_manager.h',
        'core/events/events.h',
        'core/events/event_manager.h',
        'core/os/os.h',
        'core/os/thread.h',
        'core/config.h',
        'core/core.h',
        'core/json.h',
        
        'renderer/window.h',
        'renderer/renderer.h',
        'renderer/shader.h',
        'renderer/buffers.h',
        'renderer/vertex_array.h',
        'renderer/texture.h',
        'renderer/camera.h',
        'renderer/sprite.h',
        'renderer/sprite_object.h',
        'renderer/batch_renderer.h',
    
        'main/application.h',
        'main/lunar_sprites.h',

        'modules/initialize_modules.h',
    
        'modules/dynamic_modules/dynamic_modules.h',
        ]
    
    env.api_headers += pre_headers