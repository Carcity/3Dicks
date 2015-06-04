#include "FBOHandler.h"
#include <Windows.h>
#include <iostream>

FBOHandler::FBOHandler()
{}

FBOHandler::FBOHandler(unsigned int width, unsigned int height)
{
	glGenFramebuffers(1, &FBO);					//Generate a framebuffer object
	generateTexture(width, height); //generate a black texture
	glBindFramebuffer(GL_FRAMEBUFFER, FBO);		//Bind it to the pipeline, framebuffer reads and writes data
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, color, 0); //Binds the texture to the frame buffer

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, normals, 0);

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depth, 0);

	GLenum DrawBuffers[] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1 };
	glDrawBuffers(2, DrawBuffers);		//Adds the buffer to the buffer list

	glBindFramebuffer(GL_FRAMEBUFFER, 0);		//returns to the original framebuffer

	program = 0;

	const char* vertex_shader = R"(
#version 400
layout (location = 0) in vec3 vertex_position;
layout(location = 1) in vec3 normal;

out vec2 texcoord;

void main () 
{
	gl_Position = vec4 (vertex_position, 1.0);
	texcoord = vec2(clamp(vertex_position.x,0,1), clamp(vertex_position.y,0,1));
}
)";
	const char* fragment_shader = R"(
#version 400
uniform sampler2D color;
uniform sampler2D normal;
uniform sampler2D depth;
//uniform mat3 Light;

in vec2 texcoord;
out vec4 lighted_scene;

void main ()
{
	//Ska andras
	vec4 mySample = texture(normal, vec2(texcoord.s, texcoord.t));
	lighted_scene = mySample;
}
)";

	//create vertex shader
	GLuint vs = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vs, 1, &vertex_shader, nullptr);
	glCompileShader(vs);

	//create fragment shader
	GLuint fs = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fs, 1, &fragment_shader, nullptr);
	glCompileShader(fs);

	program = glCreateProgram();
	glAttachShader(program, vs);
	glAttachShader(program, fs);
	glLinkProgram(program);

	GLint success = 10;
	glGetProgramiv(program, GL_LINK_STATUS, &success);
	if (success == GL_FALSE)
	{
		GLint maxLength = 0;
		glGetProgramiv(program, GL_INFO_LOG_LENGTH, &maxLength);

		// The maxLength includes the NULL character
		std::vector<GLchar> errorLog(maxLength);
		glGetProgramInfoLog(program, maxLength, &maxLength, &errorLog[0]);

		std::fstream myfile;
		myfile.open("errorCheck.txt", std::fstream::out);
		for (int i = 0; i < maxLength; i++)
		{
			myfile << errorLog[i];
		}
		myfile.close();

		// Provide the infolog in whatever manor you deem best.
		// Exit with failure.
		glDeleteProgram(program); // Don't leak the shader.

		if (success == GL_FALSE)
			throw;
	}
}	

FBOHandler::~FBOHandler()
{
	unbind();
}

std::string FBOHandler::getShader(const char* filePath)
{
	std::ifstream myReadFile;
	myReadFile.open(filePath);
	std::string content = "";
	std::string line;

	if (myReadFile.is_open())
	{
		while (!myReadFile.eof())
		{
			std::getline(myReadFile, line);
			content.append(line + "\n");
		}
	}
	myReadFile.close();
	return content;
}

void FBOHandler::generateTexture(unsigned int width, unsigned int height)
{
	glGenTextures(1, &color);
	glBindTexture(GL_TEXTURE_2D, color);

	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S,
		GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T,
		GL_CLAMP_TO_EDGE);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA,
		GL_UNSIGNED_BYTE, NULL);

	glGenTextures(1, &depth);
	glBindTexture(GL_TEXTURE_2D, depth);

	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S,
		GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T,
		GL_CLAMP_TO_EDGE);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT24, width, height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, 0);

	glGenTextures(1, &normals);
	glBindTexture(GL_TEXTURE_2D, normals);

	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S,
		GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T,
		GL_CLAMP_TO_EDGE);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGBA,
		GL_FLOAT, NULL);
}

void FBOHandler::bind()
{
	glBindFramebuffer(GL_FRAMEBUFFER, FBO);	//Bind it to the pipeline, framebuffer reads and writes data
}

void FBOHandler::unbind()
{
	glBindFramebuffer(GL_FRAMEBUFFER, 0);		//returns to the original framebuffer
}

GLuint FBOHandler::getColor()
{
	return color;
}

GLuint FBOHandler::getDepth()
{
	return depth;
}

GLuint FBOHandler::getNormals()
{
	return normals;
}

GLuint FBOHandler::getProgram()
{
	return program;
}