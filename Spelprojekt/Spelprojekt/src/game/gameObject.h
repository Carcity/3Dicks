#ifndef GAME_OBJECT_H
#define GAME_OBJECT_H

#include <GL\glew.h>
#include <GLFW\glfw3.h>
#include <..\glm\glm.hpp>
#include <..\glm\gtc\matrix_transform.hpp>

#include "../Align16.h"

class GameObject : public Align_16
{
private:
	glm::mat4 worldMat = { 1, 0, 0, 0,
						   0, 1, 0, 0,
						   0, 0, 1, 0,
						   0, 0, 0, 1 };
public:
	GameObject(){};
	int id = -1;
	GameObject(int idi){ id = idi; };
	int bindWorldMat(GLuint* shaderProgram, GLuint* shaderuniform) const;
	
	void rotateTo(float x, float y, float z);
	void translate(float x, float y, float z);
	void translate(float x, float y);
	void moveTo(float x, float y);
	void moveTo(float x, float y, float z);
	void moveTo(glm::vec3 target);
	void scaleUniformAD(float val);
	void scaleUniformFactor(float val);
	void scaleFactor(float x, float y, float z);

	virtual int update(float deltaTime);
	
	glm::vec3 readPos();

};

#endif


