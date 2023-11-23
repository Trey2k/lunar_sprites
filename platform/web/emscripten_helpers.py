import os, json

from SCons.Util import WhereIs


def run_closure_compiler(target, source, env, for_signature):
    closure_bin = os.path.join(os.path.dirname(WhereIs("emcc")), "node_modules", ".bin", "google-closure-compiler")
    cmd = [WhereIs("node"), closure_bin]
    cmd.extend(["--compilation_level", "ADVANCED_OPTIMIZATIONS"])
    for f in env["JSEXTERNS"]:
        cmd.extend(["--externs", f.get_abspath()])
    for f in source:
        cmd.extend(["--js", f.get_abspath()])
    cmd.extend(["--js_output_file", target[0].get_abspath()])
    return " ".join(cmd)


def get_build_version():
    import version

    name = "custom_build"
    if os.getenv("BUILD_NAME") != None:
        name = os.getenv("BUILD_NAME")
    v = "%d.%d" % (version.major, version.minor)
    if version.patch > 0:
        v += ".%d" % version.patch
    status = version.status
    if os.getenv("LUNAR_VERSION_STATUS") != None:
        status = str(os.getenv("LUNAR_VERSION_STATUS"))
    v += ".%s.%s" % (status, name)
    return v


def create_engine_file(env, target, source, externs):
    if env["use_closure_compiler"]:
        return env.BuildJS(target, source, JSEXTERNS=externs)
    return env.Textfile(target, [env.File(s) for s in source])


def create_template_zip(env, js, wasm, worker, side):
    binary_name = "lunar_sprites.release" if env["target"] == "release" else "lunar_sprites.debug"
    zip_dir = env.Dir("#bin/.web_zip")
    in_files = [
        js,
        wasm,
        worker,
    ]

    out_files = [
        zip_dir.File(binary_name + ".js"),
        zip_dir.File(binary_name + ".wasm"),
        zip_dir.File(binary_name + ".worker.js"),
    ]

    dynamic_modules = ""

    # Dynamic linking
    if env["dlink_enabled"]:
        in_files.append(side) # Side wasm contains the lunar_sprites runtime
        out_files.append(zip_dir.File(binary_name + ".side.wasm"))

        for module in env.dynamic_modules:
            in_files.append("#/bin/%s" % module)
            out_files.append(zip_dir.File(module))
            dynamic_modules += '"%s", ' % module
        
        if len(env.dynamic_modules) > 0:
            dynamic_modules = dynamic_modules[:-2]

    subst_dict = {
        "@LUNAR_VERSION@": get_build_version(),
        "@LUNAR_NAME@": "Lunar Sprites",
        "@LUNAR_URL@": "%s.js" % binary_name,
        "@LUNAR_BASE_PATH@": binary_name,
        "@LUNAR_DYNAMIC_MODULES@": dynamic_modules,
    }

    index = "#misc/dist/html/index.html"
    index = env.Substfile(target="#bin/lunar_sprites${PROGSUFFIX}.html", source=index, SUBST_DICT=subst_dict)
    # HTML
    in_files.append(index)
    out_files.append(zip_dir.File("index.html"))

    zip_files = env.InstallAs(out_files, in_files)
    env.Zip(
        "#bin/lunar_sprites",
        zip_files,
        ZIPROOT=zip_dir,
        ZIPSUFFIX="${PROGSUFFIX}${ZIPSUFFIX}",
        ZIPCOMSTR="Archiving $SOURCES as $TARGET",
    )


def add_js_libraries(env, libraries):
    env.Append(JS_LIBS=env.File(libraries))


def add_js_pre(env, js_pre):
    env.Append(JS_PRE=env.File(js_pre))


def add_js_externs(env, externs):
    env.Append(JS_EXTERNS=env.File(externs))