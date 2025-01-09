#LS opengl_vertex
#version 300 es

layout (location = 0) in vec3 position;
layout (location = 1) in vec2 tex_coord;
layout (location = 2) in vec4 in_color;
layout (location = 3) in vec2 element_size;
layout (location = 4) in float radius;
layout (location = 5) in float tex_id;

out vec2 frag_tex_coord;
out vec4 frag_color;
out vec2 frag_element_size;
out float frag_radius;
out float frag_tex_id;

void main() {
    frag_color = in_color;
    frag_tex_coord = tex_coord;
    frag_element_size = element_size;
    frag_radius = radius;
    frag_tex_id = tex_id;

    gl_Position = vec4(position, 1.0);
}

#LS opengl_fragment
#version 300 es

precision mediump float;

out vec4 frag_color_out;
in vec4 frag_color;
in vec2 frag_tex_coord;
in vec2 frag_element_size;
in float frag_radius;
in float frag_tex_id;

uniform sampler2D u_textures[16];
uniform vec2 u_resolution;

float round_box_sdf(vec2 p, vec2 b, float r) {
    vec2 q = abs(p) - (b - vec2(r));
    return length(max(q, 0.0)) - r;
}

void main() {
    vec4 bg_color = frag_color;
    vec2 pixel_pos = frag_tex_coord * frag_element_size - frag_element_size / 2.0;

    // Pass the full element size to the SDF, but adjust internally for rounding
    float dist = round_box_sdf(pixel_pos, frag_element_size / 2.0, frag_radius);

    // Discard fragments outside the rounded box
    if (dist > 0.0) {
        bg_color = vec4(0.0);
    }

    int tex_id = int(frag_tex_id);

    if (tex_id == -1) {
        frag_color_out = bg_color;
    } else if (tex_id == 0) {
        frag_color_out = texture(u_textures[0], frag_tex_coord) * bg_color;
    } else if (tex_id == 1) {
        frag_color_out = texture(u_textures[1], frag_tex_coord) * bg_color;
    } else if (tex_id == 2) {
        frag_color_out = texture(u_textures[2], frag_tex_coord) * bg_color;
    } else if (tex_id == 3) {
        frag_color_out = texture(u_textures[3], frag_tex_coord) * bg_color;
    } else if (tex_id == 4) {
        frag_color_out = texture(u_textures[4], frag_tex_coord) * bg_color;
    } else if (tex_id == 5) {
        frag_color_out = texture(u_textures[5], frag_tex_coord) * bg_color;
    } else if (tex_id == 6) {
        frag_color_out = texture(u_textures[6], frag_tex_coord) * bg_color;
    } else if (tex_id == 7) {
        frag_color_out = texture(u_textures[7], frag_tex_coord) * bg_color;
    } else if (tex_id == 8) {
        frag_color_out = texture(u_textures[8], frag_tex_coord) * bg_color;
    } else if (tex_id == 9) {
        frag_color_out = texture(u_textures[9], frag_tex_coord) * bg_color;
    } else if (tex_id == 10) {
        frag_color_out = texture(u_textures[10], frag_tex_coord) * bg_color;
    } else if (tex_id == 11) {
        frag_color_out = texture(u_textures[11], frag_tex_coord) * bg_color;
    } else if (tex_id == 12) {
        frag_color_out = texture(u_textures[12], frag_tex_coord) * bg_color;
    } else if (tex_id == 13) {
        frag_color_out = texture(u_textures[13], frag_tex_coord) * bg_color;
    } else if (tex_id == 14) {
        frag_color_out = texture(u_textures[14], frag_tex_coord) * bg_color;
    } else if (tex_id == 15) {
        frag_color_out = texture(u_textures[15], frag_tex_coord) * bg_color;
    }
}