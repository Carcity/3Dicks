#include "Shader.h"
#include <fstream>
#include <iostream>
#include <vector>
#include <string>

Shader::Shader(GLuint* gShaderP, bool in)
{
	IsShadowPass = in;
	gShaderProgram = gShaderP;
	compile();

	if(IsShadowPass == false)
	{
		view = glGetUniformLocation(*gShaderProgram, "ViewMatrix");
		proj = glGetUniformLocation(*gShaderProgram, "ProjectionMatrix");
		textureSample = glGetUniformLocation(*gShaderProgram, "texSampler");
		NumSpotLights = glGetUniformLocation(*gShaderProgram, "NumSpotLights");
		eyepos = glGetUniformLocation(*gShaderProgram, "eyepos");

		lightBlockUniformLoc = glGetUniformBlockIndex(*gShaderProgram, "Light");
		glUniformBlockBinding(*gShaderProgram, lightBlockUniformLoc, bindingPoint);
		glGenBuffers(1, &lightBuffer);

		ShadowMap = glGetUniformLocation(*gShaderProgram, "ShadowMap");
		ViewMatrixSM = glGetUniformLocation(*gShaderProgram, "ViewMatrixSM");
		ProjectionMatrixSM = glGetUniformLocation(*gShaderProgram, "ProjectionMatrixSM");
	}
	else
	{
		ShadowMap = glGetUniformLocation(*gShaderProgram, "ShadowMap");
		ViewMatrixSM = glGetUniformLocation(*gShaderProgram, "ViewMatrixSM");
		ProjectionMatrixSM = glGetUniformLocation(*gShaderProgram, "ProjectionMatrixSM");
	}
#ifdef _DEBUG
	{GLenum err = glGetError(); if (err)
		int x = 0; }
#endif
}

bool Shader::compile()
{
	if (IsShadowPass == false)
	{
		const char* vertex_shader = R"(
	#version 400
	layout(location = 0) in vec3 vertex_position;
	layout(location = 1) in vec2 UV;
	out vec2 UVCord;
	void main () 
	{
		UVCord = UV;
		vec4 vertex_matrix = vec4 (vertex_position, 1.0f);
		gl_Position = vertex_matrix;
	}
)";

		const char* geometry_shader = R"(
	#version 400
	layout(triangles) in;
	layout(triangle_strip, max_vertices = 3) out;

	uniform mat4 ViewMatrix;
	uniform mat4 ProjectionMatrix;

	in vec2 UVCord[3];
	out vec2 UVs;
	out vec3 Normals;
	out vec3 Position;
	out mat4 VP;
	void main() 
	{
		vec3 line1 = vec3(gl_in[1].gl_Position - gl_in[0].gl_Position);
		vec3 line2 = vec3(gl_in[2].gl_Position - gl_in[0].gl_Position);
		Normals = normalize(cross(line1, line2));
		VP = mat4(ProjectionMatrix * ViewMatrix);

		for(int n = 0; n < 3; n++)
		{
			Position = vec3( gl_in[n].gl_Position);
			gl_Position = ProjectionMatrix * ViewMatrix * gl_in[n].gl_Position;
			UVs = UVCord[n];
			EmitVertex();
		}
		EndPrimitive();
	}
)";

const char* fragment_shader = R"(
	#version 400
	in vec2 UVs;
	in vec3 Normals;
	in vec3 Position;
	in mat4 VP;
	
	float gSpecularPower = 4;
	float gMatSpecularIntensity = 0.4;

	uniform sampler2D ShadowMap; 
	uniform mat4 ViewMatrixSM;
	uniform mat4 ProjectionMatrixSM;

struct SpotLight
{
	vec3 Color;
	float DiffuseIntensity;
	vec3 Position;
	float AmbientIntensity;
	vec3 Direction;
	float Cutoff;
	float Constant;
	float Linear;
	float Exp;
	float padd;
};

layout (std140) uniform Light
{ 
	SpotLight lights[3];
};
	uniform sampler2D texSampler;
	uniform int NumSpotLights; 
	uniform vec3 eyepos;
	out vec4 fragment_color;

vec4 CalcLightInternal(SpotLight l, vec3 LightDirection, vec3 Normal)                   
{                                                                                           
    vec4 AmbientColor = vec4(l.Color, 1.0f) * l.AmbientIntensity;                   
    float DiffuseFactor = dot(Normal, -LightDirection);                                     
                                                                                            
    vec4 DiffuseColor  = vec4(0, 0, 0, 0);  //40                                                  
    vec4 SpecularColor = vec4(0, 0, 0, 0);                                                  
                                                                                            
    if (DiffuseFactor > 0) {                                                                
        DiffuseColor = vec4(l.Color, 1.0f) * l.DiffuseIntensity * DiffuseFactor;    
                                                                                            
        vec3 VertexToEye = normalize(eyepos - Position);                             
        vec3 LightReflect = normalize(reflect(LightDirection, Normal));                     
        float SpecularFactor = dot(VertexToEye, LightReflect);                              
        SpecularFactor = pow(SpecularFactor, gSpecularPower);                               
        if (SpecularFactor > 0) {                                                           
            SpecularColor = vec4(l.Color, 1.0f) *                                       
                            gMatSpecularIntensity * SpecularFactor;                         
        }                                                                                   
    }                                                                                       
                                                                                            
    return (AmbientColor + DiffuseColor + SpecularColor);                                   
}               

vec4 CalcPointLight(SpotLight l, vec3 Normal)
{
    vec3 LightDirection = Position - l.Position;
    float Distance = length(LightDirection);
    LightDirection = normalize(LightDirection);    

	//return(CalcLightInternal(l, LightDirection, Normal)); 
    vec4 Color = vec4(CalcLightInternal(l, LightDirection, Normal)); 
	float Attenuation =  l.Constant + l.Linear * Distance + l.Exp * Distance * Distance; 
	return Color / Attenuation;                           
}          

float CalcShadowFactor()
{
	vec4 LightSpacePos = vec4(Position, 1);
	LightSpacePos = ProjectionMatrixSM * ViewMatrixSM * LightSpacePos;
    vec3 ProjCoords = LightSpacePos.xyz / LightSpacePos.w;
    vec2 UVCoords;
    UVCoords.x = 0.5 * ProjCoords.x + 0.5;
    UVCoords.y = 0.5 * ProjCoords.y + 0.5;
    float z = 0.5 * ProjCoords.z + 0.5;
    float Depth = texture(ShadowMap, UVCoords).x;
    //return Depth;
	if (Depth < (z + 0.000001))
        return 0.01;
    else
        return 1.0;
}                                                                                 
                                                                                            
vec4 CalcSpotLight(SpotLight l, vec3 Normal)                                                
{                                                                                           
    vec3 LightToPixel = normalize(Position - l.Position);                             
    float SpotFactor = dot(LightToPixel, l.Direction);                                      
                                                                                            
    if (SpotFactor > l.Cutoff) {                                                            
        vec4 Color = CalcPointLight(l, Normal);                                        
        return Color * (1.0 - (1.0 - SpotFactor) * 1.0/(1.0 - l.Cutoff));                   
    }                                                                                       
    else {                                                                                  
        return vec4(0,0,0,0);                                                               
    }     
}  

void main () { 

	vec4 TotalLight = vec4(0,0,0,0);

	for(int n = 0; n < NumSpotLights; n++)
	{
		 TotalLight += CalcSpotLight(lights[n], Normals);
	}
	
	vec4 mySample = texture(texSampler, vec2(UVs.s, 1-UVs.t));
	vec3 textureColor = vec3(mySample.rgb);

	//vec4 mySample2 = texture(ShadowMap, vec2(UVs.s, 1-UVs.t));
	//vec3 textureColor2 = vec3(mySample2.rgb);

	fragment_color = vec4(TotalLight.xyz * textureColor * CalcShadowFactor(), 1);
}
)";

		GLint success = 0;

		//create vertex shader
		GLuint vs = glCreateShader(GL_VERTEX_SHADER);
		glShaderSource(vs, 1, &vertex_shader, nullptr);
		glCompileShader(vs);
		CompileErrorPrint(&vs);
		
		//create geometry shader
		GLuint gs = glCreateShader(GL_GEOMETRY_SHADER);
		glShaderSource(gs, 1, &geometry_shader, nullptr);
		glCompileShader(gs);
		CompileErrorPrint(&gs);

		//create fragment shader
		GLuint fs = glCreateShader(GL_FRAGMENT_SHADER);
		glShaderSource(fs, 1, &fragment_shader, nullptr);
		glCompileShader(fs);
		CompileErrorPrint(&fs);

		//link shader program (connect vs and ps)
		*gShaderProgram = glCreateProgram();
		glAttachShader(*gShaderProgram, vs);
		glAttachShader(*gShaderProgram, gs);
		glAttachShader(*gShaderProgram, fs);
		glLinkProgram(*gShaderProgram);
		LinkErrorPrint(gShaderProgram);

#ifdef _DEBUG
		{GLenum err = glGetError(); if (err)
			int x = 0; }
#endif
		
	}
	//===================================================
	//============= Shader for ShadowMapPass  ===========
	else
	{
		const char* vertex_shader = R"(
		#version 400
		layout(location = 0) in vec3 vertex_position;
		layout(location = 1) in vec2 UV;

		uniform mat4 ViewMatrixSM;
		uniform mat4 ProjectionMatrixSM;

		out vec2 UVCord;
		void main () 
		{
			UVCord = UV;
			gl_Position = ProjectionMatrixSM * ViewMatrixSM * vec4(vertex_position, 1);
		}
	)";

		const char* fragment_shader = R"(
		#version 400
		in vec2 UVCord;
                            
		uniform sampler2D ShadowMap;                                                       

		out vec4 fragment_color;
     
		void main()                                                                         
		{                                                                                   
			float Depth = texture(ShadowMap, UVCord).x;                               
			Depth = 1.0 - (1.0 - Depth) * 25.0;                                             
			fragment_color = vec4(Depth);                                                        
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

		*gShaderProgram = glCreateProgram();
		glAttachShader(*gShaderProgram, vs);
		glAttachShader(*gShaderProgram, fs);
		glLinkProgram(*gShaderProgram);
		LinkErrorPrint(gShaderProgram);
#ifdef _DEBUG
		{GLenum err = glGetError(); if (err)
			int x = 0; }
#endif
		return true;
	}
}

void Shader::CompileErrorPrint(GLuint* shader)
{
	GLint success = 0;
	glGetShaderiv(*shader, GL_COMPILE_STATUS, &success);
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

		glDeleteShader(*shader); 
		throw;
	}
}

void Shader::LinkErrorPrint(GLuint* shaderProgram)
{
	GLint success = 0;
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