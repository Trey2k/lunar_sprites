#![x for x in self["CCFLAGS"] if not x.startswith("/O")]/usr/bin/env python

EnsureSConsVersion(4, 0)
EnsurePythonVersion(3, 6)

import atexit
import glob
import os
import pickle
import sys
import time
from types import ModuleType
from collections import OrderedDict
from importlib.util import spec_from_file_location, module_from_spec


def _helper_module(name, path):
    spec = spec_from_file_location(name, path)
    module = module_from_spec(spec)
    spec.loader.exec_module(module)
    sys.modules[name] = module

    child_module = module
    parent_name = name
    while True:
        try:
            parent_name, child_name = parent_name.rsplit('.', 1)
        except ValueError:
            break
        try:
            parent_module = sys.modules[parent_name]
        except KeyError:
            parent_module = ModuleType(parent_name)
            sys.modules[parent_name] = parent_module
        setattr(parent_module, child_name, child_module)


_helper_module("methods", "methods.py")
_helper_module("platform_methods", "platform_methods.py")
_helper_module("version", "version.py")

import methods
import scu_builders
from platform_methods import architectures, architectures_aliases

platform_list = []
platform_opts = {}
platform_flags = {}

time_at_start = time.time()

for x in sorted(glob.glob("platform/*")):
    if not os.path.isdir(x) or not os.path.exists(x + "/detect.py"):
        continue
    tmppath = "./" + x

    sys.path.insert(0, tmppath)
    import detect

    platform_name = x[9:]
    if detect.can_build():
        x = x.replace("platform/", "")
        x = x.replace("platform\\", "")
        platform_list += [x]
        platform_opts[x] = detect.get_opts()
        platform_flags[x] = detect.get_flags()
    sys.path.remove(tmppath)
    sys.modules.pop("detect")

custom_tools = ["default"]

platform_arg = ARGUMENTS.get("platform", ARGUMENTS.get("p", False))

if os.name == "nt" and methods.get_cmdline_bool("use_mingw", False):
    custom_tools = ["mingw"]

env_base = Environment(tools=custom_tools)
env_base.PrependENVPath("PATH", os.environ["PATH"])
env_base.PrependENVPath("PKG_CONFIG_PATH", os.getenv("PKG_CONFIG_PATH"))
if "TERM" in os.environ:
    env_base["ENV"]["TERM"] = os.environ["TERM"]

env_base.disabled_modules = []
env_base.module_version_string = ""
env_base.msvc = False

env_base.__class__.disable_module = methods.disable_module
env_base.__class__.add_module_version_string = methods.add_module_version_string
env_base.__class__.add_source_files = methods.add_source_files
env_base.__class__.use_windows_spawn_fix = methods.use_windows_spawn_fix
env_base.__class__.add_shared_library = methods.add_shared_library
env_base.__class__.add_library = methods.add_library
env_base.__class__.add_program = methods.add_program
env_base.__class__.CommandNoCache = methods.CommandNoCache
env_base.__class__.Run = methods.Run
env_base.__class__.disable_warnings = methods.disable_warnings
env_base.__class__.force_optimization_on_debug = methods.force_optimization_on_debug
env_base.__class__.module_add_dependencies = methods.module_add_dependencies
env_base.__class__.module_check_dependencies = methods.module_check_dependencies

env_base.SConsignFile(".sconsign{0}.dblite".format(pickle.HIGHEST_PROTOCOL))

# Build options

customs = ["custom.py"]

profile = ARGUMENTS.get("profile", "")
if profile:
    if os.path.isfile(profile):
        customs.append(profile)
    elif os.path.isfile(profile + ".py"):
        customs.append(profile + ".py")

opts = Variables(customs, ARGUMENTS)

# Target options

# Target options
opts.Add("platform", "Target platform (%s)" % ("|".join(platform_list),), "")
opts.Add("p", "Platform (alias for 'platform')", "")
opts.Add(EnumVariable("target", "Compilation target", "debug", ("debug", "release")))
opts.Add(EnumVariable("arch", "CPU architecture", "auto", ["auto"] + architectures, architectures_aliases))
opts.Add(BoolVariable("dev_build", "Developer build with dev-only debugging code (DEV_ENABLED)", False))
opts.Add(
    EnumVariable(
        "optimize", "Optimization level", "speed_trace", ("none", "custom", "debug", "speed", "speed_trace", "size")
    )
)
opts.Add(BoolVariable("debug_symbols", "Build with debug symbols", False))
opts.Add(EnumVariable("lto", "Link-time optimization (production builds)", "none", ("none", "auto", "thin", "full")))
opts.Add(BoolVariable("production", "Set defaults to build Lunar Sprites for use in production", False))

# Components
opts.Add(BoolVariable("use_opengl", "Use OpenGL ES v3.2", True))

opts.Add(EnumVariable("precision", "Set the floating-point precision level", "single", ("single", "double")))
opts.Add(BoolVariable("disable_exceptions", "Force disabliplatform_apisng exception handling code", True))
opts.Add("custom_modules", "A list of comma-seperated directory paths containing custom modules to build.", "")
opts.Add(BoolVariable("custom_modules_recursive", "Recursively search for custom modules in the custom_modules path.", True))

# Advanced options
opts.Add(BoolVariable("dev_mode", "Alias for dev options: verbose=yes warnings=extra werror=yes tests=yes", False))
opts.Add(BoolVariable("tests", "Build unit tests", False))
opts.Add(BoolVariable("compiledb", "Generate compile_commands.json", False))
opts.Add(BoolVariable("verbose", "Verbose build output", False))
opts.Add(BoolVariable("progress", "Show a progress indicator during compilation", True))
opts.Add(EnumVariable("warnings", "Level of compilation warnings", "all", ("extra", "all", "moderate", "none")))
opts.Add(BoolVariable("werror", "Treat warnings as errors", False))
opts.Add("extra_suffix", "Custom extra suffix added to the base filename of all generated binary files", "")
opts.Add("object_prefix", "Custom prefix added to the base filename of all generated object files", "")
opts.Add("build_profile", "Path to a file containing a feature build profile", "")
opts.Add(BoolVariable("modules_enabled_by_default", "If no, disable all modules except ones explicitly enabled", True))
opts.Add(BoolVariable("scu_build", "Use single compilation unit build", False))
opts.Add("scu_limit", "Max includes per SCU file when using scu_build (determines RAM use)", "0")

# Compilation environment setup
opts.Add("CC", "C compiler")
opts.Add("LINK", "Linker")
opts.Add("CCFLAGS", "Custom flags for the C compiler")
opts.Add("CFALGS", "Custom flags for the C compiler")
opts.Add("LINKFLAGS", "Custom flags for the Linker")

opts.Update(env_base)

selected_platform = ""

if env_base["platform"] != "":
    selected_platform = env_base["platform"]
elif env_base["p"] != "":
    selected_platform = env_base["p"]
else:
    # Auto detect platform_list
    if (
        sys.platform.startswith("linux")
        or sys.platform.startswith("dragonfly")
        or sys.platform.startswith("freebsd")
        or sys.platform.startswith("netbsd")
        or sys.platform.startswith("openbsd")
    ):
        selected_platform = "linuxbsd"
    elif sys.platform == "darwin":
        selected_platform = "macos"
    elif sys.platform == "win32":
        selected_platform = "windows"
    else:
        print("Could not detect platform automatically, Supported platforms:")
        for x in platform_list:
            print("\t" + x)
        print("please fun SCons again and select a valid platform: platform=<string>")

env_base["platform"] = selected_platform

if selected_platform in platform_opts:
    for opt in platform_opts[selected_platform]:
        opts.Add(opt)

opts.Update(env_base)
env_base["platform"] = selected_platform

modules_detected = OrderedDict()
module_search_paths = ["modules"]

if env_base["custom_modules"] != "":
    paths = env_base["custom_modules"].split(",")
    for p in paths:
        try:
            module_search_paths.append(methods.convert_custom_modules_path(p))
        except ValueError as e:
            print(e)
            Exit(255)

for path in module_search_paths:
    if path == "modules":
        modules = methods.detect_modules(path, recursive=False)
    else:
        modules = methods.detect_modules(path, env_base["custom_modules_recursive"])
        env_base.Prepend(CPPPATH=[path, os.path.dirname(path)])
    modules_detected.update(modules)

for name, path in modules_detected.items():
    sys.path.insert(0, path)
    import config

    if env_base["modules_enabled_by_default"]:
        enabled = True
        try:
            enabled = config.is_enabled()
        except AttributeError:
            pass
    else:
        enabled = False

    opts.Add(BoolVariable("module_" + name + "_enabled", "Enable module '%s'" % (name,), enabled))

    try:
        for opt in config.get_opts(selected_platform):
            opts.Add(opt)
    except AttributeError:
        pass

    sys.path.remove(path)
    sys.modules.pop("config")

methods.write_modules(modules_detected)

# Update the environment again after all the module options are added.
opts.Update(env_base)
env_base["platform"] = selected_platform  # Must always be re-set after calling opts.Update().
Help(opts.GenerateHelpText(env_base))

# add default include paths

env_base.Prepend(CPPPATH=["#"])

# Configuration of build targets:
# - Editor or template
# - Debug features (DEBUG_ENABLED code)
# - Dev only code (DEV_ENABLED code)
# - Optimization level
# - Debug symbols for crash traces / debuggers

env_base.dev_build = env_base["dev_build"]
env_base.debug_features = env_base["target"] == "debug"

if env_base.dev_build:
    opt_level = "none"
elif env_base.debug_features:
    opt_level = "speed_trace"
else:  # Release
    opt_level = "speed"

env_base["optimize"] = ARGUMENTS.get("optimize", opt_level)
env_base["debug_symbols"] = methods.get_cmdline_bool("debug_symbols", env_base.dev_build)

if env_base.debug_features:
    # DEBUG_ENABLED enables debugging *features* and debug-only code, which is intended
    # to give *users* extra debugging information for their game development.
    env_base.Append(CPPDEFINES=["DEBUG_ENABLED"])

if env_base.dev_build:
    # DEV_ENABLED enables *engine developer* code which should only be compiled for those
    # working on the engine itself.
    env_base.Append(CPPDEFINES=["DEV_ENABLED"])
else:
    # Disable assert() for production targets (only used in thirdparty code).
    env_base.Append(CPPDEFINES=["NDEBUG"])

if env_base["precision"] == "double":
    env_base.Append(CPPDEFINES=["REAL_T_IS_DOUBLE"])

if selected_platform in platform_list:
    tmppath = "./platform/" + selected_platform
    sys.path.insert(0, tmppath)
    import detect

    env = env_base.Clone()

    # Default num_jobs to local cpu count if not user specified.
    # SCons has a peculiarity where user-specified options won't be overridden
    # by SetOption, so we can rely on this to know if we should use our default.
    initial_num_jobs = env.GetOption("num_jobs")
    altered_num_jobs = initial_num_jobs + 1
    env.SetOption("num_jobs", altered_num_jobs)
    if env.GetOption("num_jobs") == altered_num_jobs:
        cpu_count = os.cpu_count()
        if cpu_count is None:
            print("Couldn't auto-detect CPU count to configure build parallelism. Specify it with the -j argument.")
        else:
            safer_cpu_count = cpu_count if cpu_count <= 4 else cpu_count - 1
            print(
                "Auto-detected %d CPU cores available for build parallelism. Using %d cores by default. You can override it with the -j argument."
                % (cpu_count, safer_cpu_count)
            )
            env.SetOption("num_jobs", safer_cpu_count)

    env.extra_suffix = ""

    if env["extra_suffix"] != "":
        env.extra_suffix += "." + env["extra_suffix"]

    # Environment flags
    CCFLAGS = env.get("CCFLAGS", "")
    env["CCFLAGS"] = ""
    env.Append(CCFLAGS=str(CCFLAGS).split())

    CFLAGS = env.get("CFLAGS", "")
    env["CFLAGS"] = ""
    env.Append(CFLAGS=str(CFLAGS).split())

    LINKFLAGS = env.get("LINKFLAGS", "")
    env["LINKFLAGS"] = ""
    env.Append(LINKFLAGS=str(LINKFLAGS).split())

    # Platform specific flags.
    # These can sometimes override default options.
    flag_list = platform_flags[selected_platform]
    for f in flag_list:
        if not (f[0] in ARGUMENTS) or ARGUMENTS[f[0]] == "auto":  # Allow command line to override platform flags
            env[f[0]] = f[1]

    # 'dev_mode' and 'production' are aliases to set default options if they haven't been
    # set manually by the user.
    # These need to be checked *after* platform specific flags so that different
    # default values can be set (e.g. to keep LTO off for `production` on some platforms).
    if env["dev_mode"]:
        env["verbose"] = methods.get_cmdline_bool("verbose", True)
        env["warnings"] = ARGUMENTS.get("warnings", "extra")
        env["werror"] = methods.get_cmdline_bool("werror", True)
        env["tests"] = methods.get_cmdline_bool("tests", True)
    if env["production"]:
        env["use_static_c"] = methods.get_cmdline_bool("use_static_c", True)
        env["debug_symbols"] = methods.get_cmdline_bool("debug_symbols", False)
        # LTO "auto" means we handle the preferred option in each platform detect.py.
        env["lto"] = ARGUMENTS.get("lto", "auto")

    # Run SCU file generation script if in a SCU build.
    if env["scu_build"]:
        max_includes_per_scu = 8
        if env_base.dev_build == True:
            max_includes_per_scu = 1024

        read_scu_limit = int(env["scu_limit"])
        read_scu_limit = max(0, min(read_scu_limit, 1024))
        if read_scu_limit != 0:
            max_includes_per_scu = read_scu_limit

        methods.set_scu_folders(scu_builders.generate_scu_files(max_includes_per_scu))

    # Must happen after the flags' definition, as configure is when most flags
    # are actually handled to change compile options, etc.
    detect.configure(env)

    print(f'Building for platform "{selected_platform}", architecture "{env["arch"]}", target "{env["target"]}".')
    if env.dev_build:
        print("NOTE: Developer build, with debug optimization level and debug symbols (unless overridden).")

    # Set optimize and debug_symbols flags.
    # "custom" means do nothing and let users set their own optimization flags.
    # Needs to happen after configure to have `env.msvc` defined.
    if env.msvc:
        if env["debug_symbols"]:
            env.Append(CCFLAGS=["/Zi", "/FS"])
            env.Append(LINKFLAGS=["/DEBUG:FULL"])
        else:
            env.Append(LINKFLAGS=["/DEBUG:NONE"])

        if env["optimize"] == "speed":
            env.Append(CCFLAGS=["/O2"])
            env.Append(LINKFLAGS=["/OPT:REF"])
        elif env["optimize"] == "speed_trace":
            env.Append(CCFLAGS=["/O2"])
            env.Append(LINKFLAGS=["/OPT:REF", "/OPT:NOICF"])
        elif env["optimize"] == "size":
            env.Append(CCFLAGS=["/O1"])
            env.Append(LINKFLAGS=["/OPT:REF"])
        elif env["optimize"] == "debug" or env["optimize"] == "none":
            env.Append(CCFLAGS=["/Od"])
    else:
        if env["debug_symbols"]:
            # Adding dwarf-4 explicitly makes stacktraces work with clang builds,
            # otherwise addr2line doesn't understand them
            env.Append(CCFLAGS=["-gdwarf-4"])
            if env.dev_build:
                env.Append(CCFLAGS=["-g3"])
            else:
                env.Append(CCFLAGS=["-g2"])
        else:
            if methods.using_clang(env) and not methods.is_vanilla_clang(env):
                # Apple Clang, its linker doesn't like -s.
                env.Append(LINKFLAGS=["-Wl,-S", "-Wl,-x", "-Wl,-dead_strip"])
            else:
                env.Append(LINKFLAGS=["-s"])

        if env["optimize"] == "speed":
            env.Append(CCFLAGS=["-O3"])
        # `-O2` is friendlier to debuggers than `-O3`, leading to better crash backtraces.
        elif env["optimize"] == "speed_trace":
            env.Append(CCFLAGS=["-O2"])
        elif env["optimize"] == "size":
            env.Append(CCFLAGS=["-Os"])
        elif env["optimize"] == "debug":
            env.Append(CCFLAGS=["-Og"])
        elif env["optimize"] == "none":
            env.Append(CCFLAGS=["-O0"])

    # Needs to happen after configure to handle "auto".
    if env["lto"] != "none":
        print("Using LTO: " + env["lto"])

    if not env.msvc:
        env.Prepend(CFLAGS=["-std=c17"])
    else:
        env.Prepend(CCFLAGS=["/std:c17"])

    # Enforce our minimal compiler version requirements
    cc_version = methods.get_compiler_version(env) or {
        "major": None,
        "minor": None,
        "patch": None,
        "metadata1": None,
        "metadata2": None,
        "date": None,
    }
    cc_version_major = int(cc_version["major"] or -1)
    cc_version_minor = int(cc_version["minor"] or -1)
    cc_version_metadata1 = cc_version["metadata1"] or ""

    if methods.using_gcc(env):
        if cc_version_major == -1:
            print(
                "Couldn't detect compiler version, skipping version checks. "
                "Build may fail if the compiler doesn't support C++17 fully."
            )
    elif methods.using_clang(env):
        if cc_version_major == -1:
            print(
                "Couldn't detect compiler version, skipping version checks. "
                "Build may fail if the compiler doesn't support C++17 fully."
            )

    if env["disable_exceptions"]:
        if env.msvc:
            env.Append(CPPDEFINES=[("_HAS_EXCEPTIONS", 0)])
        else:
            env.Append(CXXFLAGS=["-fno-exceptions"])
    elif env.msvc:
        env.Append(CXXFLAGS=["/EHsc"])

    # Configure compiler warnings
    if env.msvc:  # MSVC
        if env["warnings"] == "none":
            env.Append(CCFLAGS=["/w"])
        else:
            if env["warnings"] == "extra":
                env.Append(CCFLAGS=["/W4"])
            elif env["warnings"] == "all":
                # C4458 is like -Wshadow. Part of /W4 but let's apply it for the default /W3 too.
                env.Append(CCFLAGS=["/W3", "/w34458"])
            elif env["warnings"] == "moderate":
                env.Append(CCFLAGS=["/W2"])
            # Disable warnings which we don't plan to fix.

            env.Append(
                CCFLAGS=[]
            )

        if env["werror"]:
            env.Append(CCFLAGS=["/WX"])
            env.Append(LINKFLAGS=["/WX"])
    else:  # GCC, Clang
        common_warnings = []

        if methods.using_gcc(env):
            common_warnings += ["-Wshadow", "-Wno-misleading-indentation"]
            if cc_version_major == 7:  # Bogus warning fixed in 8+.
                common_warnings += ["-Wno-strict-overflow"]
        elif methods.using_clang(env):
            common_warnings += ["-Wshadow-field-in-constructor", "-Wshadow-uncaptured-local"]
            # We often implement `operator<` for structs of pointers as a requirement
            # for putting them in `Set` or `Map`. We don't mind about unreliable ordering.
            common_warnings += ["-Wno-ordered-compare-function-pointers"]

        if env["warnings"] == "extra":
            env.Append(CCFLAGS=["-Wall", "-Wextra", "-Wwrite-strings", "-Wno-unused-parameter"] + common_warnings)
            env.Append(CXXFLAGS=["-Wctor-dtor-privacy", "-Wnon-virtual-dtor"])
            if methods.using_gcc(env):
                env.Append(
                    CCFLAGS=[
                        "-Walloc-zero",
                        "-Wduplicated-branches",
                        "-Wduplicated-cond",
                        "-Wstringop-overflow=4",
                    ]
                )
                env.Append(CXXFLAGS=["-Wplacement-new=1"])
                # Need to fix a warning with AudioServer lambdas before enabling.
                # if cc_version_major != 9:  # GCC 9 had a regression (GH-36325).
                #    env.Append(CXXFLAGS=["-Wnoexcept"])
                if cc_version_major >= 9:
                    env.Append(CCFLAGS=["-Wattribute-alias=2"])
                if cc_version_major >= 11:  # Broke on MethodBind templates before GCC 11.
                    env.Append(CCFLAGS=["-Wlogical-op"])
            elif methods.using_clang(env):
                env.Append(CCFLAGS=["-Wimplicit-fallthrough"])
        elif env["warnings"] == "all":
            env.Append(CCFLAGS=["-Wall"] + common_warnings)
        elif env["warnings"] == "moderate":
            env.Append(CCFLAGS=["-Wall", "-Wno-unused"] + common_warnings)
        else:  # 'no'
            env.Append(CCFLAGS=["-w"])

        if env["werror"]:
            env.Append(CCFLAGS=["-Werror"])

    if hasattr(detect, "get_program_suffix"):
        suffix = "." + detect.get_program_suffix()
    else:
        suffix = "." + selected_platform

    suffix += "." + env["target"]
    if env.dev_build:
        suffix += ".dev"

    if env_base["precision"] == "double":
        suffix += ".double"

    suffix += "." + env["arch"]
    suffix += env.extra_suffix

    sys.path.remove(tmppath)
    sys.modules.pop("detect")

    modules_enabled = OrderedDict()
    env.module_dependencies = {}

    for name, path in modules_detected.items():
        if not env["module_" + name + "_enabled"]:
            continue
        sys.path.insert(0, path)
        env.current_module = name
        import config

        if config.can_build(env, selected_platform):
            # Disable it if a required dependency is missing.
            if not env.module_check_dependencies(name):
                continue

            config.configure(env)
            modules_enabled[name] = path

        sys.path.remove(path)
        sys.modules.pop("config")

    env.module_list = modules_enabled
    methods.sort_module_list(env)

    methods.generate_version_header(env.module_version_string)

    env["PROGSUFFIX_WRAP"] = suffix + env.module_version_string + ".console" + env["PROGSUFFIX"]
    env["PROGSUFFIX"] = suffix + env.module_version_string + env["PROGSUFFIX"]
    env["OBJSUFFIX"] = suffix + env["OBJSUFFIX"]

    if os.name == "nt":
        env["LIBSUFFIXES"] += [env["LIBSUFFIX"]]
    else:
        env["LIBSUFFIXES"] += [env["LIBSUFFIX"], env["SHLIBSUFFIX"]]
    env["LIBSUFFIX"] = suffix + env["LIBSUFFIX"]
    env["SHLIBSUFFIX"] = suffix + env["SHLIBSUFFIX"]

    env["OBJPREFIX"] = env["object_prefix"]
    env["SHOBJPREFIX"] = env["object_prefix"]

    if not env["verbose"]:
        methods.no_verbose(sys, env)

    scons_cache_path = os.environ.get("SCONS_CACHE")
    if scons_cache_path != None:
        CacheDir(scons_cache_path)
        print("Scons cache enabled... (path: '" + scons_cache_path + "')")

    if env["compiledb"]:
        # Generating the compilation DB (`compile_commands.json`) requires SCons 4.0.0 or later.
        from SCons import __version__ as scons_raw_version

        scons_ver = env._get_major_minor_revision(scons_raw_version)

        if scons_ver < (4, 0, 0):
            print("The `compiledb=yes` option requires SCons 4.0 or later, but your version is %s." % scons_raw_version)
            Exit(255)

        env.Tool("compilation_db")
        env.Alias("compiledb", env.CompilationDatabase())

    Export("env")

    # Build subdirs, the build order is dependent on link order.
    SConscript("renderer/SCsub")
    SConscript("modules/SCsub")
    SConscript("core/SCsub")
    SConscript("platform/SCsub")
    if env["tests"]:
        SConscript("tests/SCsub")
    SConscript("main/SCsub")

    SConscript("platform/" + selected_platform + "/SCsub")  # Build selected platform.

    # Check for the existence of headers
    conf = Configure(env)
    if "check_c_headers" in env:
        headers = env["check_c_headers"]
        for header in headers:
            if conf.CheckCHeader(header):
                env.AppendUnique(CPPDEFINES=[headers[header]])

elif selected_platform != "":
    if selected_platform == "list":
        print("The following platforms are available:\n")
    else:
        print('Invalid target platform "' + selected_platform + '".')
        print("The following platforms were detected:\n")

    for x in platform_list:
        print("\t" + x)

    print("\nPlease run SCons again and select a valid platform: platform=<string>")

    if selected_platform == "list":
        # Exit early to suppress the rest of the built-in SCons messages
        Exit()
    else:
        Exit(255)

# The following only makes sense when the 'env' is defined, and assumes it is.
if "env" in locals():
    # FIXME: This method mixes both cosmetic progress stuff and cache handling...
    methods.show_progress(env)
    # TODO: replace this with `env.Dump(format="json")`
    # once we start requiring SCons 4.0 as min version.
    methods.dump(env)


def print_elapsed_time():
    elapsed_time_sec = round(time.time() - time_at_start, 3)
    time_ms = round((elapsed_time_sec % 1) * 1000)
    print("[Time elapsed: {}.{:03}]".format(time.strftime("%H:%M:%S", time.gmtime(elapsed_time_sec)), time_ms))


atexit.register(print_elapsed_time)
