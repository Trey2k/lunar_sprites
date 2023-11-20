#!/usr/bin/env python

import os

# List of headers to include in the API header
# The order of the headers is important
source_headers = [
    'core/api.h',
    'core/version_info.gen.h',
    'core/version.h',
    'core/types/typedefs.h',
    'core/types/string.h',
    'core/log.h',
    'core/memory.h',
    'core/path.h',
    'core/time.h',
    'core/debug.h',
    'core/types/vector/vector2.h',
    'core/types/slice.h',
    'core/types/hashtable.h',
    'core/flags.h',
    'core/window.h',
    'core/input/keycodes.h',
    'core/input/input_manager.h',
    'core/events/events.h',
    'core/events/event_manager.h',
    'core/os/os.h',
    'core/core.h',

    'renderer/window.h',
    'renderer/renderer.h',

    'main/application.h',
    'main/project_settings.h',
    'main/lunar_sprites.h',


    'modules/native_application/native_application.h',
]


# TODO: Clean this up and capture comments
def make_api_header():
    api_header_txt = """
/* THIS FILE IS GENERATED DO NOT EDIT */
#ifndef LS_API_H
#define LS_API_H

#if defined(__linux__) || defined(__FreeBSD__)
#define LINUXBSD_ENABLED
#elif defined(_WIN32) || defined(WIN32)
#define WINDOWS_ENABLED
#elif defined(__EMSCRIPTEN__)
#define WEB_ENABLED
#endif // defined(__linux__) || defined(__FreeBSD__)

#if defined(_MSC_VER)
#define LS_IMPORT __declspec(dllimport)
#elif defined(__GNUC__)
#define LS_IMPORT __attribute__((visibility("default")))
#else
#define LS_IMPORT
#endif // defined(_MSC_VER)

"""

    all_macros = ""
    all_typedefs = ""
    all_exports = ""

    for header in source_headers:
        header = os.path.join(os.path.dirname(__file__), '../..', header)
        if not os.path.exists(header):
            continue
        with open(header, 'r') as f:
            macros = ""
            typedefs = ""
            exports = ""

            in_macro = False

            in_complex_macro = False
            complexy_macro_depth = 0
            complex_macro_start = 0
            complex_macro_discard = False

            in_typedef = False
            typedef_depth = 0

            in_export = False
            export_depth = 0

            for line in f:
                # Skip header guards
                if in_macro or (
                    line.startswith('#define') and not 
                    line.endswith('_H\n') and not
                    in_complex_macro and not
                    "OPENGL_ENABLED" in line):
                
                    macros += line
                    if line.endswith('\\\n'):
                        in_macro = True
                    else:
                        in_macro = False
                        macros += '\n'
                
                elif in_complex_macro or (
                    (line.startswith('#if') or 
                    line.startswith('#ifdef') or 
                    line.startswith('#ifndef')) and not 
                    line.endswith('_H\n') and not
                    "OPENGL_ENABLED" in line):
                    
                    if not in_complex_macro:
                        in_complex_macro = True
                        complex_macro_start = len(macros)

                    macros += line

                    if '#include' in line:
                        complex_macro_discard = True

                    if (line.startswith('#if') or
                        line.startswith('#ifdef') or
                        line.startswith('#ifndef')):
                        complexy_macro_depth += 1
                    
                    elif line.startswith('#endif'):
                        complexy_macro_depth -= 1
                        if complexy_macro_depth == 0:
                            in_complex_macro = False
                            macros += '\n'

                            if complex_macro_discard:
                                macros = macros[:complex_macro_start]
                                complex_macro_discard = False
                    
                elif in_typedef or line.startswith('typedef'):
                    typedefs += line
                    if '{' in line:
                        typedef_depth += 1
                    
                    if not ';' in line:
                            in_typedef = True
                    
                    if '}' in line:
                        typedef_depth -= 1
                    
                    if ';' in line and typedef_depth == 0:
                        in_typedef = False
                        typedefs += '\n'
                    
                elif in_export or line.startswith('LS_EXPORT') or line.startswith('_FORCE_INLINE_'):
                    exports += line.replace('LS_EXPORT', 'LS_IMPORT')
                    if '{' in line:
                        export_depth += 1

                    if ';' not in line:
                        in_export = True
                    
                    if '}' in line:
                        export_depth -= 1
                    
                    if (';' in line or '}' in line) and export_depth == 0:
                        in_export = False
                        exports += '\n'

            if len(exports) > 0:
                exports += '\n'
        
        all_macros += macros
        all_typedefs += typedefs
        all_exports += exports
        
    api_header_txt += all_macros
    api_header_txt += all_typedefs
    api_header_txt += all_exports
    api_header_txt += '#endif // LS_API_H\n'

    return api_header_txt


if __name__ == '__main__':
    with open(os.path.join(os.path.dirname(__file__), 'ls_api.h'), 'w') as f:
        f.write(make_api_header())