

def make_shader_source(shader, shader_var_name, out_source_file):
    bytes_per_line = 16
    shader_bytes = None

    with open(shader, "rb") as f:
        shader_bytes = f.read()

    out_source = "const char *const %s = (const char[%d]){\n\t" % (shader_var_name, len(shader_bytes) + 1)

    for i, b in enumerate(shader_bytes):
        out_source += str(b) + ', '
        if i % bytes_per_line == 0 and i != 0:
            out_source += '\n\t'
    
    out_source += '0\n};\n'

    with open(out_source_file, "w") as f:
        f.write(out_source)
    

