#!/usr/bin/env python

import os

source_headers = [
    'modules/native_api/native_api.h',

    'core/api.h',
    'core/version_info.gen.h',
    'core/version.h',
    'core/types/typedefs.h',
    'core/types/string.h',
    'core/log.h',
    'core/memory.h',
    'core/debug.h',
    'core/types/hashtable.h',
    'core/types/vector/vector2.h',
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
]

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

            for line in f:
                # Skip header guards
                if in_macro or (
                    line.startswith('#define') and not 
                    line.endswith('_H\n') and not
                    in_complex_macro):
                
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
                    line.endswith('_H\n')):
                    
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
                    
                elif in_export or line.startswith('LS_EXPORT'):
                    exports += line.replace('LS_EXPORT', 'LS_IMPORT')
                    if ';' not in line:
                        in_export = True
                    else:
                        in_export = False

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


def make_native_api_gen():
    api_header_txt = make_api_header()
    native_api_gen_c = """
/* THIS FILE IS GENERATED DO NOT EDIT */
#include "native_api.h"

const unsigned char *LS_API_HEADER = (const unsigned char[%d]){
""" % (len(api_header_txt) + 1)

    bytes_per_line = 16
    native_api_gen_c += "\t"
    for i, c in enumerate(api_header_txt):
        native_api_gen_c += str(ord(c)) + ', '
        if i % bytes_per_line == 0 and i != 0:
            native_api_gen_c += '\n\t'
    
    native_api_gen_c += "0"
    
    native_api_gen_c += """
};
"""

    with open(os.path.join(os.path.dirname(__file__), 'native_api.gen.c'), 'w') as f:
        f.write(native_api_gen_c)


if __name__ == '__main__':
    with open(os.path.join(os.path.dirname(__file__), 'ls_api.h'), 'w') as f:
        f.write(make_api_header())