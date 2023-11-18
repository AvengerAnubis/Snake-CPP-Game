#pragma once

#define GLEW_STATIC
#include <gl/glew.h>

#include <iostream>

class Shader
{
private:
	GLuint _id;
public:
	Shader(const char* v_source, const char* f_source);
	~Shader();
	GLuint id();
};