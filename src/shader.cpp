#include "shader.h"

GLuint Shader::id()
{
	return _id;
}

Shader::Shader(const char* v_source, const char* f_source)
{
	GLint success;
	GLchar log[512];

	GLuint v_shader;
	v_shader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(v_shader, 1, &v_source, 0);
	glCompileShader(v_shader);

#ifdef _DEBUG
	glGetShaderiv(v_shader, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(v_shader, 512, 0, log);
		std::cout << "Vertex shader compilation failed! Log:\n" << log << std::endl;
		_id = 0;
		return;
	}
#endif
	
	GLuint f_shader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(f_shader, 1, &f_source, 0);
	glCompileShader(f_shader);

#ifdef _DEBUG
	glGetShaderiv(f_shader, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(f_shader, 512, 0, log);
		std::cout << "Fragment shader compilation failed! Log:\n" << log << std::endl;
		_id = 0;
		return;
	}
#endif

	_id = glCreateProgram();
	glAttachShader(_id, v_shader);
	glAttachShader(_id, f_shader);
	glLinkProgram(_id);

#ifdef _DEBUG
	glGetProgramiv(_id, GL_LINK_STATUS, &success);
	if(!success) {
		glGetProgramInfoLog(_id, 512, 0, log);
		std::cout << "Shader program linking failed! Log:\n" << log << std::endl;
		_id = 0;
		return;
	}
#endif
	glDeleteShader(v_shader);
	glDeleteShader(f_shader);

}

Shader::~Shader()
{
	glDeleteProgram(_id);
}