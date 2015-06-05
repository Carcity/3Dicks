#include "engine.h"
#include <vector>

using namespace std;

Engine::~Engine()
{
}

void Engine::init(CameraControl* cc)
{
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	glDepthMask(GL_TRUE);
	glClearColor(0, 0, 0, 1);
	//glEnable(GL_CULL_FACE);
	//glFrontFace(GL_CCW);

	cam = cc;

	//Temp shader
	const char* vertex_shader = R"(
	#version 410
	layout(location = 0) in vec3 vertex_position;
	layout(location = 1) in vec2 UV;

	layout(location = 0) out vec2 UVCord;

	uniform mat4 modelMatrix;
	uniform mat4 VP;

	void main () 
	{
		vec3 position = vertex_position;
		
		UVCord = UV;
		gl_Position =  VP * (vec4(position, 1.0f) * modelMatrix);
	}
)";

	const char* fragment_shader = R"(
	#version 410
	layout(location = 0) in vec2 UV;

	uniform sampler2D textureSample;
	out vec4 fragment_color;

	void main () 
	{
		fragment_color = texture(textureSample,vec2(UV.s, UV.t));
	}
)";

	GLint success = 0;

	//create vertex shader
	GLuint vs = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vs, 1, &vertex_shader, nullptr);
	glCompileShader(vs);
	CompileErrorPrint(&vs);

	//create fragment shader
	GLuint fs = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fs, 1, &fragment_shader, nullptr);
	glCompileShader(fs);
	CompileErrorPrint(&fs);

	//link shader program (connect vs and ps)
	tempshader = glCreateProgram();
	glAttachShader(tempshader, vs);
	glAttachShader(tempshader, fs);
	glLinkProgram(tempshader);

	LinkErrorPrint(&tempshader);

	uniformModel = glGetUniformLocation(tempshader, "modelMatrix");
	uniformVP = glGetUniformLocation(tempshader, "VP");

	const char* frust_vertex_shader = R"(
	#version 400
	layout(location = 0) in vec3 vertex_position;
		
	uniform mat4 View;
	uniform mat4 Projection;

	void main () {
		gl_Position = vec4(vertex_position, 1.0);
		gl_Position = View*gl_Position;
		gl_Position = Projection*gl_Position;
	}
)";

	const char* frust_fragment_shader = R"(
	#version 400
	out vec4 fragment_color;

	void main () {
		fragment_color = vec4(1, 0, 0, 1);
	}
)";
	//create vertex shader
	GLuint fvs = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(fvs, 1, &frust_vertex_shader, nullptr);
	glCompileShader(fvs);
	CompileErrorPrint(&fvs);

	//create fragment shader
	GLuint ffs = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(ffs, 1, &frust_fragment_shader, nullptr);
	glCompileShader(ffs);
	CompileErrorPrint(&ffs);

	//link shader program (connect vs and ps)
	frustumProgram = glCreateProgram();
	glAttachShader(frustumProgram, fvs);
	glAttachShader(frustumProgram, ffs);
	glLinkProgram(frustumProgram);
}

void Engine::renderFrustum(QuadTree* qt)
{
	
	GLuint frustBuf = 0;
	glUseProgram(frustumProgram);
	vec3 contain[8];
	qt->getFrustumCorners(contain);
	//This rebinding of buffer data crashes the game, cause it changes data, that it for some reason can't change back when it tries to draw the normal objet

	glBindBuffer(GL_ARRAY_BUFFER, frustBuf);
	glBufferData(GL_ARRAY_BUFFER, sizeof(contain), contain, GL_STREAM_DRAW);
	//glEnableVertexAttribArray(0);

	GLuint vertexPos = glGetAttribLocation(frustumProgram, "vertex_position");
	glVertexAttribPointer(vertexPos, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));

	mat4 viewMat = cam->getViewMatrix();
	GLuint loc = glGetUniformLocation(frustumProgram, "View");
	glUniformMatrix4fv(loc, 1, GL_FALSE, &viewMat[0][0]);
	mat4 projMat = cam->getProjMatrix();
	loc = glGetUniformLocation(frustumProgram, "Projection");
	glUniformMatrix4fv(loc, 1, GL_FALSE, &projMat[0][0]);

	//glDrawArrays(GL_LINE_STRIP, 0, 8);
	//glDrawElements(GL_TRIANGLES, 8, GL_UNSIGNED_SHORT, 0);
}

void Engine::render(const Map* map, const ContentManager* content, const AnimationManager* anim)
{
	cam->updateVectors();
	mat4 viewMatrix = cam->getViewMatrix();
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	int facecount = 0;
	glUseProgram(tempshader);

	mat4 VP = cam->getProjMatrix() * viewMatrix;
	glProgramUniformMatrix4fv(tempshader, uniformVP, 1, false, &VP[0][0]);

	int id = 0;
	int lastid = -1;

	vector<GameObject> gameObjects = map->getObjects();
	const GameObject* background = map->getBackground();

	id = background->bindWorldMat(&tempshader, &uniformModel);

	//Den kan rita bakgrunden i en frame, sen dör den Vad händer efter första framen?
	facecount = content->bindMapObj(id); //This will be the same
	glDrawElements(GL_TRIANGLES, facecount * 3, GL_UNSIGNED_SHORT, 0);
	lastid = id;
	lastid = -1;
	//world objects

	for (int i = 0; i < gameObjects.size(); i++)
	{
		id = gameObjects[i].bindWorldMat(&tempshader, &uniformModel);
		if (id != lastid)
			facecount = content->bindMapObj(id); //This will be changed to AnimationManager, to get the animated meshes
		glDrawElements(GL_TRIANGLES, facecount * 3, GL_UNSIGNED_SHORT, 0);
		lastid = id;
	}
	
	//renderFrustum(map->getQuadTree());
}

void Engine::CompileErrorPrint(GLuint* shader)
{
	GLint success = 0;
	glGetShaderiv(*shader, GL_COMPILE_STATUS, &success); //not working????
	if (success == GL_FALSE)
	{
		GLint maxLength = 0;
		glGetShaderiv(*shader, GL_INFO_LOG_LENGTH, &maxLength);

		// The maxLength includes the NULL character
		std::vector<GLchar> errorLog(maxLength);
		glGetShaderInfoLog(*shader, maxLength, &maxLength, &errorLog[0]);

		std::fstream myfile;
		myfile.open("errorCheck.txt", std::fstream::out);
		for (int i = 0; i < maxLength; i++)
		{
			myfile << errorLog[i];
		}
		myfile.close();

		// Provide the infolog in whatever manor you deem best.
		// Exit with failure.
		glDeleteShader(*shader); // Don't leak the shader.
		throw;
	}
}

void Engine::LinkErrorPrint(GLuint* shaderProgram)
{
	GLint success = 10;
	glGetProgramiv(*shaderProgram, GL_LINK_STATUS, &success);
	if (success == GL_FALSE)
	{
		GLint maxLength = 0;
		glGetProgramiv(*shaderProgram, GL_INFO_LOG_LENGTH, &maxLength);

		// The maxLength includes the NULL character
		std::vector<GLchar> errorLog(maxLength);
		glGetProgramInfoLog(*shaderProgram, maxLength, &maxLength, &errorLog[0]);

		std::fstream myfile;
		myfile.open("errorCheck.txt", std::fstream::out);
		for (int i = 0; i < maxLength; i++)
		{
			myfile << errorLog[i];
		}
		myfile.close();

		// Provide the infolog in whatever manor you deem best.
		// Exit with failure.
		glDeleteProgram(*shaderProgram); // Don't leak the shader.

		if (success == GL_FALSE)
			throw;
	}
}