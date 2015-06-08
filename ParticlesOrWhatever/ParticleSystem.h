#ifndef PARTICLESYSTEM_H
#define PARTICLESYSTEM_H
#include <gl/glew.h>
#include <gl/GL.h>
#include "glm/glm/glm.hpp"
#include "glm/glm/gtx/transform.hpp"
#include <string>

using namespace glm;

struct Particle
{
	vec3 pos, speed;
	vec4 color;
	float life;
	float size;

	Particle(){}
	Particle(vec3 &pos, vec3 &speed, vec4 &color, float life, float size)
	{
		this->pos = pos;
		this->speed = speed;
		this->color = color;
		this->life = life;
		this->size = size;
	}
};

class ParticleSystem
{
private:
	GLuint particleShader;

	GLuint particles;
	static const int maxParticles = 7000;

	Particle system[maxParticles];
	int particleCount;
	Particle modelParticle;

	int intensity;

	std::string getShader(const char* filePath);

	void createParticle();
	int lastUsed;
	float* update(int& dataInBuffer);
public:
	ParticleSystem();
	ParticleSystem(Particle &original, int intensity);
	int bindParticleSystem();
	GLuint getShaderProgram();

	void compileErrorPrint(GLuint* shader);
};

#endif