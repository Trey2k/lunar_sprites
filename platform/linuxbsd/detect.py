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
        BoolVariable("use_x11", "Use X11 display server", True),
        BoolVariable("use_wayland", "Use Wayland display server", False),
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
    
    if env["use_wayland"]:
        env.Append(CPPDEFINES=["WAYLAND_ENABLED"])

        if os.system("pkg-config --exists wayland-client"):
            print("Error: Wayland libraries not found. Aborting.")
            sys.exit(255)
        env.ParseConfig("pkg-config wayland-client --cflags --libs")

        if os.system("pkg-config --exists wayland-cursor"):
            print("Error: Wayland cursor libraries not found. Aborting.")
            sys.exit(255)
        env.ParseConfig("pkg-config wayland-cursor --cflags --libs")

        if os.system("pkg-config --exists wayland-egl"):
            print("Error: Wayland EGL libraries not found. Aborting.")
            sys.exit(255)
        env.ParseConfig("pkg-config wayland-egl --cflags --libs")

        if os.system("pkg-config --exists wayland-protocols"):
            print("Error: Wayland protocols not found. Aborting.")
            sys.exit(255)
        env.ParseConfig("pkg-config wayland-protocols --cflags --libs")

        wayland_protocols_dir = os.popen("pkg-config wayland-protocols --variable=pkgdatadir").read().strip().replace("//", "/")
        print("Wayland protocols dir: " + wayland_protocols_dir)
        os.system("wayland-scanner client-header %s/stable/xdg-shell/xdg-shell.xml %s/xdg-shell.gen.h" % (wayland_protocols_dir, "platform/linuxbsd/wayland"))
        os.system("wayland-scanner private-code %s/stable/xdg-shell/xdg-shell.xml %s/xdg-shell.gen.c" % (wayland_protocols_dir, "platform/linuxbsd/wayland"))

    if env["use_x11"]:
        env.Append(CPPDEFINES=["X11_ENABLED"])

        if os.system("pkg-config --exists x11"):
            print("Error: X11 libraries not found. Aborting.")
            sys.exit(255)
        env.ParseConfig("pkg-config x11 --cflags --libs")
