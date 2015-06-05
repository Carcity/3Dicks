#ifndef RENDER_H
#define RENDER_H

#include <gl/glew.h>
#include <gl/GL.h>
#include "glm\glm\glm.hpp"
#include "glm\glm\gtc\matrix_transform.hpp"
#include "Shader.h"
#include "obj.h"
#include "object.h"
#include "SpotLight.h"
#include "ShadowMapFBO.h"
#include "UserInput.h"
#define BUFFER_OFFSET(i) ((char *)nullptr + (i))
#define GLM_FORCE_RADIANS

class Render
{
private:
	int view = 0;
	void setLights();
public:
	GLuint gVertexAttribute = 0;

	GLuint gShaderProgram = 0;
	GLuint gShaderProgramShadow = 0;
	Shader* shader;
	Shader* shaderShadow;

	UserInput* in;
	//Global matrixes
	glm::mat4 viewMatrix;// = glm::lookAt(glm::vec3(0, 0, -2), glm::vec3(0, 0, -1), glm::vec3(0, 1, 0));
	glm::mat4 projMatrix = glm::perspective(3.1415f*0.45f, 640.f / 480.0f, 0.1f, 1000.0f);

	Render();
	~Render();
	int render();
	void CreateShaders();

	void ShadowMapPass();
	void RenderPass();

	void clear();
	//game objects
	Object* obj = 0;
	Object* obj2 = 0;

	ShadowMapFBO* shadowMap;
	SpotLight* spotLights;
	int nrSpotLights;

	float rottemp = 0;

};

#endif