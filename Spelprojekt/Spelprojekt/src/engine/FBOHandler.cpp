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

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, worldpos, 0);

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depth, 0);

	GLenum DrawBuffers[] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2 };
	glDrawBuffers(3, DrawBuffers);		//Adds the buffer to the buffer list

	glBindFramebuffer(GL_FRAMEBUFFER, 0);		//returns to the original framebuffer

	program = 0;

	const char* vertex_shader = R"(
#version 400
layout (location = 0) in vec3 vertex_position;
layout(location = 1) in vec3 normal;

uniform float gTanHalfFOV;

out vec2 texcoord;
out vec2 viewRay;
void main () 
{
	gl_Position = vec4 (vertex_position, 1.0);
	texcoord = (vertex_position.xy + vec2(1.0)) / 2.0f;
	viewRay.x = vertex_position.x * gTanHalfFOV * (800.0f / 800.0f);
	viewRay.y = vertex_position.y * gTanHalfFOV;
}
)";
	const char* fragment_shader = R"(
#version 400
uniform sampler2D color;
uniform sampler2D normal;
uniform sampler2D depth;
uniform sampler2D worldpos;
//uniform mat3 Light;

in vec2 texcoord;
out vec4 lighted_scene;
in vec2 viewRay;

uniform mat4 gProj;
uniform mat4 viewMat;

uniform vec3 eyepos;


vec3 pSphere[16] = vec3[](vec3(0.53812504, 0.18565957, -0.43192),vec3(0.13790712, 0.24864247, 0.44301823),vec3(0.33715037, 0.56794053, -0.005789503),vec3(-0.6999805, -0.04511441, -0.0019965635),vec3(0.06896307, -0.15983082, -0.85477847),vec3(0.056099437, 0.006954967, -0.1843352),vec3(-0.014653638, 0.14027752, 0.0762037),vec3(0.010019933, -0.1924225, -0.034443386),vec3(-0.35775623, -0.5301969, -0.43581226),vec3(-0.3169221, 0.106360726, 0.015860917),vec3(0.010350345, -0.58698344, 0.0046293875),vec3(-0.08972908, -0.49408212, 0.3287904),vec3(0.7119986, -0.0154690035, -0.09183723),vec3(-0.053382345, 0.059675813, -0.5411899),vec3(0.035267662, -0.063188605, 0.54602677),vec3(-0.47761092, 0.2847911, -0.0271716));

void main ()
{
	vec3 gKernel[64];
	

	vec4 mySample = texture(color, vec2(texcoord.s, texcoord.t));

	vec4 pos = texture(worldpos, vec2(texcoord.s, texcoord.t));
	vec4 norm = normalize(texture(normal, vec2(texcoord.s, texcoord.t)));

	norm.w = 1.0f;
	float AO = 0.0f;
	
	vec3 samp = vec3(0);
	for (int i = 1 ; i < 16 ; ++i) {
        vec4 poissonPos = pos + vec4(pSphere[i] * 0.25, 1.0);
		vec4 sampleProj = viewMat * poissonPos;

		sampleProj /= sampleProj.w;
		sampleProj = (sampleProj + 1) / 2;

		vec2 sampleTexCoord = sampleProj.xy;
		vec4 samplePos = (texture(worldpos, vec2(sampleTexCoord.s, sampleTexCoord.t)));
		vec4 sampleDir = normalize(pos - samplePos);

		normalize(norm);
		
		float dist = -distance(samplePos, pos);
		float a = 1.0 - smoothstep(1, 2, dist);
		float b = max(dot(norm, sampleDir), 0);

		AO += (a * b);
	
		samp = poissonPos.xyz;
}

	AO = ( AO)/16.0f;
	AO = (1 - 2 * AO);
	

	lighted_scene = vec4(AO, AO, AO, 1.0);
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

	glGenTextures(1, &worldpos);
	glBindTexture(GL_TEXTURE_2D, worldpos);

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

GLuint FBOHandler::getworldPos()
{
	return worldpos;
}

GLuint FBOHandler::getProgram()
{
	return program;
}