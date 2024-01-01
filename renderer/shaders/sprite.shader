#LS opengl_vertex
#version 300 es

in vec3 a_position;
in vec2 a_tex_coord;
out vec2 tex_coord;

uniform mat4 u_transform;
uniform mat4 u_projection;
uniform mat4 u_view;

void main() {
	tex_coord = a_tex_coord;
	gl_Position = u_projection * u_view * u_transform * vec4(a_position, 1.0);
}

#LS opengl_fragment
#version 300 es

precision mediump float;

in vec2 tex_coord;
out vec4 frag_color;

uniform sampler2D u_texture;

void main() {
	
	frag_color = texture(u_texture, tex_coord);
}