#ifndef GAME_H
#define GAME_H

#include "../engine/CameraControl.h"

#include "../engine/engine.h"
#include "../engine/ContentManager.h"
#include "../engine/object.h"
#include "../engine/AnimationManager.h"


#include "map.h"

#include <GLFW/glfw3.h>

class Game
{
private:
	Engine* engine = 0;
	ContentManager* content = 0;
	AnimationManager* animationManager = 0;
	CameraControl* cam = 0;

	Map* map = 0;
	//CameraControl* cam = 0;

	double lastX = 0.0f;
	double lastY = 0.0f;

	mat4* viewMat = 0;

	GLFWwindow* windowRef;

public:
	Game() {};
	~Game();
	void init(GLFWwindow* windowRef);
	void mainLoop();
	void update(float deltaTime);
	void readInput(float deltaTime);
};

#endif