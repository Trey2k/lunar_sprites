import os
import platform
import sys
from methods import get_compiler_version, using_gcc
from platform_methods import detect_arch

from typing import TYPE_CHECKING

if TYPE_CHECKING:
    from SCons import Environment


def get_name():
    return "LinuxBSD"


def can_build():
    if os.name != "posix" or sys.platform == "darwin":
        return False

    pkgconf_error = os.system("pkg-config --version > /dev/null")
    if pkgconf_error:
        print("Error: pkg-config not found. Aborting.")
        return False

    return True


def get_opts():
    from SCons.Variables import BoolVariable, EnumVariable

    return [
        EnumVariable("linker", "Linker program", "default", ("default", "bfd", "gold", "lld", "mold")),
        BoolVariable("use_llvm", "Use the LLVM compiler", False),
        BoolVariable("use_static_c", "Link libgcc statically for better portability", True),
        BoolVariable("use_asan", "Use LLVM/GCC compiler address sanitizer (ASAN)", False),
        BoolVariable("use_lsan", "Use LLVM/GCC compiler leak sanitizer (LSAN)", False),
        BoolVariable("use_tsan", "Use LLVM/GCC compiler thread sanitizer (TSAN)", False),
        BoolVariable("use_msan", "Use LLVM compiler memory sanitizer (MSAN)", False),
        BoolVariable("use_sowrap", "Dynamically load system libraries", True),
    ]


def get_flags():
    return [
        ("arch", detect_arch()),
    ]


def configure(env: "Environment"):
    # Validate arch.
    supported_arches = ["x86_32", "x86_64", "arm32", "arm64"]
    if env["arch"] not in supported_arches:
        print(
            'Unsupported CPU architecture "%s" for Linux / *BSD. Supported architectures are: %s.'
            % (env["arch"], ", ".join(supported_arches))
        )
        sys.exit(255)

    ## Build type

    if env.dev_build:
        env.Append(LINKFLAGS=["-rdynamic"])

    host_is_64_bit = sys.maxsize > 2**32
    if host_is_64_bit and env["arch"] == "x86_32":
        env.Append(CCFLAGS=["-m32"])
        env.Append(LINKFLAGS=["-m32"])
    elif not host_is_64_bit and env["arch"] == "x86_64":
        env.Append(CCFLAGS=["-m64"])
        env.Append(LINKFLAGS=["-m64"])

    ## Compiler configuration

    if "CC" in env and "clang" in os.path.basename(env["CC"]):
        # Convenience check to enforce the use_llvm overrides when CC is clang(++)
        env["use_llvm"] = True

    if env["use_llvm"]:
        if "clang" not in os.path.basename(env["CC"]):
            env["CC"] = "clang"
        env.extra_suffix = ".llvm" + env.extra_suffix

    if env["linker"] != "default":
        print("Using linker program: " + env["linker"])
        env.Append(LINKFLAGS=["-fuse-ld=%s" % env["linker"]])

    # LTO

    if env["lto"] == "auto":  # Full LTO for production.
        env["lto"] = "full"

    if env["lto"] != "none":
        if env["lto"] == "thin":
            if not env["use_llvm"]:
                print("ThinLTO is only compatible with LLVM, use `use_llvm=yes` or `lto=full`.")
                sys.exit(255)
            env.Append(CCFLAGS=["-flto=thin"])
            env.Append(LINKFLAGS=["-flto=thin"])
        elif not env["use_llvm"] and env.GetOption("num_jobs") > 1:
            env.Append(CCFLAGS=["-flto"])
            env.Append(LINKFLAGS=["-flto=" + str(env.GetOption("num_jobs"))])
        else:
            env.Append(CCFLAGS=["-flto"])
            env.Append(LINKFLAGS=["-flto"])

        if not env["use_llvm"]:
            env["RANLIB"] = "gcc-ranlib"
            env["AR"] = "gcc-ar"

    env.Append(CCFLAGS=["-pipe"])

    ## Dependencies
    ## TODO: Need Deps

    env.Append(
        CPPDEFINES=[
            "LINUXBSD_ENABLED",
            "UNIX_ENABLED",
        ]
    )

    env.Append(LIBS=["pthread"])

    if platform.system() == "Linux":
        env.Append(LIBS=["dl"])

    if platform.system() == "FreeBSD":
        env.Append(LINKFLAGS=["-lkvm"])

    # Link those statically for portability
    if env["use_static_c"]:
        env.Append(LINKFLAGS=["-static-libgcc"])
        if env["use_llvm"] and platform.system() != "FreeBSD":
            env["LINKCOM"] = env["LINKCOM"] + " -l:libatomic.a"
    else:
        if env["use_llvm"] and platform.system() != "FreeBSD":
            env.Append(LIBS=["atomic"])
