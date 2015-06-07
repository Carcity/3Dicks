#include "engine.h"

Engine::~Engine()
{
	if (cam)
		delete cam;
	if (viewMatrix)
		delete viewMatrix;
	if (fboHandler)
		delete fboHandler;
}

void Engine::init(glm::mat4* viewMat)
{
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	glDepthMask(GL_TRUE);
	glClearColor(0,0,0, 1);
	//glEnable(GL_CULL_FACE);
	//glFrontFace(GL_CCW);


	//temp camera
	viewMatrix = viewMat;
	projMatrix = glm::perspective(3.14f*0.45f, 800.f / 800.0f, 0.1f, 1000.0f);

	cam = new CameraControl();
	fboHandler = new FBOHandler(800, 800);
	createScreenQuad();
	
	createLights();
	//Temp shader
	const char* vertex_shader = R"(
	#version 410
	layout(location = 0) in vec3 vertex_position;
	layout(location = 1) in vec2 UV;

	layout(location = 0) out vec2 UVCord;

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
	layout(location = 1) out vec3 normal;

	void main ()
	{
		vec3 vector0 = vec3(gl_in[1].gl_Position - gl_in[0].gl_Position);
		vec3 vector1 = vec3(gl_in[2].gl_Position - gl_in[0].gl_Position);

		vec3 myCross = normalize(cross(vector0, vector1));
	
		mat3 normalMat = transpose(inverse(mat3(modelMatrix)));
		normal = normalize(normalMat * myCross);


		//backface culling. Send the triangle into world space and decide if its facing the camera
		vec3 viewDirectionVector = normalize(gl_in[0].gl_Position.xyz - ViewPoint);
	
		//If the dot product is negative, the primitive is facing away from the camera
		float dotProduct = dot(viewDirectionVector, normal);

		//if(dotProduct <= 0.0)		//If its facing the wrong way, dont draw it.
		//{
			for(int i = 0; i < gl_in.length(); i++)
			{
				UVCoord = UV[i];
				gl_Position = VP * (gl_in[i].gl_Position * modelMatrix);
				EmitVertex();
			}
		
		EndPrimitive();
		//}
	}
)";

	const char* fragment_shader = R"(
	#version 410
	layout(location = 0) in vec2 UV;
	layout(location = 1) in vec3 normal;

	uniform sampler2D textureSample;
	layout(location = 0) out vec4 fragment_color;
	layout(location = 1) out vec3 normal_out;

	void main () 
	{
		fragment_color = texture(textureSample,vec2(UV.s, UV.t));
		normal_out = normal;
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

void Engine::linkDeferredTextures(GLuint program)
{
	GLuint loc = 0;
	loc = glGetUniformLocation(program, "color");		//Sends in the color texture to deferred shader
	glUniform1i(loc, 0);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, fboHandler->getColor());

	loc = glGetUniformLocation(program, "depth");
	glUniform1i(loc, 1);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, fboHandler->getDepth());

	loc = glGetUniformLocation(program, "normal");
	glUniform1i(loc, 2);
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, fboHandler->getNormals());

	GLuint ViewPoint = glGetUniformLocation(program, "eyepos");
	glUniform3fv(ViewPoint, 1, &cam->getPosition()[0]);

	//mat3 Light = getLightMatrix();
	//GLuint LightID = glGetUniformLocation(program, "Light"); //Sends in the light matrix to defferred shader
	//glUniformMatrix3fv(LightID, 1, GL_FALSE, &Light[0][0]);
}

void Engine::createScreenQuad()
{
	GLuint vbo = 0;
	float screenquad[] =
	{
		1.0f, 1.0f, 0.0f,
		1.0f, -1.0f, 0.0f,
		-1.0f, 1.0, 0.0f,
		-1.0f, -1.0f, 0.0f
	};
	glGenVertexArrays(1, &screenQuad);
	glBindVertexArray(screenQuad);


	glGenBuffers(1, &vbo); // Generate our Vertex Buffer Object  
	glBindBuffer(GL_ARRAY_BUFFER, vbo); // Bind our Vertex Buffer Object 

	glEnableVertexAttribArray(0);
	glBufferData(GL_ARRAY_BUFFER, sizeof(screenquad), screenquad, GL_STATIC_DRAW); // Set the size and data of our VBO and set it to STATIC_DRAW  
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GL_FLOAT), (void*)0);


	//delete[] screenquad;
}

void Engine::createLights()
{
	//including lights that cast shadows

	

	spotlight = new SpotLight[1];
	nrOfSpotLights = 1;

	spotlight[0].Color = vec3(1.0f, 1.0f, 1.0f);
	spotlight[0].Position = vec3(5.0f, 2.0f, 5.0f);
	spotlight[0].Direction = normalize(vec3(-3.0f, -1.0f, -3.0f));
	spotlight[0].DiffuseIntensity = 1.00f;
	spotlight[0].AmbientIntensity = 0.2f;


}

void Engine::linkLights()
{
	GLuint eyepos, nrOfSpotlights;
	eyepos = glGetUniformLocation(fboHandler->getProgram(), "eyepos");
	nrOfSpotlights = glGetUniformLocation(fboHandler->getProgram(), "eyepos");
	glProgramUniform3fv(fboHandler->getProgram(), eyepos, 1, &cam->getPosition()[0]);
	glProgramUniform1i(fboHandler->getProgram(), nrOfSpotlights, nrOfSpotlights);
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

	fboHandler->bind();
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glViewport(0, 0, 800, 800);


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

	fboHandler->unbind();
	glViewport(0, 0, 800, 800);
	glUseProgram(fboHandler->getProgram());

	linkDeferredTextures(fboHandler->getProgram());
	linkLights();
	glBindVertexArray(screenQuad);

	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	//glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
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
		// Exit with failure.9
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