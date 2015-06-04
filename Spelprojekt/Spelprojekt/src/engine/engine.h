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
	void Engine::CompileErrorPrint(GLuint* shader);
	void Engine::LinkErrorPrint(GLuint* shaderProgram);
	int cameraSwap = 0;
	int cameraSwapCounter = 0;
	// </temp>

	GLuint frustumProgram;

	CameraControl* cam = 0;

	void renderFrustum(QuadTree* qt);

public:
	Engine(){};
	~Engine();
	void init(CameraControl* cc);

	void render(const Map* map, const ContentManager* content, const AnimationManager* anim);
};

#endif