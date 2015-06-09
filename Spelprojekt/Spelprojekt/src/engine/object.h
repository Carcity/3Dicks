#ifndef OBJECT_H
#define OBJECT_H

#include <string>

#include <GL\glew.h>
#include <GLFW\glfw3.h>

#include <fstream>
#include <iostream>
#include <vector>
#include <string>
#include <sstream>
#define BUFFER_OFFSET(i) ((char *)nullptr + (i))

class Object
{
private:
	
	bool vertexHost = false;
	GLuint textureId = 0;
	bool textureHost = false;
	int TEXTUREINDEXOFFSET = 0;
	int faceCount = -1;
	

	//temprary implementations
	bool loadVert(std::string path);
	bool loadBMP(std::string path); 
	bool loadMat(std::string path);

public:
	GLuint vertexData = 0;
	GLuint indexBuffer = 0;
	GLuint vertexAttribute = 0;

	struct TriangleVertex
	{
		float x, y, z;
		float u, v;
	};

	std::vector<TriangleVertex> vert;
	std::vector<TriangleVertex> uv;
	std::vector<GLushort> Indices;

	int count = 0;
	int size = 0;
	int state = 0;

	Object() {}
	Object(std::string pathTex);
	Object(std::string pathVert, std::string pathTex, Object* obj = 0, bool copyVert = false, bool copyTex = false);
	Object(const Object& obj);
	~Object();
	void bind();
	int getFaces();

	void updateVAO(std::vector<TriangleVertex> someVerts, std::vector<GLushort> someIndices);
};

#endif