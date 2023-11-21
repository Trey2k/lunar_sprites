#!/usr/bin/env python
 
import os
from enum import Enum
 
state = Enum('State', ['Normal', 'Comment', 'MultiComment', 'Macro', 'Typedef', 'Export'])
 
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
    info['exports'] = info['exports'].replace('LS_EXPORT', 'LS_IMPORT')
    return info['macros'], info['typedefs'], info['exports']
 
 
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
            print('WARNING: Header {} does not exist'.format(header))
            continue
        
        macros, typedefs, exports = get_header_content(header)
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