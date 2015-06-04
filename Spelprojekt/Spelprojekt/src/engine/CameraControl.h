#ifndef CAMERACONTROL_H
#define CAMERACONTROL_H
#include <gl/glew.h>
#include <gl/GL.h>
#include "../../../glm/glm/glm.hpp"
#include "../../../glm/glm/gtx/transform.hpp"

using namespace glm;

class CameraControl
{
private:
	float side, up;
	vec3 position, direction, right, head;

	mat4 secView;
	bool secondViewTime;
public:
	CameraControl();
	mat4 getProjMatrix();
	mat4 getViewMatrix();
	vec3 getPosition();
	vec3 getDirection();
	vec3 getRightVec();
	vec3 getUpVec();

	void setPosition(vec3);

	void updateVectors();
};

#endif