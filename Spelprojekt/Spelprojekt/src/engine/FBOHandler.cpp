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

uniform vec3 eyepos;
float gSpecularPower = 20;
	float gMatSpecularIntensity = 0.4;

struct SpotLight
	{
		vec3 Color;
		float DiffuseIntensity;
		vec3 Position;
		float AmbientIntensity;
		vec3 Direction;
		float Cutoff;
		float linear;
		float constant;
		float exp;
	};

	vec4 Position0;
SpotLight gSpotLight;

vec4 CalcLightInternal(SpotLight l, vec3 LightDirection, vec3 Normal)                   
	{                                                                                           
		vec4 AmbientColor = vec4(l.Color, 1.0f) * l.AmbientIntensity;                   
		float DiffuseFactor = dot(Normal, -LightDirection);                                     
                                                                                            
		vec4 DiffuseColor  = vec4(0, 0, 0, 0);                                            
		vec4 SpecularColor = vec4(0, 0, 0, 0);                                                  
                                                                                            
		if (DiffuseFactor > 0) 
		{                                                                
			DiffuseColor = vec4(l.Color, 1.0f) * l.DiffuseIntensity * DiffuseFactor;    
                                                                                            
			vec3 VertexToEye = normalize(eyepos - Position0.xyz);                             
			vec3 LightReflect = normalize(reflect(LightDirection, Normal));                     
			float SpecularFactor = dot(VertexToEye, LightReflect);                              
			SpecularFactor = pow(SpecularFactor, gSpecularPower);                               
			if (SpecularFactor > 0) 
			{                                                           
				SpecularColor = vec4(l.Color, 1.0f) * gMatSpecularIntensity * SpecularFactor;                         
			}                                                                                   
		}                                                                                                                                                                     
		return (AmbientColor + DiffuseColor + SpecularColor);                                   
	}               
	
	vec4 CalcPointLight(SpotLight l, vec3 Normal)
	{
		vec3 LightDirection = Position0.xyz - l.Position;
		float Distance = length(LightDirection);
		LightDirection = normalize(LightDirection);    
	
		//return (CalcLightInternal(l, LightDirection, Normal)); 
		vec4 Color = vec4(CalcLightInternal(l, LightDirection, Normal)); 
		float Attenuation =  l.constant + l.linear * Distance + l.exp * Distance * Distance; 
		return Color / Attenuation;                           
	}                                                                                           
                                                                                            
	vec4 CalcSpotLight(SpotLight l, vec3 Normal)                                                
	{                                                                                           
		vec3 LightToPixel = normalize(Position0.xyz - l.Position);                             
		float SpotFactor = dot(LightToPixel, l.Direction);                                      
                                                                                            
		if (SpotFactor > l.Cutoff) {                                                            
			vec4 Color = CalcPointLight(l, Normal);                             
			return Color * (1.0 - (1.0 - SpotFactor) * 1.0/(1.0 - l.Cutoff));                   
		}                                                                                       
		else {                                                                                  
			return vec4(0,0,0,0);                                                               
		}     
	}  	

void main ()
{
	//Ska andras
	gSpotLight.Color = vec3(1,1,1);
	gSpotLight.DiffuseIntensity = 1;
	gSpotLight.AmbientIntensity = 0.2f;
	gSpotLight.Position = vec3(5,2,5);
	gSpotLight.Direction = normalize(vec3(-3, -1, -3));//normalize(vec3(0.22f, -0.33f, 0.44f));
	gSpotLight.Cutoff = 0.75f;
	gSpotLight.constant = 1.0f;
	gSpotLight.linear = 0.0f;
	gSpotLight.exp = 0.0f;

	
	float offset[5] = float[](0.0, 1.0, 2.0, 3.0, 4.0);
	float weights[5] = float[]( 0.2270270270, 0.1945945946, 0.1216216216, 0.0540540541, 0.0162162162 );
	
	vec4 mySample = texture(color, vec2(texcoord.s, texcoord.t));
	Position0 = texture(depth, vec2(texcoord.s, texcoord.t));
	vec4 normal0 = texture(normal, vec2(texcoord.s, texcoord.t));
	lighted_scene = mySample * CalcSpotLight(gSpotLight, normal0.xyz);
	
	
	for(int i = 1; i < 5; i++)
	{
		lighted_scene += texture(color, vec2(texcoord.s, texcoord.t) + vec2(offset[i], offset[i]) / 800.0f) * weights[i]; 
		lighted_scene += texture(color, vec2(texcoord.s, texcoord.t) - vec2(offset[i], offset[i]) / 800.0f) * weights[i]; 
	}
	
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