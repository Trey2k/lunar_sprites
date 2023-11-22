#!/usr/bin/env python

def can_build(env, platform):
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

