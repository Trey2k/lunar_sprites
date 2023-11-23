#!/usr/bin/env python
 
import os
from enum import Enum
 
state = Enum('State', ['Normal', 'Comment', 'MultiComment', 'Macro', 'Typedef', 'Export'])
 
# List of headers to include in the API header
# The order of the headers is important
source_headers = [
    'core/version_info.gen.h',
    'core/version.h',
    'core/types/typedefs.h',
    'core/types/string.h',
    'core/log.h',
    'core/memory.h',
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
    'main/lunar_sprites.h',

    'modules/initialize_modules.h',
 
    'modules/dynamic_modules/dynamic_modules.h',
]
 
def is_line_macro_start(line):
    return ((line.startswith('#define') or
                line.startswith('#if') or
                line.startswith('#ifdef') or
                line.startswith('#ifndef')
            ) and not 
        line.endswith('_H\n') and not 
        "OPENGL_ENABLED" in line)
 
def is_line_complex_macro_start(line):
    return ((line.startswith('#if') or
                line.startswith('#ifdef') or
                line.startswith('#ifndef')
            ) and not 
        line.endswith('_H\n') and not 
        "OPENGL_ENABLED" in line)
 
 
def is_line_export(line):
    return line.startswith('LS_EXPORT') or line.startswith('_FORCE_INLINE_')
 
 
def header_normal_state(last_state, line, info):
    new_state = state.Normal
    if line.startswith('/*'):
        info['comment'] = line
        if not line.replace(' ', '').endswith('*/\n'):
            new_state = state.MultiComment
        else:
            new_state = state.Comment
        
    elif line.startswith('//'):
        info['comment'] = line
        new_state = state.Comment
    elif is_line_macro_start(line):
        if is_line_complex_macro_start(line):
            info['depth'] += 1
        
        if last_state == state.Comment or last_state == state.MultiComment:
            info['macros'] += info['comment']
            info['comment'] = ""
        info['macros'] += line
 
        new_state = state.Macro
    elif line.startswith('typedef'):
        if last_state == state.Comment or last_state == state.MultiComment:
            info['typedefs'] += info['comment']
            info['comment'] = ""
        
        info['typedefs'] += line
        if '{' in line:
            info['depth'] += 1
 
        if '}' in line:
           info['depth'] -= 1
        
        if not ';' in line or info['depth'] != 0:
            new_state = state.Typedef
        else:
            info['typedefs'] += '\n'
        
    elif is_line_export(line):
        if last_state == state.Comment or last_state == state.MultiComment:
            info['exports'] += info['comment']
            info['comment'] = ""
 
        info['exports'] += line
        if '{' in line:
            info['depth'] += 1
        
        if '}' in line:
            info['depth'] -= 1
        
        if not ';' in line or info['depth'] != 0:
            new_state = state.Export
 
 
    return new_state
 
 
def header_export_state(last_state, line, info):
    new_state = state.Export
 
    info['exports'] += line
 
    if '{' in line:
        info['depth'] += 1
    
    if '}' in line:
        info['depth'] -= 1
    
    if (';' in line or '}' in line) and info['depth'] == 0:
        new_state = state.Normal
 
    return new_state
 
 
def header_typedef_state(last_state, line, info):
    new_state = state.Typedef
 
    info['typedefs'] += line
 
    if '{' in line:
        info['depth'] += 1
    
    if '}' in line:
        info['depth'] -= 1
    
    if ';' in line and info['depth'] == 0:
        new_state = state.Normal
        info['typedefs'] += '\n'
 
    return new_state
 
 
def header_macro_state(last_state, line, info):
    new_state = state.Macro
    
    info['macros'] += line
 
    if line.startswith('#endif'):
        info['depth'] -= 1
 
    if not line.endswith('\\\n') and info['depth'] == 0:
        new_state = state.Normal
        info['macros'] += '\n'
 
    if is_line_complex_macro_start(line):
        info['depth'] += 1
 
    return new_state
 
 
def header_multi_comment_state(last_state, line, info):
    info['comment'] += line
 
    if line.endswith('*/'):
        return state.Normal
    
    return state.MultiComment
 
 
def header_comment_state(last_state, line, info):
    if line.startswith('//'):
        info['comment'] += line
        return state.Comment
    
    # Normal comments seek a line further since we do not know if we are at the end until we are on the line past.
    # So if the comment is over we invoke the normal state.
    return header_normal_state(state.Comment, line, info)
 
def get_header_content(header):
    f = open(header, 'r')
    if not f:
        print('ERROR: Could not open header {}'.format(header))
        return
    
    info = {
        'depth': 0,
        'comment': "",
        'macros': "",
        'typedefs': "",
        'exports': ""
    }
          
    current_state = state.Normal
    last_state = state.Normal
 
    for line in f:
        new_state = current_state
        match current_state:
            case state.Normal:
                new_state = header_normal_state(last_state, line, info)
            case state.Comment:
                new_state = header_comment_state(last_state, line, info)
            case state.MultiComment:
                new_state = header_multi_comment_state(last_state, line, info)
            case state.Macro:
                new_state = header_macro_state(last_state, line, info)
            case state.Typedef:
                new_state = header_typedef_state(last_state, line, info)
            case state.Export:
                new_state = header_export_state(last_state, line, info)
        
        
        last_state = current_state
        current_state = new_state
 
    f.close()
    return info['macros'], info['typedefs'], info['exports']
 
 
def make_exports(exports, make_dynamic_module_gen = False):
    exports = exports.replace('LS_EXPORT ', '')
    exports_header_txt = exports
    export_source_txt = "/* THIS FILE IS GENERATED DO NOT EDIT */\n#include \"@API_HEADER@\"\n\n"

    method_typedefs = ""
    api_interface_typedef = "struct LSAPIInterface {\n"
    api_interface_impl = ""
    in_multi_line = False
    api_impl_methods = []
    
    in_inline = False
    depth = 0

    dynamic_module_gen = ""

    for line in exports.splitlines():
        if line.startswith("_FORCE_INLINE_") or in_inline:
            in_inline = True
            if '{' in line:
                depth += 1
            if '}' in line:
                depth -= 1
            
            if (';' in line or '}' in line) and depth == 0:
                in_inline = False
                continue

        elif line.startswith('//'):
            continue
        elif line.startswith('/*') or in_multi_line:
            in_multi_line = True
            if '*/' in line:
                in_multi_line = False
            continue
        elif line.startswith('extern'):
            var = line.replace('extern ', '')
            if "*const " in var:
                var = var.replace('*const ', '*')
            
            word_count = len(var.split(' '))
            var_name = var.split(' ')[word_count - 1].replace(';', '')
            var_type = var.replace(var.split(' ')[word_count - 1], ' ').replace(';', '')

            if '*' in var_name:
                var_name = var_name.replace('*', '')
                var_type = var_type + '*'

            api_interface_typedef += "\t%s_impl_%s;\n" % (var_type, var_name)
            api_interface_impl += "extern %s_impl_%s;\n" % (var_type, var_name)
            export_source_txt += "%s_impl_%s = NULL;\n" % (var_type, var_name)
            api_impl_methods.append(var_name)
            exports_header_txt = exports_header_txt.replace(line, "#define %s_impl_%s" % (var_name, var_name))
        else:
            if len(line.split(' ')) < 2:
                continue
            ret_type = line.split(' ')[0]

            after_type = 1
            if 'static' in ret_type:
                ret_type += ' ' + line.split(' ')[1]
                after_type += 1
            
            if 'const' in ret_type:
                ret_type += ' ' + line.split(' ')[1]
                after_type += 1
            
            if 'struct' in ret_type:
                ret_type += ' ' + line.split(' ')[1]
                after_type += 1
            
            if 'enum' in ret_type:
                ret_type += ' ' + line.split(' ')[1]
                after_type += 1
                
            ret_type += ' '
            method_name = line.split(' ')[after_type].split('(')[0]
            
            if method_name.startswith('*'):
                method_name = method_name[1:]
                ret_type = ret_type + '*'

            params = line.split('(')[1].split(')')[0]

            method_type_name = ""
            for word in method_name.split('_'):
                word = word.capitalize()
                if word == 'Ls':
                    word = 'LS'
                method_type_name += word
            method_type_name += 'Func'

            method_typedefs += "typedef %s(*%s)(%s);\n" % (ret_type, method_type_name, params)
            api_interface_typedef += "\t%s _impl_%s;\n" % (method_type_name, method_name)
            export_source_txt += "%s _impl_%s = NULL;\n" % (method_type_name, method_name)
            api_interface_impl += "extern %s _impl_%s;\n" % (method_type_name, method_name)
            api_impl_methods.append(method_name)

            exports_header_txt = exports_header_txt.replace(line, "#define %s _impl_%s" % (method_name, method_name))
    
    api_interface_typedef += "};\n"
    

    if make_dynamic_module_gen:
        dynamic_module_gen = method_typedefs + "\n"
        dynamic_module_gen += api_interface_typedef + "\n"

    exports_header_prepend = method_typedefs + "\n"
    exports_header_prepend += api_interface_impl + "\n"
    exports_header_txt = exports_header_prepend + exports_header_txt + "\n"

    export_source_txt += "\n" + api_interface_typedef + "\n"
    export_source_txt += "LS_EXPORT void ls_api_init(LSAPIInterface *interface) {\n"
    for method in api_impl_methods:
        export_source_txt += "\t_impl_%s = interface->_impl_%s;\n" % (method, method)
    export_source_txt += "};\n\n\n"

    if make_dynamic_module_gen:
        dynamic_module_gen += """
LSAPIInterface *ls_api_interface = NULL;

void api_interface_init() {
    ls_api_interface = ls_malloc(sizeof(LSAPIInterface));
"""
        for method in api_interface_typedef.splitlines():
            if "LSAPIInterface" in method or "}" in method or method == "":
                continue

            after_type = len(method.split(' ')) - 1
            method_name = method.split(' ')[after_type].replace(';', '')
            if '*' in method_name:
                method_name = method_name.replace('*', '')
            
            dynamic_module_gen += "\tls_api_interface->%s = %s;\n" % (method_name, method_name.replace('_impl_', ''))
        dynamic_module_gen += "}\n"

        return exports_header_txt, export_source_txt, dynamic_module_gen
    return exports_header_txt, export_source_txt


def make_api_header(make_dynamic_module_gen = False):
    api_header_txt = """/* THIS FILE IS GENERATED DO NOT EDIT */
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
#elif defined(__EMSCRIPTEN__)
#define LS_IMPORT
#elif defined(__GNUC__)
#define LS_IMPORT __attribute__((visibility("default")))
#else
#define LS_IMPORT
#endif // defined(_MSC_VER)

#if defined(_MSC_VER)
#define LS_EXPORT __declspec(dllexport)
#elif defined(__EMSCRIPTEN__)
#define LS_EXPORT __attribute__((used))
#elif defined(__GNUC__)
#define LS_EXPORT __attribute__((visibility("default")))
#else // _MSC_VER
#define LS_EXPORT
#endif // _MSC_VER
 
"""
 
    all_macros = ""
    all_typedefs = ""
    all_exports = ""
 
    for header in source_headers:
        header = os.path.join(os.path.dirname(__file__), '../..', header)
        if not os.path.exists(header):
            print('WARNING: Header {} does not exist'.format(header))
            continue
        
        macros, typedefs, exports = get_header_content(header)
        all_macros += macros
        all_typedefs += typedefs
        all_exports += exports
    dynamic_module_gen = ""
    if make_dynamic_module_gen:
        all_exports, api_source_text, dynamic_module_gen = make_exports(all_exports, make_dynamic_module_gen)
    else:
        all_exports, api_source_text = make_exports(all_exports)

    api_header_txt += all_macros
    api_header_txt += all_typedefs
    api_header_txt += all_exports
    api_header_txt += '#endif // LS_API_H\n'
    
    api_header_txt = api_header_txt.replace("\n\n\n", "\n")
    api_source_text = api_source_text.replace("\n\n\n", "\n")
    
    if make_dynamic_module_gen:
        return api_header_txt, api_source_text, dynamic_module_gen
    return api_header_txt, api_source_text
 

def make_dynamic_module_gen():
    api_header_txt, api_source_txt, exports_interface = make_api_header(True)

    inlude_txt = ""
    for header in source_headers:
        inlude_txt += '#include "%s"\n' % header

    dynamic_modules_gen_c = """/* THIS FILE IS GENERATED DO NOT EDIT */
#include "dynamic_modules.h"

%s
""" % inlude_txt

    dynamic_modules_gen_c += exports_interface

    dynamic_modules_gen_c += "const unsigned char *LS_API_HEADER = (const unsigned char[%d]){\n" % (
        len(api_header_txt) + 1)

    bytes_per_line = 16
    dynamic_modules_gen_c += "\t"
    for i, c in enumerate(api_header_txt):
        dynamic_modules_gen_c += str(ord(c)) + ', '
        if i % bytes_per_line == 0 and i != 0:
            dynamic_modules_gen_c += '\n\t'

    dynamic_modules_gen_c += "0\n};\n"

    dynamic_modules_gen_c += "const unsigned char *LS_API_SOURCE = (const unsigned char[%d]){\n" % (
        len(api_source_txt) + 1)
    
    dynamic_modules_gen_c += "\t"
    for i, c in enumerate(api_source_txt):
        dynamic_modules_gen_c += str(ord(c)) + ', '
        if i % bytes_per_line == 0 and i != 0:
            dynamic_modules_gen_c += '\n\t'
    
    dynamic_modules_gen_c += "0\n};\n"

    dynamic_modules_gen_c = dynamic_modules_gen_c.replace("\n\n\n", "\n")

    with open('dynamic_modules.gen.c', 'w') as f:
        f.write(dynamic_modules_gen_c)
 
if __name__ == '__main__':
    header, source = make_api_header()
    source = source.replace('@API_HEADER@', 'ls_api.h')

    with open(os.path.join(os.path.dirname(__file__), 'ls_api.h'), 'w') as f:
        f.write(header)

    with open(os.path.join(os.path.dirname(__file__), 'ls_api.c'), 'w') as f:
        f.write(source)