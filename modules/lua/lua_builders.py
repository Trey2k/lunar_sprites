from enum import Enum

    
STATE = Enum("STATE", ["NORMAL", "IN_ENUM", "IN_DEFINE"])

def contstants_normal_state(remove_prefix, last_state, line, info):
    if "typedef enum" in line:
        return STATE.IN_ENUM
    elif "define" in line:
        return STATE.IN_DEFINE
    return last_state

def constants_in_enum_state(remove_prefix, last_state, line, info):
    if "}" in line:
        type_name = line.replace("}", "").replace(" ", "").replace(";", "").replace("\n", "").replace("\r", "")
        info['type_name'] = type_name
        return STATE.NORMAL

    line = line.replace("\t", "")

    if line.startswith("/*") or line.startswith("//") or line == "\n":
        return last_state
    
    if "=" in line:
        value_name = line.split("=")[0].replace(" ", "")
        name = value_name.removeprefix(remove_prefix)
        info['values'].append({'value': value_name, 'name': name, 'type': 'integer'})
        return last_state
    else:
        value_name = line.replace(" ", "").replace(",", "").replace("\n", "").replace("\r", "")
        name = value_name.removeprefix(remove_prefix)
        info['values'].append({'value': value_name, 'name': name, 'type': 'integer'})

    return last_state


def constants_in_define_state(remove_prefix, last_state, line, info):
    # skip if multi line
    if "\\\n" in line or len(line.split(" "))< 3:
        return last_state
    
    name = line.split(" ")[1]
    value_name = name.removeprefix(remove_prefix)
    info['type_name'] = name
    value = line.split(" ")[2].replace("\n", "").replace("\r", "")

    if value.startswith("\""):
        info['values'].append({'value': value_name, 'name': name, 'type': 'string'})
    elif value.endswith("f") or value.endswith("F") or "." in value:
        info['values'].append({'value': value_name, 'name': name, 'type': 'number'})
    else:
        info['values'].append({'value': value_name, 'name': name, 'type': 'integer'})

    return STATE.NORMAL

def make_lua_constants_source(remove_prefix, head_files, method_name, out_source_file):
    includes_str = ""
    for path in head_files:
        includes_str += "#include \"%s\"\n" % path.replace("../", "")
    
    out_source = """/* THIS FILE IS GENERATED DO NOT EDIT */
#include "core/core.h"
%s
#include <lauxlib.h>
#include <lua.h>
#include <lualib.h>

void %s(lua_State *L, int32 table_index) {
""" % (includes_str, method_name)
    
    constants = {}

    for path in head_files:
        current_state = STATE.NORMAL
        info = {
            'type_name': "",
            'values': []
        }

        with open(path, "rb") as f:
            new_state = current_state
            for line in f:
                line = line.decode("utf-8")
                match current_state:
                    case STATE.NORMAL:
                        if info['type_name'] != "":
                            constants[info['type_name']] = info['values']
                            info = {
                                'type_name': "",
                                'values': []
                            }

                        new_state = contstants_normal_state(remove_prefix, current_state, line, info)
                    case STATE.IN_ENUM:
                        new_state = constants_in_enum_state(remove_prefix, current_state, line, info)
                
                if new_state == STATE.IN_DEFINE:
                    new_state = constants_in_define_state(remove_prefix, current_state, line, info)
                
                current_state = new_state
                    
    

    for type_name, values in constants.items():
        out_source += "\t// %s\n" % type_name
        for value in values:
            if value['type'] == "" or value['name'] == "":
                continue
            out_source +=  "\tlua_push%s(L, %s);\n\tlua_setfield(L, table_index, \"%s\");\n" % (value['type'], value['value'], value['name'])
        out_source += "\n"
    
    out_source = out_source[:-1]
    out_source += "}\n"
    with open(out_source_file, "w") as f:
        f.write(out_source)