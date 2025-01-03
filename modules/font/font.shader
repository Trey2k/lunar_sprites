#LS opengl_vertex
#version 300 es

layout (location = 0) in vec2 position;
layout (location = 1) in vec2 tex_coord;
layout (location = 2) in vec4 in_color;
out vec2 frag_tex_coord;
out float frag_tex_id;
out vec4 frag_color;

void main() {
    frag_color = in_color;
    gl_Position = vec4(position, 0.0, 1.0);
    frag_tex_coord = tex_coord;
}

#LS opengl_fragment
#version 300 es

precision mediump float;

out vec4 frag_color_out;
in vec4 frag_color;
in vec2 frag_tex_coord;

uniform sampler2D u_texture;

void main() {
	frag_color_out = texture(u_texture, frag_tex_coord) * frag_color;
}