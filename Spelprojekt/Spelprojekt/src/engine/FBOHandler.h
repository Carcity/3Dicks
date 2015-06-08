#ifndef FBOHANDLER_H
#define FBOHANDLER_H
#include <gl/glew.h>
#include <gl/GL.h>
#include <string>
#include <fstream>

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

class FBOHandler
{
	private:
		GLuint color;
		GLuint depth;
		GLuint normals;
		GLuint FBO;
		GLuint worldpos;

		GLuint program;
		std::string getShader(const char* filePath);
	public:
		FBOHandler();
		FBOHandler(unsigned int width, unsigned int height);
		~FBOHandler();

		void generateTexture(unsigned int width, unsigned int height);
		void bind();
		void unbind();

		GLuint getColor();
		GLuint getDepth();
		GLuint getNormals();
		GLuint getworldPos();

		GLuint getProgram();
};
#endif