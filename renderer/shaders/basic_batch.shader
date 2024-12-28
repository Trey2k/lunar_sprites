 #LS opengl_vertex
#version 300 es

layout (location = 0) in vec3 position;
layout (location = 1) in float tex_id;
layout (location = 2) in vec2 tex_coord;
layout (location = 3) in vec4 in_color;
out vec2 frag_tex_coord;
out float frag_tex_id;
out vec4 frag_color;

void main() {
    frag_color = in_color;
    gl_Position = vec4(position, 1.0);
    frag_tex_coord = tex_coord;
    frag_tex_id = tex_id;
}

#LS opengl_fragment
#version 300 es

precision mediump float;

out vec4 frag_color_out;
in vec4 frag_color;
in float frag_tex_id;
in vec2 frag_tex_coord;

uniform sampler2D[16] u_texture;

void main() {
    int index = int(frag_tex_id);
	frag_color_out = texture(u_texture[index], frag_tex_coord) * frag_color;
}