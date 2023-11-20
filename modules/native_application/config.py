#!/usr/bin/env python

def can_build(env, platform):
    # TODO: Add support for other platforms.
    return env["platform"] == "linuxbsd"


def configure(env):
    from SCons.Script import BoolVariable, Variables

    env_vars = Variables()

    env_vars.AddVariables(
        BoolVariable(
            "api_test_client",
            "Build the API Test Client.",
            True,
        ),
    )

    env_vars.Update(env)

