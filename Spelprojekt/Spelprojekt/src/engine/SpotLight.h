#ifndef SPOTLIGHT_H
#define SPOTLIGHT_H

#include <gl/glew.h>
#include <gl/GL.h>
#include "../../../glm/glm/glm.hpp"
#include "../../../glm/glm/gtc/matrix_transform.hpp"

using namespace glm;

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
	
	SpotLight()
	{
		Color = vec3(0.0f, 0.0f, 0.0f); // 0 1 2
		Position = vec3(0.0f, 0.0f, 0.0f); // 3 4 5
		Direction = vec3(0.0f, -1.0f, 0.0f); // 6 7 8
		AmbientIntensity = 0.0f; // 9
		DiffuseIntensity = 1.0f; // 10
		Constant = 1.0f; // 11
		Linear = 0.0f; // 12
		Exp = 0.0f; // 13
		Cutoff = 0.9f; // 14
	}
};

#endif
