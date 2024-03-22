#LS opengl_vertex
#version 300 es

in vec3 a_position;
in vec4 a_color;

out vec4 v_color;

void main() {
    gl_Position = vec4(a_position, 1.0);
    v_color = a_color;
}

#LS opengl_fragment
#version 300 es

precision mediump float;

in vec4 v_color;
out vec4 frag_color;

void main() {
    frag_color = v_color;
}