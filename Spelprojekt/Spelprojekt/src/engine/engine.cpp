#include "engine.h"

Engine::~Engine()
{
	if (cam)
		delete cam;
	if (viewMatrix)
		delete viewMatrix;
}

void Engine::init(glm::mat4* viewMat)
{
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	glDepthMask(GL_TRUE);
	glClearColor(0, 0, 0, 1);
	//glEnable(GL_CULL_FACE);
	//glFrontFace(GL_CCW);


	//temp camera
	viewMatrix = viewMat;
	projMatrix = glm::perspective(3.14f*0.45f, 800.f / 800.0f, 0.1f, 1000.0f);

	cam = new CameraControl();

	//Temp shader
	const char* vertex_shader = R"(
	#version 410
	layout(location = 0) in vec3 vertex_position;
	layout(location = 1) in vec2 UV;

	layout(location = 0) out vec2 UVCord;

	//uniform mat4 modelMatrix;
	//uniform mat4 VP;

	void main () 
	{
		vec3 position = vertex_position;
		
		UVCord = UV;
		gl_Position =  vec4(position, 1.0f);//VP * (vec4(position, 1.0f) * modelMatrix);
	}
)";

	const char* geometry_shader = R"(
	#version 410
	layout(triangles) in;
	layout(triangle_strip, max_vertices = 3) out;

	in gl_PerVertex
	{
		vec4 gl_Position;
		float gl_PointSize;
		float gl_ClipDistance[];
	}
	gl_in[];

	uniform mat4 modelMatrix;
	uniform mat4 VP;
	uniform vec3 ViewPoint;
	
	layout(location = 0) in vec2 UV[];

	layout(location = 0) out vec2 UVCoord;

	void main ()
	{
		vec3 vector0 = vec3(gl_in[1].gl_Position - gl_in[0].gl_Position);
		vec3 vector1 = vec3(gl_in[2].gl_Position - gl_in[0].gl_Position);

		vec3 myCross = normalize(cross(vector0, vector1));
	
		mat3 normalMat = transpose(inverse(mat3(modelMatrix)));
		vec3 normal = normalize(normalMat * myCross);


		//backface culling. Send the triangle into world space and decide if its facing the camera
		vec3 viewDirectionVector = normalize(gl_in[0].gl_Position.xyz - ViewPoint);
	
		//If the dot product is negative, the primitive is facing away from the camera
		float dotProduct = dot(viewDirectionVector, normal);

		if(dotProduct <= 0.0)		//If its facing the wrong way, dont draw it.
		{
			for(int i = 0; i < gl_in.length(); i++)
			{
				UVCoord = UV[i];
				gl_Position = VP * (gl_in[i].gl_Position * modelMatrix);
				EmitVertex();
			}
		
		EndPrimitive();
		}
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

	//Geometry shader
	GLuint gs = glCreateShader(GL_GEOMETRY_SHADER);
	glShaderSource(gs, 1, &geometry_shader, nullptr);
	glCompileShader(gs);
	CompileErrorPrint(&gs);

	//link shader program (connect vs and ps)
	tempshader = glCreateProgram();
	glAttachShader(tempshader, vs);
	glAttachShader(tempshader, fs);
	glAttachShader(tempshader, gs);
	glLinkProgram(tempshader);

	LinkErrorPrint(&tempshader);

	uniformModel = glGetUniformLocation(tempshader, "modelMatrix");
	uniformVP = glGetUniformLocation(tempshader, "VP");

}

void Engine::render(const Map* map, const ContentManager* content, const AnimationManager* anim)
{
	viewMatrix = &cam->getViewMatrix();
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	int facecount = 0;
	glUseProgram(tempshader);

	glm::mat4 VP = projMatrix * *viewMatrix;
	glProgramUniformMatrix4fv(tempshader, uniformVP, 1, false, &VP[0][0]);

	GLuint ViewPoint = glGetUniformLocation(tempshader, "ViewPoint");
	glUniform3fv(ViewPoint, 1, &cam->getPosition()[0]);


	// -- PlayerDraw --
	//player->bindWorldMat(&tempshader, &uniformModel);
	//facecount = anim->bindPlayer(); //animationManager

	//glDrawElements(GL_TRIANGLES, facecount * 3, GL_UNSIGNED_SHORT, 0);

	// - -Map Draw --
	int id = 0;
	int lastid = -1;

	int size = map->getSize();
	GameObject** gameObjects = map->getObjects();
	const GameObject* background = map->getBackground();

	id = background->bindWorldMat(&tempshader, &uniformModel);
	if (id != lastid)
		facecount = content->bindMapObj(id); //This will be the same
	glDrawElements(GL_TRIANGLES, facecount * 3, GL_UNSIGNED_SHORT, 0);
	lastid = id;
	lastid = -1;
	//world objects

	for (int i = 0; i < size; i++)
	{
		id = gameObjects[i]->bindWorldMat(&tempshader, &uniformModel);
		if (id != lastid)
			facecount = content->bindMapObj(id); //This will be changed to AnimationManager, to get the animated meshes
		glDrawElements(GL_TRIANGLES, facecount * 3, GL_UNSIGNED_SHORT, 0);
		lastid = id;

	}
	
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