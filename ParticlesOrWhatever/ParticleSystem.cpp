#include "ParticleSystem.h"
#include <string>
#include <fstream>
#include <algorithm>

#define BUFFER_OFFSET(i) ((char *)nullptr + (i))

ParticleSystem::ParticleSystem(){}

ParticleSystem::ParticleSystem(Particle &original, int intensity)
{
	particleCount = 0;
	modelParticle = original;
	this->intensity = intensity;
	lastUsed = 0;

	std::string vShader = "";
	vShader = getShader("partVert.txt");
	std::string gShader = "";
	gShader = getShader("partGeo.txt");
	std::string fShader = "";
	fShader = getShader("partFrag.txt");

	const char* vertex_shader = vShader.c_str();
	const char* geo_shader = gShader.c_str();
	const char* fragment_shader = fShader.c_str();

	GLuint vs = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vs, 1, &vertex_shader, nullptr);
	glCompileShader(vs);
	compileErrorPrint(&vs);

	GLuint gs = glCreateShader(GL_GEOMETRY_SHADER);
	glShaderSource(gs, 1, &geo_shader, nullptr);
	glCompileShader(gs);
	compileErrorPrint(&gs);

	GLuint fs = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fs, 1, &fragment_shader, nullptr);
	glCompileShader(fs);
	compileErrorPrint(&fs);

	particleShader = glCreateProgram();
	glAttachShader(particleShader, vs);
	glAttachShader(particleShader, gs);
	glAttachShader(particleShader, fs);
	glLinkProgram(particleShader);

	glGenBuffers(1, &particles);
	glBindBuffer(GL_ARRAY_BUFFER, particles);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * maxParticles * 8, NULL, GL_STREAM_DRAW);
}

int ParticleSystem::bindParticleSystem()
{
	int particlesInBuffer = 0;
	float* data = update(particlesInBuffer);

	glBindBuffer(GL_ARRAY_BUFFER, particles);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * maxParticles * 8, NULL, GL_STREAM_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(float) * 8 * particlesInBuffer, data);
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, (void*)0);
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(sizeof(float) * particlesInBuffer * 4));
	delete[]data;
	return particlesInBuffer;
}

float* ParticleSystem::update(int& dataInBuffer)
{
	for (int i = 0; i<particleCount; i++){
		system[i].life -= 1.0f + (0.5f - float(rand()%100)/100.0f);
		if (system[i].life > 0.0f){

			system[i].speed += vec3(0.0f, -9.81f, 0.0f)*(0.01f - float(rand() % 50) / 10000.0f);
			system[i].pos += system[i].speed * (1.1f - float(rand() % 200) / 1000.0f);
		}
		else
		{
			system[i] = system[particleCount - 1];
			particleCount--;
			i--;
		}
	}


	float* data = new float[particleCount * 8];
	for (int c = 0; c < particleCount; c++)
	{
		data[4 * c] = system[c].pos.x;
		data[4 * c + 1] = system[c].pos.y;
		data[4 * c + 2] = system[c].pos.z;
		data[4 * c + 3] = system[c].size;

		data[4 * (particleCount + c)] = system[c].color.r;
		data[4 * (particleCount + c) + 1] = system[c].color.g;
		data[4 * (particleCount + c) + 2] = system[c].color.b;
		data[4 * (particleCount + c) + 3] = system[c].color.a;
	}
	dataInBuffer = particleCount;
	for (int c = 0; c < intensity; c++)
	{
		if (particleCount < maxParticles)
		{
			createParticle();
		}
	}
	return data;
}

std::string ParticleSystem::getShader(const char* filePath)
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

GLuint ParticleSystem::getShaderProgram()
{
	return particleShader;
}

void ParticleSystem::createParticle()
{
	system[particleCount] = modelParticle;
	vec3 randvec(0.5f - float((rand() % 1000) / 1000.0f), 0, 0.5f - float((rand() % 1000) / 1000.0f));
	randvec = randvec*0.5f;
	system[particleCount].speed += randvec;
	particleCount++;
}

#include <vector>
void ParticleSystem::compileErrorPrint(GLuint* shader)
{
	GLint success;
	glGetShaderiv(*shader, GL_COMPILE_STATUS, &success);
	if (success == GL_FALSE)
	{
		GLint maxLength = 0;
		glGetShaderiv((*shader), GL_INFO_LOG_LENGTH, &maxLength);

		// The maxLength includes the NULL character
		std::vector<GLchar> errorLog(maxLength);
		glGetShaderInfoLog((*shader), maxLength, &maxLength, &errorLog[0]);

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