#include "obj.h"


Obj::Obj(const char* name, int id)
{
	this->id = id;
	std::string temp = name;
	temp.append(".v");
	std::string temp2 = name;
	temp2.append(".bmp");
	
	//load triangle data
	if (!loadVert(temp))
		throw;
	//load texture data
	if (!loadBMP(temp2))
		throw;
}

bool Obj::loadBMP(const std::string imagepath)
{
	unsigned char header[54]; // Each BMP file begins by a 54-bytes header
	unsigned int dataPos;     // Position in the file where the actual data begins
	unsigned int width, height;
	unsigned int imageSize;   // = width*height*3
	// Actual RGB data
	unsigned char * data;

	FILE* file = fopen(imagepath.c_str(), "rb");
	if (!file)
		return false;

	if (fread(header, 1, 54, file) != 54) // If not 54 bytes read : problem
		return false;

	if (header[0] != 'B' || header[1] != 'M')
		return false;

	// Read ints from the byte array
	dataPos = *(int*)&(header[0x0A]);
	imageSize = *(int*)&(header[0x22]);
	width = *(int*)&(header[0x12]);
	height = *(int*)&(header[0x16]);

	// Some BMP files are misformatted, guess missing information
	if (imageSize == 0)
		imageSize = width*height * 3; // 3 : one byte for each Red, Green and Blue component
	if (dataPos == 0)
		dataPos = 54; // The BMP header is done that way

	// Create a buffer
	data = new unsigned char[imageSize];

	// Read the actual data from the file into the buffer
	fread(data, 1, imageSize, file);

	//Everything is in memory now, the file can be closed
	fclose(file);

	// Create one OpenGL texture
	glGenTextures(1, &textureId);

	glActiveTexture(GL_TEXTURE0);

	// "Bind" the newly created texture : all future texture functions will modify this texture
	glBindTexture(GL_TEXTURE_2D, textureId);

	// Give the image to OpenGL
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_BGR, GL_UNSIGNED_BYTE, data);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

	delete[] data;

#ifdef _DEBUG
	{GLenum err = glGetError(); if (err)
		int x = 0; }
#endif

	return true;
}

bool Obj::loadVert(const std::string vertdatapath)
{
	std::string line;
	std::ifstream myfile(vertdatapath);
	if (myfile.is_open())
	{
		getline(myfile, line);
		int vert_count = atoi(line.c_str());
		TriangleVertex* vert = new TriangleVertex[vert_count];

		for (int n = 0; n < vert_count; n++)
		{
			//read vert data, seperate with space
			getline(myfile, line);
			std::istringstream iss(line);
			std::string sub;
			//vert
			iss >> sub;
			vert[n].x = std::stof(sub);
			iss >> sub;
			vert[n].y = std::stof(sub);
			iss >> sub;
			vert[n].z = std::stof(sub);
			getline(myfile, line);
			std::istringstream iss2(line);
			iss2.str(line);
			//uv
			iss2 >> sub;
			vert[n].u = std::stof(sub);
			iss2 >> sub;
			vert[n].v = std::stof(sub);
		}
		//load index buffer (faces)
		getline(myfile, line);
		int face_count = atoi(line.c_str());
		GLushort* Indices = new GLushort[face_count * 3];

		for (int n = 0; n < face_count; n++)
		{
			//read index data, seperate with space
			getline(myfile, line);
			std::istringstream iss(line);
			std::string sub;
			//vert
			iss >> sub;
			Indices[n * 3] = std::stoi(sub)-1;
			iss >> sub;
			Indices[n * 3 + 1] = std::stoi(sub) - 1;
			iss >> sub;
			Indices[n * 3 + 2] = std::stoi(sub) - 1;
		}
		
		glGenBuffers(1, &vertexDataId);
		glBindBuffer(GL_ARRAY_BUFFER, vertexDataId);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vert[0])* vert_count, vert, GL_STATIC_DRAW);
		vertCount = vert_count;
		delete[] vert;

		glGenBuffers(1, &IndexBufferId);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IndexBufferId);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(Indices[0]) * face_count * 3, Indices, GL_STATIC_DRAW);
		faceCount = face_count;
		delete [] Indices;
		glEnableVertexAttribArray(IndexBufferId);

		//define vertex data layout
		glGenVertexArrays(1, &gVertexAttribute);
		glBindVertexArray(gVertexAttribute);
		glEnableVertexAttribArray(0); //the vertex attribute object will remember its enabled attributes
		glEnableVertexAttribArray(1);

		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Obj::TriangleVertex), BUFFER_OFFSET(0));
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Obj::TriangleVertex), BUFFER_OFFSET(sizeof(float) * 3));
#ifdef _DEBUG
		{GLenum err = glGetError(); if (err)
			int x = 0; }
#endif
	}
	else
		return false;
	myfile.close();
	return true;
	
}

int Obj::Bind()
{
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, textureId);
	glBindVertexArray(gVertexAttribute);
	glBindBuffer(GL_ARRAY_BUFFER, vertexDataId);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IndexBufferId);
#ifdef _DEBUG
	{GLenum err = glGetError(); if (err)
		int x = 0; }
#endif
	return 1;
}

void Obj::rotate(float x, float y, float z, bool local)
{
	if (local == false) //rotate order x, y, z	
	{
		rotx += toRad * x;
		roty += toRad * y;
		rotz += toRad * z;

		//rot Y
		worldMatrix *= glm::mat4(cos(y), 0.0f, -sin(y), 0.0f,
			0.0f, 1.0f, 0.0f, 0.0f, 
			sin(y), 0.0f, cos(y), 0.0f,
			0.0f, 0.0f, 0.0f, 1.0f);
	}

	glm::mat4 normalMatrix = glm::inverse(glm::transpose(worldMatrix));
}