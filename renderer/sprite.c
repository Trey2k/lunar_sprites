#include "renderer/sprite.h"

#include "renderer/buffers.h"
#include "renderer/camera.h"
#include "renderer/shader.h"
#include "renderer/texture.h"
#include "renderer/vertex_array.h"

extern const char *const SPRITE_SHADER_SOURCE;

static const float32 SPRITE_VERTICIES[] = {
	// positions          // texture coords
	1.0f, 1.0f, 0.0f, 1.0f, 1.0f, // top right
	1.0f, -1.0f, 0.0f, 1.0f, 0.0f, // bottom right
	-1.0f, -1.0f, 0.0f, 0.0f, 0.0f, // bottom left
	-1.0f, 1.0f, 0.0f, 0.0f, 1.0f // top left
};

#define LAYOUT_COUNT 2
static const BufferElement SPRITE_BUFFER_LAYOUT[] = {
	{ SHADER_DATA_TYPE_FLOAT3, "pos", false },
	{ SHADER_DATA_TYPE_FLOAT2, "tex_coord", false }
};

static const uint32 SPRITE_INDECIES[] = {
	0, 1, 3, // first triangle
	1, 2, 3 // second triangle
};

struct Sprite {
	Vector2 size;
	Matrix4 transform;

	Texture *texture;
	Shader *shader;
	VertexArray *vao;

	const Renderer *renderer;

	Vector2i viewport_size;
};

Sprite *renderer_create_sprite(const Renderer *renderer, String image_path, Vector2 position, Vector2 scale, float32 rotation) {
	Sprite *sprite = ls_malloc(sizeof(Sprite));
	sprite->renderer = renderer;
	sprite->texture = texture_create(image_path);

	sprite->size = vec2(texture_get_width(sprite->texture), texture_get_height(sprite->texture));
	sprite->size = vec2(sprite->size.x * scale.x, sprite->size.y * scale.y);

	sprite->transform = mat4_translate(vec3(position.x, position.y, 0.0));
	sprite->transform = mat4_multiply(sprite->transform, mat4_rotate(rotation, vec3(0.0, 0.0, 1.0)));
	sprite->transform = mat4_multiply(sprite->transform, mat4_scale(vec3(scale.x, scale.y, 1.0)));

	VertexBuffer *vbo = renderer_create_vertex_buffer(renderer, SPRITE_VERTICIES, sizeof(SPRITE_VERTICIES));
	BufferLayout *layout = buffer_layout_create(SPRITE_BUFFER_LAYOUT, LAYOUT_COUNT);
	vertex_buffer_set_layout(vbo, layout);

	IndexBuffer *ibo = renderer_create_index_buffer(renderer, SPRITE_INDECIES, sizeof(SPRITE_INDECIES));

	sprite->vao = renderer_create_vertex_array(renderer);
	vertex_arrray_add_vertex_buffer(sprite->vao, vbo);
	vertex_array_set_index_buffer(sprite->vao, ibo);

	sprite->shader = renderer_create_shader(renderer, SPRITE_SHADER_SOURCE, ls_str_length(SPRITE_SHADER_SOURCE));

	return sprite;
}

void sprite_destroy(Sprite *sprite) {
	texture_destroy(sprite->texture);
	shader_destroy(sprite->shader);
	vertex_array_destroy(sprite->vao);

	ls_free(sprite);
}

void sprite_draw(const Sprite *sprite) {
	texture_bind(sprite->texture, 0);

	vertex_array_bind(sprite->vao);
	shader_bind(sprite->shader);

	const Camera *camera = get_current_camera();
	if (camera) {
		shader_set_uniform_mat4(sprite->shader, "u_view", camera_get_view_matrix(camera));
		shader_set_uniform_mat4(sprite->shader, "u_projection", camera_get_projection_matrix(camera));
	} else {
		shader_set_uniform_mat4(sprite->shader, "u_view", MAT4_IDENTITY);
		shader_set_uniform_mat4(sprite->shader, "u_projection", MAT4_IDENTITY);
	}

	shader_set_uniform_mat4(sprite->shader, "u_transform", sprite->transform);

	vertex_array_draw(sprite->vao);

	shader_unbind(sprite->shader);
	vertex_array_unbind(sprite->vao);
}

void sprite_set_position(Sprite *sprite, Vector2 position) {
	sprite->transform = mat4_translate(vec3(position.x, position.y, 0.0));
}

Vector2 sprite_get_position(const Sprite *sprite) {
	return vec2(sprite->transform.w0, sprite->transform.w1);
}

void sprite_set_scale(Sprite *sprite, Vector2 scale) {
	sprite->transform = mat4_scale(vec3(sprite->size.x * scale.x, sprite->size.y * scale.y, 1.0));
}

Vector2 sprite_get_scale(const Sprite *sprite) {
	return vec2(sprite->transform.x0, sprite->transform.y1);
}

void sprite_set_rotation(Sprite *sprite, float32 rotation) {
	sprite->transform = mat4_multiply(sprite->transform, mat4_rotate(rotation, vec3(0.0, 0.0, 1.0)));
}

float32 sprite_get_rotation(const Sprite *sprite) {
	return sprite->transform.z2;
}