#include "renderer/opengl/shader.h"
#include "renderer/opengl/debug.h"
#include "renderer/opengl/renderer_interface.h"

#include <glad/gl.h>

struct OpenGLShader {
	GLuint program;
};

OpenGLShader *opengl_shader_create(const char *vertex_source, const char *fragment_source) {
	GL_CALL(GLuint vertex_shader = glCreateShader(GL_VERTEX_SHADER));
	GL_CALL(glShaderSource(vertex_shader, 1, &vertex_source, NULL));
	GL_CALL(glCompileShader(vertex_shader));

	GLint isCompiled = 0;
	GL_CALL(glGetShaderiv(vertex_shader, GL_COMPILE_STATUS, &isCompiled));
	if (isCompiled == GL_FALSE) {
		GLint maxLength = 0;
		GL_CALL(glGetShaderiv(vertex_shader, GL_INFO_LOG_LENGTH, &maxLength));

		// The maxLength includes the NULL character
		char *errorLog = ls_malloc(maxLength);
		GL_CALL(glGetShaderInfoLog(vertex_shader, maxLength, &maxLength, errorLog));
		ls_log(LOG_LEVEL_ERROR, "Vertex Shader error: %s\n", errorLog);
		ls_free(errorLog);

		GL_CALL(glDeleteShader(vertex_shader));
		return 0;
	}

	// Repeat the process for the fragment shader
	GL_CALL(uint32 fragment_shader = glCreateShader(GL_FRAGMENT_SHADER));
	GL_CALL(glShaderSource(fragment_shader, 1, &fragment_source, NULL));
	GL_CALL(glCompileShader(fragment_shader));

	glGetShaderiv(fragment_shader, GL_COMPILE_STATUS, &isCompiled);
	if (isCompiled == GL_FALSE) {
		GLint maxLength = 0;
		GL_CALL(glGetShaderiv(fragment_shader, GL_INFO_LOG_LENGTH, &maxLength));

		char *errorLog = ls_malloc(maxLength);
		GL_CALL(glGetShaderInfoLog(fragment_shader, maxLength, &maxLength, errorLog));
		ls_log(LOG_LEVEL_ERROR, "Fragment Shader error: %s\n", errorLog);
		ls_free(errorLog);

		GL_CALL(glDeleteShader(fragment_shader));
		GL_CALL(glDeleteShader(vertex_shader));
		return 0;
	}

	OpenGLShader *shader = ls_malloc(sizeof(OpenGLShader));

	GL_CALL(shader->program = glCreateProgram());
	GL_CALL(glAttachShader(shader->program, vertex_shader));
	GL_CALL(glAttachShader(shader->program, fragment_shader));
	GL_CALL(glLinkProgram(shader->program));

	GLint isLinked = 0;
	glGetProgramiv(shader->program, GL_LINK_STATUS, (int *)&isLinked);
	if (isLinked == GL_FALSE) {
		GLint maxLength = 0;
		GL_CALL(glGetProgramiv(shader->program, GL_INFO_LOG_LENGTH, &maxLength));

		char *errorLog = ls_malloc(maxLength);
		GL_CALL(glGetProgramInfoLog(shader->program, maxLength, &maxLength, errorLog));
		ls_log(LOG_LEVEL_ERROR, "Shader Program link error: %s\n", errorLog);
		ls_free(errorLog);
		GL_CALL(glDeleteShader(vertex_shader));
		GL_CALL(glDeleteShader(fragment_shader));

		opengl_shader_destroy(shader);
		return 0;
	}

	GL_CALL(glDeleteShader(vertex_shader));
	GL_CALL(glDeleteShader(fragment_shader));

	return shader;
}

void opengl_shader_destroy(OpenGLShader *shader) {
	GL_CALL(glDeleteProgram(shader->program));
	ls_free(shader);
}

void opengl_shader_bind(const OpenGLShader *shader) {
	GL_CALL(glUseProgram(shader->program));
}

void opengl_shader_unbind(const OpenGLShader *shader) {
	GL_CALL(glUseProgram(0));
}

void opengl_shader_set_uniform_int(const OpenGLShader *shader, String name, int32 value) {
	GL_CALL(glUniform1i(glGetUniformLocation(shader->program, name), value));
}

void opengl_shader_set_uniform_float(const OpenGLShader *shader, String name, float value) {
	GL_CALL(glUniform1f(glGetUniformLocation(shader->program, name), value));
}

void opengl_shader_set_uniform_vec2(const OpenGLShader *shader, String name, Vector2 value) {
	GL_CALL(glUniform2f(glGetUniformLocation(shader->program, name), value.x, value.y));
}

void opengl_shader_set_uniform_vec3(const OpenGLShader *shader, String name, Vector3 value) {
	GL_CALL(glUniform3f(glGetUniformLocation(shader->program, name), value.x, value.y, value.z));
}

int32 opengl_shader_get_uniform_location(const OpenGLShader *shader, String name) {
	return glGetUniformLocation(shader->program, name);
}

uint32 opengl_shader_get_attrib_location(const OpenGLShader *shader, String name) {
	return glGetAttribLocation(shader->program, name);
}