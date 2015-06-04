#include "gameObject.h"

int GameObject::bindWorldMat(GLuint* shaderProgram, GLuint* shaderuniform) const
{
	glProgramUniformMatrix4fv(*shaderProgram, *shaderuniform, 1, false, &worldMat[0][0]);
	return id;
}

void GameObject::rotateTo(float x, float y, float z)
{
	glm::vec3 pos = glm::vec3(worldMat[0].w, worldMat[1].w, worldMat[2].w);

	//move to origo
	worldMat[0].w -= pos.x;
	worldMat[1].w -= pos.y;
	worldMat[2].w -= pos.z;

	//rot X
	worldMat *= glm::mat4(1, 0.0f, 0.0f, 0.0f,
		0.0f, cos(x), -sin(x), 0.0f,
		0.0f, sin(x), cos(x), 0.0f,
		0.0f, 0.0f, 0.0f, 1.0f);
	//rot Y
	worldMat *= glm::mat4(cos(y), 0.0f, -sin(y), 0.0f,
		0.0f, 1.0f, 0.0f, 0.0f,
		sin(y), 0.0f, cos(y), 0.0f,
		0.0f, 0.0f, 0.0f, 1.0f);

	//rot Z
	worldMat *= glm::mat4(cos(z), -sin(z), 0.0f, 0.0f,
		sin(z), cos(z), 0.0f, 0.0f,
		0.0f, 0.0f, 1.0f, 0.0f,
		0.0f, 0.0f, 0.0f, 1.0f);

	//move back to worldspace pos
	worldMat[0].w += pos.x;
	worldMat[1].w += pos.y;
	worldMat[2].w += pos.z;
}

void GameObject::translate(float x, float y)
{
	worldMat[0].w += x;
	worldMat[1].w += y;
}

void GameObject::translate(float x, float y, float z)
{
	worldMat[0].w += x;
	worldMat[1].w += y;
	worldMat[2].w += z;
}

void GameObject::moveTo(glm::vec3 target)
{
	worldMat[0].w = target.x;
	worldMat[1].w = target.y;
	worldMat[2].w = target.z;
}

void GameObject::moveTo(float x, float y)
{
	worldMat[0].w = x;
	worldMat[1].w = y;
}

void GameObject::moveTo(float x, float y, float z)
{
	worldMat[0].w = x;
	worldMat[1].w = y;
	worldMat[2].w = z;
}

glm::vec3 GameObject::readPos()
{
	return glm::vec3(worldMat[0].w, worldMat[1].w, worldMat[2].w);
}

void GameObject::scaleUniformAD(float val)
{
	worldMat[0].x += val;
	worldMat[1].y += val;
	worldMat[2].z += val;
}

void GameObject::scaleUniformFactor(float val)
{
	worldMat[0].x *= val;
	worldMat[1].y *= val;
	worldMat[2].z *= val;
}

void GameObject::scaleFactor(float x, float y, float z)
{
	worldMat[0].x *= x;
	worldMat[1].y *= y;
	worldMat[2].z *= z;
}

int GameObject::update(float deltaTime)
{
	return 0;
}