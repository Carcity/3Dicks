#include "game.h"
#include <ctime>
//#include <iostream>
//#include <Windows.h>

// opengl debug callback

#ifdef _DEBUG
extern "C"
{
	void APIENTRY openglCallbackFunction(GLenum source,
		GLenum type,
		GLuint id,
		GLenum severity,
		GLsizei length,
		const GLchar* message,
		void* userParam)
	{

		std::cout << "---------------------opengl-callback-start------------" << std::endl;
		std::cout << "message: " << message << std::endl;
		std::cout << "type: ";
		switch (type) {
		case GL_DEBUG_TYPE_ERROR:
			std::cout << "ERROR";
			break;
		case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR:
			std::cout << "DEPRECATED_BEHAVIOR";
			break;
		case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:
			std::cout << "UNDEFINED_BEHAVIOR";
			break;
		case GL_DEBUG_TYPE_PORTABILITY:
			std::cout << "PORTABILITY";
			break;
		case GL_DEBUG_TYPE_PERFORMANCE:
			std::cout << "PERFORMANCE";
			break;
		case GL_DEBUG_TYPE_OTHER:
			std::cout << "OTHER";
			break;
		}
		std::cout << std::endl;

		std::cout << "id: " << id << std::endl;
		std::cout << "severity: ";
		switch (severity){
		case GL_DEBUG_SEVERITY_LOW:
			std::cout << "LOW";
			break;
		case GL_DEBUG_SEVERITY_MEDIUM:
			std::cout << "MEDIUM";
			break;
		case GL_DEBUG_SEVERITY_HIGH:
			std::cout << "HIGH";
			break;
		}
		std::cout << std::endl;
		std::cout << "---------------------opengl-callback-end--------------" << std::endl;
	}

}
#endif


Game::~Game()
{
	if (engine)
		delete engine;
	if (content)
		delete content;
	if (animationManager)
		delete animationManager;
	if (map)
		delete map;
}

void Game::init(GLFWwindow* windowRef)
{
	// register callback
#ifdef _DEBUG
	if (glDebugMessageCallback){
		std::cout << "Register OpenGL debug callback " << std::endl;
		glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
		glDebugMessageCallback((GLDEBUGPROC)openglCallbackFunction, nullptr);
		GLuint unusedIds = 0;
		glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, &unusedIds, true);
	}
	else
		std::cout << "glDebugMessageCallback not available" << std::endl;
#endif

	glm::mat4* viewMat = new glm::mat4(); //deleted in UserInput
	engine = new Engine();
	engine->init(viewMat);
	content = new ContentManager();
	content->init();
	animationManager = new AnimationManager();
	animationManager->init();
	map = new Map();
	map->init();
	//cam = new CameraControl();

	glfwGetCursorPos(windowRef, &lastX, &lastY);

	// do not delete in this class
	this->windowRef = windowRef;
}

void Game::mainLoop()
{
	clock_t start = clock();
	//how much of a second have passed since last frame
	float deltaTime = 0.0f;
	float clock;
	float lastClock = 0.0f;
	int fpsCount = 0;
	glfwSwapInterval(0);
	while (!glfwWindowShouldClose(windowRef))
	{
		glfwPollEvents();

		readInput(deltaTime);
		update(deltaTime);

		glfwSwapBuffers(windowRef);

		fpsCount++;
		clock = (std::clock() - start) / (double)CLOCKS_PER_SEC;
		deltaTime = clock - lastClock;
		lastClock = clock;
		
		if (clock > 1)
		{
			start = std::clock();
			lastClock = 0;
			std::string s = std::to_string(fpsCount);
			fpsCount = 0;
			glfwSetWindowTitle(windowRef, s.c_str());
		}
	}
}

void Game::update(float deltaTime)
{
	//Game code
	//..
	//..

	//animationManager->update();
	//Render const
	engine->render(map, content, animationManager);
}

void Game::readInput(float deltaTime)
{
	//cam->getViewMatrix();
	/*
	int state;
	//Mouse Buttons
	state = glfwGetMouseButton(windowRef, GLFW_MOUSE_BUTTON_RIGHT);
	state == GLFW_PRESS ? in->RMB(true) : in->RMB(false);
	//Character Keys
	state = glfwGetKey(windowRef, GLFW_KEY_W);
	state == GLFW_PRESS ? in->KeyDown('W') : in->KeyUp('W');
	state = glfwGetKey(windowRef, GLFW_KEY_A);
	state == GLFW_PRESS ? in->KeyDown('A') : in->KeyUp('A');
	state = glfwGetKey(windowRef, GLFW_KEY_S);
	state == GLFW_PRESS ? in->KeyDown('S') : in->KeyUp('S');
	state = glfwGetKey(windowRef, GLFW_KEY_D);
	state == GLFW_PRESS ? in->KeyDown('D') : in->KeyUp('D');
	//Special Keys
	state = glfwGetKey(windowRef, GLFW_KEY_LEFT_SHIFT);
	state == GLFW_PRESS ? in->Shift(true) : in->Shift(false);
	state = glfwGetKey(windowRef, GLFW_KEY_SPACE);
	state == GLFW_PRESS ? in->Space(true) : in->Space(false);
	state = glfwGetKey(windowRef, GLFW_KEY_LEFT_CONTROL);
	state == GLFW_PRESS ? in->Ctrl(true) : in->Ctrl(false);
	
	double x, y;
	glfwGetCursorPos(windowRef, &x, &y);
	if(in->updateMouse())
		in->Mouse(x - lastX, y - lastY);
	lastX = x;
	lastY = y;
	
	in->Act(deltaTime);*/
}
