import os
import sys
import re
import glob
import subprocess
from collections import OrderedDict
from collections.abc import Mapping
from typing import Iterator
from pathlib import Path
from os.path import normpath, basename

# Get the root directory of the project
base_folder_path = str(os.path.abspath(Path(__file__).parent)) + "/"
base_folder_only = os.path.basename(os.path.normpath(base_folder_path))

_scu_folders = set()

def set_scu_folders(scu_folders):
    global _scu_folders
    _scu_folders = scu_folders


def add_source_files_orig(env, sources, files, allow_gen=False):
    if isinstance(files, (str, bytes)):
        if files.startswith("#"):
            if "*" in files:
                print("ERROR: Wildcards can't be expanded in SCons project-absolute path: '{}'".format(files))
                return
            files = [files]
        else:
            # Exclude .gen.c files from globbing, to avoid including old ones.
            skip_gen = "*" in files
            dir_path = env.Dir(".").abspath
            files = sorted(glob.glob(dir_path + "/" + files))
            if skip_gen and not allow_gen:
                files = [f for f in files if not f.endswith(".gen.c")]


    for path in files:
        obj = env.Object(path)
        if obj in sources:
            print("WARNING: Duplicate source file: '{}'".format(path))
            continue
        sources.append(obj)


def _find_scu_section_name(subdir):
    section_path = os.path.abspath(subdir) + "/"

    folders = []
    folder = ""

    for i in range(8):
        folder = os.path.dirname(section_path)
        folder = os.path.basename(folder)
        if folder == base_folder_only:
            break
        folders += [folder]
        section_path += "../"
        section_path = os.path.abspath(section_path) + "/"

    section_name = ""
    for n in range(len(folders)):
        section_name += folders[len(folders) - n - 1]
        if n != (len(folders) - 1):
            section_name += "/"

    return section_name

def add_source_files_scu(env, sources, files):
    if not env["scu_build"] or not isinstance(files, str):
        return False

    if "*." not in files:
        return False

    subdir = os.path.dirname(files)
    if subdir != "":
        subdir += "/"

    section_name = _find_scu_section_name(subdir)
    global _scu_folders
    if section_name not in (_scu_folders):
        return False

    # Add all the .gen.c files in the SCU directory
    add_source_files_orig(env, sources, subdir + "scu/scu_*.gen.c", True)


def add_source_files(env, sources, files, allow_gen=False):
    if not add_source_files_scu(env, sources, files):
        add_source_files_orig(env, sources, files, allow_gen)
        return False
    return True


def disable_warnings(env):
    if env.msvc:
        env["CCFLAGS"] = [ x for x in env["CCFLAGS"] if not (x.startswith("/W") or x.startswith("/w"))]
        env["CFLAGS"] = [ x for x in env["CFLAGS"] if not (x.startswith("/W") or x.startswith("/w"))]
        env.AppendUnique(CCFLAGS = ["/w"])
    else:
        env.AppendUnique(CCFLAGS = ["-w"])


def force_optimization_on_debug(env):
    if env["target"] == "release":
        return

    if env.msvc:
        env["CCFLAGS"] = [x for x in env["CCFLAGS"] if not x.startswith("/O")]
        env["CFLAGS"] = [x for x in env["CFLAGS"] if not x.startswith("/O")]
        env.AppendUnique(CCFLAGS = ["/O2"])
    else:
        env.AppendUnique(CCFLAGS = ["-O3"])


def add_module_version_string(env, s):
    env.module_version_string += "." + s


def get_version_info(module_version_string="", silent=False):
    build_name = "custom_build"
    if os.getenv("BUILD_NAME") != None:
        build_name = str(os.getenv("BUILD_NAME"))
        if not silent:
            print("Using custom build name: %s" % build_name)

    import version
    version_info = {
        "short_name": str(version.short_name),
        "name": str(version.name),
        "major": str(version.major),
        "minor": str(version.minor),
        "patch": str(version.patch),
        "status": str(version.status),
        "build": str(build_name),
        "module_config": str(version.module_config) + module_version_string,
        "year": int(version.year),
        "website": str(version.website),
    }

    if os.getenv("LUNAR_VERSION_STATUS") != None:
        version_info["status"] = str(os.getenv("LUNAR_VERSION_STATUS"))
        if not silent:
            print("Using custom version status: %s" % version_info["status"])


    githash = ""
    gitfolder = ".git"

    if os.path.isfile(".git"):
        module_folder = open(".git", "r").readline().strip()
        if module_folder.startswith("gitdir: "):
            gitfolder = module_folder[8:]

    if os.path.isfile(os.path.join(gitfolder, "HEAD")):
        head = open(os.path.join(gitfolder, "HEAD"), "r", encoding="utf8").readline().strip()
        if head.startswith("ref: "):
            ref = head[5:]

            parts = gitfolder.split("/")
            if len(parts) > 2 and parts[-2] == "worktrees":
                gitfolder = "/".join(parts[0:-2])
            head = os.path.join(gitfolder, ref)
            packedrefs = os.path.join(gitfolder, "packed-refs")
            if os.path.isfile(head):
                githash = open(head, "r").readline().strip()
            elif os.path.isfile(packedrefs):
                for line in open(packedrefs, "r").read().splitlines():
                    if line.startswith("#"):
                        continue
                    (line_hash, line_ref) = line.split(" ")
                    if ref == line_ref:
                        githash = line_hash
                        break

        else:
            githash = head

    version_info["githash"] = githash

    return version_info


def generate_version_header(module_version_string=""):
    version_info = get_version_info(module_version_string)


    f = open("core/version_info.gen.h", "w")
    f.write("""
/* THIS FILE IS GENERATED DO NOT EDIT */
#ifndef VERSION_INFO_GEN_H
#define VERSION_INFO_GEN_H

#define VERSION_SHORT_NAME "{short_name}"
#define VERSION_NAME "{name}"
#define VERSION_MAJOR {major}
#define VERSION_MINOR {minor}
#define VERSION_PATCH {patch}
#define VERSION_STATUS "{status}"
#define VERSION_BUILD "{build}"
#define VERSION_MODULE_CONFIG "{module_config}"
#define VERSION_YEAR {year}
#define VERSION_WEBSITE "{website}"

#endif // VERSION_INFO_GEN_H
    """.format(**version_info))
    f.close()

    fhash = open("core/version_info.gen.c", "w")
    fhash.write("""
/* THIS FILE IS GENERATED DO NOT EDIT */
#include "core/version.h"
const char *const VERSION_HASH = "{githash}";
    """.format(**version_info))
    fhash.close()


def get_cmdline_bool(option, default):
    from SCons.Script import ARGUMENTS
    from SCons.Variables.BoolVariable import _text2bool

    cmdline_val = ARGUMENTS.get(option)
    if cmdline_val is None:
        return default

    return _text2bool(cmdline_val)


def detect_modules(search_path, recursive=False):
    """Detects and collects a list of C modules at the given path."""
    modules = OrderedDict()

    def add_module(path):
        module_name = os.path.basename(path)
        module_path = path.replace("\\", "/")
        modules[module_name] = module_path

    def is_core(path):
        version_path = os.path.join(path, "version.py")
        if os.path.exists(version_path):
            with open(version_path) as f:
                if 'short_name = "lunar_sprites"' in f.read():
                    return True
        return False

    def get_files(path):
        files = glob.glob(os.path.join(path, "*"))

        files.sort()
        return files

    if not recursive:
        if is_module(search_path):
            add_module(search_path)
        for path in get_files(search_path):
            if is_core(path):
                continue
            if is_module(path):
                add_module(path)
    else:
        to_search = [search_path]
        while to_search:
            path = to_search.pop()
            if is_module(path):
                add_module(path)
            for child in get_files(path):
                if not os.path.isdir(child):
                    continue
                if is_core(child):
                    continue
                to_search.insert(0, child)
    return modules


def is_module(path):
    if not os.path.isdir(path):
        return False
    must_exist = ["register_types.h", "SCsub", "config.py"]
    for f in must_exist:
        if not os.path.exists(os.path.join(path, f)):
            return False
    return True

def write_modules(modules):
    includes_c = ""
    initialize_c = ""
    uninitialize_c = ""

    for name, path in modules.items():
        try:
            with open(os.path.join(path, "register_types.h")):
                includes_c += '#include "' + path + '/register_types.h"\n'
                initialize_c += "#ifdef MODULE_" + name.upper() + "_ENABLED\n"
                initialize_c += "\tinitialize_" + name + "_module(p_level);\n"
                initialize_c += "#endif\n"
                uninitialize_c += "#ifdef MODULE_" + name.upper() + "_ENABLED\n"
                uninitialize_c += "\tuninitialize_" + name + "_module(p_level);\n"
                uninitialize_c += "#endif\n"
        except OSError:
            pass

    modules_c = """
/* THIS FILE IS GENERATED DO NOT EDIT */
#include "register_module_types.h"

#include "modules/modules_enabled.gen.h"

%s

void initialize_modules(ModuleInitializationLevel p_level) {
%s
}

void uninitialize_modules(ModuleInitializationLevel p_level) {
%s
}
""" % (
        includes_c,
        initialize_c,
        uninitialize_c,
    )

    with open("modules/register_module_types.gen.c", "w") as f:
        f.write(modules_c)


def convert_custom_module_path(path):
    if not path:
        return path

    path = os.path.realpath(os.path.expanduser(os.path.expandvars(path)))
    err_msg = "Build option 'custom_modules' must %s"
    if not os.path.isdir(path):
        raise ValueError(err_msg % "be a directory.")
    if path == os.path.realpath("modules"):
        raise ValueError(err_msg % "be a directory other than built-in `modules` directory.")
    return path


def disable_module(env):
    env.disabled_modules.append(env.current_module)


def module_add_dependencies(env, module, dependencies, optional=False):
    """
    Add dependencies to the given module.
    """
    if module not in env.module_dependencies:
        env.module_dependencies[module] = [[], []]
    if optional:
        env.module_dependencies[module][1].extend(dependencies)
    else:
        env.module_dependencies[module][0].extend(dependencies)


def module_check_dependencies(env, module):
    """
    Check if the given module has all its dependencies satisfied.
    """
    missing_deps = []
    required_deps = env.module_dependencies[module][0] if module in env.module_dependencies else []
    for dep in required_deps:
        opt = "module_{}_enabled".format(dep)
        if not opt in env or not env[opt]:
            missing_deps.append(dep)

    if missing_deps != []:
        print(
            "Disabling '{}' module as the following dependencies are not satisfied: {}".format(
                module, ", ".join(missing_deps)
            )
        )
        return False
    else:
        return True

def sort_module_list(env):
    out = OrderedDict()
    deps = {k: v[0] + list(filter(lambda x: x in env.module_list, v[1])) for k, v in env.module_dependencies.items()}

    frontier = list(env.module_list.keys())
    explored = []
    while len(frontier):
        cur = frontier.pop()
        deps_list = deps[cur] if cur in deps else []
        if len(deps_list) and any([d not in explored for d in deps_list]):
            frontier.insert(0, cur)
            continue
        explored.append(cur)
    for k in explored:
        env.module_list.move_to_end(k)


def use_windows_spawn_fix(env, platform=None):
    if os.name != "nt":
        return  # not needed, only for windows

    # On Windows, due to the limited command line length, when creating a static library
    # from a very high number of objects SCons will invoke "ar" once per object file;
    # that makes object files with same names to be overwritten so the last wins and
    # the library loses symbols defined by overwritten objects.
    # By enabling quick append instead of the default mode (replacing), libraries will
    # got built correctly regardless the invocation strategy.
    # Furthermore, since SCons will rebuild the library from scratch when an object file
    # changes, no multiple versions of the same object file will be present.
    env.Replace(ARFLAGS="q")

    def mySubProcess(cmdline, env):
        startupinfo = subprocess.STARTUPINFO()
        startupinfo.dwFlags |= subprocess.STARTF_USESHOWWINDOW
        popen_args = {
            "stdin": subprocess.PIPE,
            "stdout": subprocess.PIPE,
            "stderr": subprocess.PIPE,
            "startupinfo": startupinfo,
            "shell": False,
            "env": env,
        }
        if sys.version_info >= (3, 7, 0):
            popen_args["text"] = True
        proc = subprocess.Popen(cmdline, **popen_args)
        _, err = proc.communicate()
        rv = proc.wait()
        if rv:
            print("=====")
            print(err)
            print("=====")
        return rv

    def mySpawn(sh, escape, cmd, args, env):
        newargs = " ".join(args[1:])
        cmdline = cmd + " " + newargs

        rv = 0
        env = {str(key): str(value) for key, value in iter(env.items())}
        if len(cmdline) > 32000 and cmd.endswith("ar"):
            cmdline = cmd + " " + args[1] + " " + args[2] + " "
            for i in range(3, len(args)):
                rv = mySubProcess(cmdline + args[i], env)
                if rv:
                    break
        else:
            rv = mySubProcess(cmdline, env)

        return rv

    env["SPAWN"] = mySpawn


def no_verbose(sys, env):
    colors = {}

    if sys.stdout.isatty():
        colors["blue"] = "\033[94m"
        colors["bold_blue"] = "\033[1;94m"
        colors["reset"] = "\033[0m"
    else:
        colors["blue"] = ""
        colors["bold_blue"] = ""
        colors["reset"] = ""

    compile_source_message = "{}Compiling {}$SOURCE{} ...{}".format(
        colors["bold_blue"], colors["blue"], colors["bold_blue"], colors["reset"]
    )
    compile_shared_source_message = "{}Compiling shared {}$SOURCE{} ...{}".format(
        colors["bold_blue"], colors["blue"], colors["bold_blue"], colors["reset"]
    )
    link_program_message = "{}Linking Program {}$TARGET{} ...{}".format(
        colors["bold_blue"], colors["blue"], colors["bold_blue"], colors["reset"]
    )
    link_library_message = "{}Linking Static Library {}$TARGET{} ...{}".format(
        colors["bold_blue"], colors["blue"], colors["bold_blue"], colors["reset"]
    )
    ranlib_library_message = "{}Ranlib Library {}$TARGET{} ...{}".format(
        colors["bold_blue"], colors["blue"], colors["bold_blue"], colors["reset"]
    )
    link_shared_library_message = "{}Linking Shared Library {}$TARGET{} ...{}".format(
        colors["bold_blue"], colors["blue"], colors["bold_blue"], colors["reset"]
    )

    env.Append(CXXCOMSTR=[compile_source_message])
    env.Append(CCCOMSTR=[compile_source_message])
    env.Append(SHCCCOMSTR=[compile_shared_source_message])
    env.Append(SHCXXCOMSTR=[compile_shared_source_message])
    env.Append(ARCOMSTR=[link_library_message])
    env.Append(RANLIBCOMSTR=[ranlib_library_message])
    env.Append(SHLINKCOMSTR=[link_shared_library_message])
    env.Append(LINKCOMSTR=[link_program_message])


def glob_recursive(pattern, node="."):
    from SCons import node
    from SCons.Script import glob

    results = []
    for f in Glob(str(node) + "/*", source=True):
        if type(f) is Node.FS.Dir:
            results += glob_recursive(pattern, f)

    results += Glob(str(node) + "/" + pattern, source=True)
    return results


def precious_program(env, program, sources, **args):
    program = env.ProgramOriginal(program, sources, **args)
    env.Precious(program)
    return program


def add_shared_library(env, name, sources, **args):
    library = env.SharedLibrary(name, sources, **args)
    env.NoCache(library)
    return library


def add_library(env, name, sources, **args):
    library = env.Library(name, sources, **args)
    env.NoCache(library)
    return library


def add_program(env, name, sources, **args):
    program = env.Program(name, sources, **args)
    env.NoCache(program)
    return program


def CommandNoCache(env, target, sources, command, **args):
    result = env.Command(target, sources, command, **args)
    env.NoCache(result)
    return result


def Run(env, function, short_message, subprocess=True):
    from SCons.Script import Action
    from platform_methods import run_in_subprocess

    output_print = short_message if not env["verbose"] else ""
    if not subprocess:
        return Action(function, output_print)
    else:
        return Action(run_in_subprocess(function), output_print)


def is_vanilla_clang(env):
    if not using_clang(env):
        return False
    try:
        version = subprocess.check_output([env.subst(env["CXX"]), "--version"]).strip().decode("utf-8")
    except (subprocess.CalledProcessError, OSError):
        print("Couldn't parse CXX environment variable to infer compiler version.")
        return False
    return not version.startswith("Apple")


def get_compiler_version(env):
    """
    Returns a dictionary with various version information:

    - major, minor, patch: Version following semantic versioning system
    - metadata1, metadata2: Extra information
    - date: Date of the build
    """
    ret = {
        "major": -1,
        "minor": -1,
        "patch": -1,
        "metadata1": None,
        "metadata2": None,
        "date": None,
    }

    if not env.msvc:
        # Not using -dumpversion as some GCC distros only return major, and
        # Clang used to return hardcoded 4.2.1: # https://reviews.llvm.org/D56803
        try:
            version = subprocess.check_output([env.subst(env["CXX"]), "--version"]).strip().decode("utf-8")
        except (subprocess.CalledProcessError, OSError):
            print("Couldn't parse CXX environment variable to infer compiler version.")
            return ret
    else:
        # TODO: Implement for MSVC
        return ret
    match = re.search(
        r"(?:(?<=version )|(?<=\) )|(?<=^))"
        r"(?P<major>\d+)"
        r"(?:\.(?P<minor>\d*))?"
        r"(?:\.(?P<patch>\d*))?"
        r"(?:-(?P<metadata1>[0-9a-zA-Z-]*))?"
        r"(?:\+(?P<metadata2>[0-9a-zA-Z-]*))?"
        r"(?: (?P<date>[0-9]{8}|[0-9]{6})(?![0-9a-zA-Z]))?",
        version,
    )
    if match is not None:
        for key, value in match.groupdict().items():
            if value is not None:
                ret[key] = value
    # Transform semantic versioning to integers
    for key in ["major", "minor", "patch"]:
        ret[key] = int(ret[key] or -1)
    return ret


def using_gcc(env):
    return "gcc" in os.path.basename(env["CC"])


def using_clang(env):
    return "clang" in os.path.basename(env["CC"])


def show_progress(env):
    import sys
    from SCons.Script import Progress, Command, AlwaysBuild

    screen = sys.stdout
    # Progress reporting is not available in non-TTY environments since it
    # messes with the output (for example, when writing to a file)
    show_progress = env["progress"] and sys.stdout.isatty()
    node_count = 0
    node_count_max = 0
    node_count_interval = 1
    node_count_fname = str(env.Dir("#")) + "/.scons_node_count"

    import time, math

    class cache_progress:
        # The default is 1 GB cache and 12 hours half life
        def __init__(self, path=None, limit=1073741824, half_life=43200):
            self.path = path
            self.limit = limit
            self.exponent_scale = math.log(2) / half_life
            if env["verbose"] and path != None:
                screen.write(
                    "Current cache limit is {} (used: {})\n".format(
                        self.convert_size(limit), self.convert_size(self.get_size(path))
                    )
                )
            self.delete(self.file_list())

        def __call__(self, node, *args, **kw):
            nonlocal node_count, node_count_max, node_count_interval, node_count_fname, show_progress
            if show_progress:
                # Print the progress percentage
                node_count += node_count_interval
                if node_count_max > 0 and node_count <= node_count_max:
                    screen.write("\r[%3d%%] " % (node_count * 100 / node_count_max))
                    screen.flush()
                elif node_count_max > 0 and node_count > node_count_max:
                    screen.write("\r[100%] ")
                    screen.flush()
                else:
                    screen.write("\r[Initial build] ")
                    screen.flush()

        def delete(self, files):
            if len(files) == 0:
                return
            if env["verbose"]:
                # Utter something
                screen.write("\rPurging %d %s from cache...\n" % (len(files), len(files) > 1 and "files" or "file"))
            [os.remove(f) for f in files]

        def file_list(self):
            if self.path is None:
                # Nothing to do
                return []
            # Gather a list of (filename, (size, atime)) within the
            # cache directory
            file_stat = [(x, os.stat(x)[6:8]) for x in glob.glob(os.path.join(self.path, "*", "*"))]
            if file_stat == []:
                # Nothing to do
                return []
            # Weight the cache files by size (assumed to be roughly
            # proportional to the recompilation time) times an exponential
            # decay since the ctime, and return a list with the entries
            # (filename, size, weight).
            current_time = time.time()
            file_stat = [(x[0], x[1][0], (current_time - x[1][1])) for x in file_stat]
            # Sort by the most recently accessed files (most sensible to keep) first
            file_stat.sort(key=lambda x: x[2])
            # Search for the first entry where the storage limit is
            # reached
            sum, mark = 0, None
            for i, x in enumerate(file_stat):
                sum += x[1]
                if sum > self.limit:
                    mark = i
                    break
            if mark is None:
                return []
            else:
                return [x[0] for x in file_stat[mark:]]

        def convert_size(self, size_bytes):
            if size_bytes == 0:
                return "0 bytes"
            size_name = ("bytes", "KB", "MB", "GB", "TB", "PB", "EB", "ZB", "YB")
            i = int(math.floor(math.log(size_bytes, 1024)))
            p = math.pow(1024, i)
            s = round(size_bytes / p, 2)
            return "%s %s" % (int(s) if i == 0 else s, size_name[i])

        def get_size(self, start_path="."):
            total_size = 0
            for dirpath, dirnames, filenames in os.walk(start_path):
                for f in filenames:
                    fp = os.path.join(dirpath, f)
                    total_size += os.path.getsize(fp)
            return total_size

    def progress_finish(target, source, env):
        nonlocal node_count, progressor
        try:
            with open(node_count_fname, "w") as f:
                f.write("%d\n" % node_count)
            progressor.delete(progressor.file_list())
        except Exception:
            pass

    try:
        with open(node_count_fname) as f:
            node_count_max = int(f.readline())
    except Exception:
        pass

    cache_directory = os.environ.get("SCONS_CACHE")
    # Simple cache pruning, attached to SCons' progress callback. Trim the
    # cache directory to a size not larger than cache_limit.
    cache_limit = float(os.getenv("SCONS_CACHE_LIMIT", 1024)) * 1024 * 1024
    progressor = cache_progress(cache_directory, cache_limit)
    Progress(progressor, interval=node_count_interval)

    progress_finish_command = Command("progress_finish", [], progress_finish)
    AlwaysBuild(progress_finish_command)


def dump(env):
    # Dumps latest build information for debugging purposes and external tools.
    from json import dump

    def non_serializable(obj):
        return "<<non-serializable: %s>>" % (type(obj).__qualname__)

    with open(".scons_env.json", "w") as f:
        dump(env.Dictionary(), f, indent=4, default=non_serializable)
