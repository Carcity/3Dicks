#ifndef SHADER_H
#define SHADER_H

#include <gl/glew.h>
#include <gl/GL.h>
#include "glm\glm\glm.hpp"
#include "glm\glm\gtc\matrix_transform.hpp"

#include "SpotLight.h"

class Shader
{
private:
	GLuint* gShaderProgram;
	GLuint ID;
	bool compile();
	//bool compileShader(GLuint shader, char const* path);

public:
	Shader(GLuint* gShaderP, bool isShadowPass);
	bool IsShadowPass;
	Shader();
	Shader(char* path);
	void Uniforms();
	~Shader();
	
	// Regular shader
	GLint model;
	GLint view;
	GLint proj;
	GLint normal;
	GLint textureSample;
	GLint NumSpotLights;
	GLint SpotLights;
	GLint eyepos;

	GLuint lightBuffer, lightBlockUniformLoc;
	GLuint bindingPoint = 1;

	// Lightpass shadow map shader
	GLuint ShadowMap;
	GLuint ViewMatrixSM;
	GLuint ProjectionMatrixSM;

	void CompileErrorPrint(GLuint* shader);
	void LinkErrorPrint(GLuint* gShaderProgram);

	//void Log(GLuint obj);
	//~Shader();
	//bool create(char const* vertex_program_file, char const* fragment_program_file);
	//void bind();
	//void unbind();
};

#endif