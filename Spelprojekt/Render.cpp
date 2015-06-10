#include "Render.h"
#include "bth_image.h"
#include <fstream>
#include <vector>

Render::Render()
{
	shadowMap = new ShadowMapFBO();
	//glViewport(0, 0, 640 * 10, 480 * 10);
	shadowMap->Init(480, 640);
	//glViewport(0, 0, 640, 480);	
	rottemp = 0.0f;
#ifdef _DEBUG
	{GLenum err = glGetError(); if (err)
		int x = 0; }
#endif
}

Render::~Render()
{

}

void Render::CreateShaders()
{
	shader = new Shader(&gShaderProgram, false);
	shaderShadow = new Shader(&gShaderProgramShadow, true);
#ifdef _DEBUG
	{GLenum err = glGetError(); if (err)
		int x = 0; }
#endif
}

int Render::render()
{
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glUseProgram(gShaderProgram);
	glProgramUniformMatrix4fv(gShaderProgram, shader->view, 1, false, &viewMatrix[0][0]);
	glProgramUniformMatrix4fv(gShaderProgram, shader->proj, 1, false, &projMatrix[0][0]);

	glProgramUniform1i(gShaderProgram, shader->textureSample, 0);
	glProgramUniform3fv(gShaderProgram, shader->eyepos, 1, &(*in->GetPos())[0] );
	glProgramUniform1i(gShaderProgram, shader->NumSpotLights, nrSpotLights);
	glBindBuffer(GL_UNIFORM_BUFFER, shader->lightBuffer);
	glBufferData(GL_UNIFORM_BUFFER, sizeof(SpotLight) * nrSpotLights, spotLights, GL_DYNAMIC_DRAW);
	glBindBufferBase(GL_UNIFORM_BUFFER, shader->bindingPoint, shader->lightBuffer);


	glProgramUniform1i(gShaderProgram, shader->ShadowMap, 1);
	shadowMap->BindForReading(GL_TEXTURE0 + 1);
	glm::mat4 lightview = glm::mat4(glm::lookAt(spotLights[0].Position, spotLights[0].Position + spotLights[0].Direction, vec3(0, 1, 0)));
	glProgramUniformMatrix4fv(gShaderProgram, shader->ViewMatrixSM, 1, false, &lightview[0][0]);
	glProgramUniformMatrix4fv(gShaderProgram, shader->ProjectionMatrixSM, 1, false, &projMatrix[0][0]);

	obj->bind();
	glDrawElements(GL_TRIANGLES, obj->getFaces() * 3, GL_UNSIGNED_SHORT, 0);
	obj2->bind();
	glDrawElements(GL_TRIANGLES, obj2->getFaces() * 3, GL_UNSIGNED_SHORT, 0);
#ifdef _DEBUG
	{GLenum err = glGetError(); if (err)
		int x = 0; }
#endif
	return 0;
}

void Render::ShadowMapPass()
{
	//glViewport(0, 0, 6400, 4800);
	glUseProgram(gShaderProgramShadow);

	shadowMap->BindForWriting();
	
	glClear(GL_DEPTH_BUFFER_BIT);

	//save old camera matrix
	mat4 saveview = viewMatrix;
	//put camera in light's possition
	viewMatrix = glm::lookAt(spotLights[0].Position, spotLights[0].Position + spotLights[0].Direction, vec3(0, 1, 0));

	glProgramUniformMatrix4fv(gShaderProgramShadow, shaderShadow->ViewMatrixSM, 1, false, &viewMatrix[0][0]);
	glProgramUniformMatrix4fv(gShaderProgramShadow, shaderShadow->ProjectionMatrixSM, 1, false, &projMatrix[0][0]);

	obj->bind();
	glDrawElements(GL_TRIANGLES, obj->getFaces() * 3, GL_UNSIGNED_SHORT, 0);
	obj2->bind();
	glDrawElements(GL_TRIANGLES, obj2->getFaces() * 3, GL_UNSIGNED_SHORT, 0);

	viewMatrix = saveview;
	//glViewport(0, 0, 640, 480);
#ifdef _DEBUG
	{GLenum err = glGetError(); if (err)
		int x = 0; }
#endif
}
