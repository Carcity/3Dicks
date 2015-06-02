#ifndef ENGINE_H
#define ENGINE_H


#include <GL\glew.h>
#include <GLFW\glfw3.h>
#include <..\glm\glm.hpp>
#include <..\glm\gtc\matrix_transform.hpp>

#include "ContentManager.h"
#include "object.h"
#include"CameraControl.h"
#include "AnimationManager.h"
#include "../game/map.h"

#include "../Align16.h"

class Engine : public Align_16
{
private:
	// <temp>
	GLuint tempshader;
	GLuint uniformModel;
	GLuint uniformVP;
	glm::mat4* viewMatrix;
	glm::mat4 projMatrix;
	void Engine::CompileErrorPrint(GLuint* shader);
	void Engine::LinkErrorPrint(GLuint* shaderProgram);
	int cameraSwap = 0;
	int cameraSwapCounter = 0;
	// </temp>

	CameraControl* cam = 0;

public:
	Engine(){};
	~Engine();
	void init(glm::mat4* viewMatrix);

	void render(const Map* map, const ContentManager* content, const AnimationManager* anim);
};

#endif