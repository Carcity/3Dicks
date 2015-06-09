#include "object.h"

Object::~Object()
{
	if (vertexHost)
	{
		glDeleteBuffers(1, &vertexData);
		glDeleteBuffers(1, &indexBuffer);
		glDeleteVertexArrays(1, &vertexAttribute);
	}
	if (textureHost)
		glDeleteTextures(1, &textureId);
}

Object::Object(std::string pathVert, std::string pathTex, Object* obj, bool copyVert, bool copyTex)
{
	if (copyVert)
	{
		vertexData = obj->vertexData;
		indexBuffer = obj->indexBuffer;
		vertexAttribute = obj->vertexAttribute;
		faceCount = obj->faceCount;
	}
	else
		if (!loadVert(pathVert))
			throw;
		else
			vertexHost = true;
	/*
	if (copyTex)
	{
		textureId = obj->textureId;
		TEXTUREINDEXOFFSET = obj->TEXTUREINDEXOFFSET;
	}
	else
		if (!loadBMP(pathTex))
			throw;
		else
			textureHost = true;*/
}

Object::Object(std::string pathTex)
{
}

Object::Object(const Object& obj)
{
	vertexHost = obj.vertexHost;
	textureId = obj.textureId;
	textureHost = obj.textureHost;
	TEXTUREINDEXOFFSET = obj.TEXTUREINDEXOFFSET;
	faceCount = obj.faceCount;

	vertexData = obj.vertexData;
	indexBuffer = obj.indexBuffer;
	vertexAttribute = obj.vertexAttribute;

	vert = obj.vert;
	uv = obj.uv;
	Indices = obj.Indices;

	count = obj.count;
	size = obj.size;
	state = obj.state;
}

void Object::bind()
{
	glActiveTexture(GL_TEXTURE0 + TEXTUREINDEXOFFSET);
	glBindTexture(GL_TEXTURE_2D, textureId);
	glBindVertexArray(vertexAttribute);
	glBindBuffer(GL_ARRAY_BUFFER, vertexData);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer);
}

bool Object::loadVert(std::string path)
{
	std::string line;
	std::ifstream myfile(path);
	if (myfile.is_open())
	{

		std::string sub;
		std::string imgFile = "";

		while (true) {
			if (!(getline(myfile, line))) break;
			if (line.size() < 5)
				continue;

			//checks for mtllib
			if (line[0] == 'm')
			{
				std::istringstream iss(line);
				std::string token;
				iss >> token;
				if (token == "mtllib");
				{
					//now we know it's an mtllib, and not some other whatever
					iss >> token;
					imgFile = token;
					loadMat(imgFile);
				}
			}

			if (line[0] == 'u')
			{
				std::istringstream iss(line);
				std::string token;
				iss >> token;
				if (token == "usemtl")
				{

				}
			}
			if (line[0] == 'v' && line[1] == ' ') // vertex pos
			{
				vert.push_back(TriangleVertex());

				std::istringstream iss(line);
				std::string sub;
				iss >> sub; // discard 'v'
				iss >> sub;
				vert[count].x = std::stof(sub);
				iss >> sub;
				vert[count].y = std::stof(sub);
				iss >> sub;
				vert[count].z = std::stof(sub);
				vert[count].u = -1.0f;
				vert[count].v = -1.0f;
				size++;
				count++;
			}
			else if (line[0] == 'v' && line[1] == 't') //UV cord
			{
				if (state != 1)
				{
					state = 1;
					count = 0;
				}
				uv.push_back(TriangleVertex());
				std::istringstream iss(line);
				std::string sub;
				iss >> sub; // discard 'vt'
				iss >> sub;
				uv[count].u = std::stof(sub);
				iss >> sub;
				uv[count].v = std::stof(sub);
				count++;
			}
			else if (line[0] == 'f') // face
			{
				if (state != 2)
				{
					state = 2;
					count = 0;
				}
				std::istringstream iss(line);
				std::string sub;
				Indices.push_back(0);
				Indices.push_back(0);
				Indices.push_back(0);
				//vert
				iss >> sub; // discard 'f'

				for (int n = 0; n < 3; n++)
				{
					std::string pos;
					iss >> pos; // vertex index
					iss >> sub; // uv index
					int indexVERT = std::stoi(pos) - 1;
					int indexUV = std::stoi(sub) - 1;
					if (vert[indexVERT].u < 0)
					{
						Indices[count * 3 + n] = indexVERT; // set vertex index
						vert[indexVERT].u = uv[indexUV].u;
						vert[indexVERT].v = uv[indexUV].v;
					}
					else
					{
						int temp = indexVERT;
						indexVERT = vert.size();
						vert.push_back(TriangleVertex());
						vert[indexVERT] = vert[temp];
						vert[indexVERT].u = uv[indexUV].u;
						vert[indexVERT].v = uv[indexUV].v;
						Indices[count * 3 + n] = indexVERT;
					}
					iss >> sub; // normal index
				}
				count++;
			}
		}

		glGenBuffers(1, &vertexData);
		glBindBuffer(GL_ARRAY_BUFFER, vertexData);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vert[0])* vert.size(), &vert[0], GL_STATIC_DRAW);

		glGenBuffers(1, &indexBuffer);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(Indices[0]) * count * 3, &Indices[0], GL_STATIC_DRAW);
		faceCount = count;
		glEnableVertexAttribArray(indexBuffer);

		//define vertex data layout
		glGenVertexArrays(1, &vertexAttribute);
		glBindVertexArray(vertexAttribute);
		glEnableVertexAttribArray(0);
		glEnableVertexAttribArray(1);
		//pos
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Object::TriangleVertex), BUFFER_OFFSET(0));
		//uv
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Object::TriangleVertex), BUFFER_OFFSET(sizeof(float) * 3));
		return true;
	}
	else //file not found
		return false;
}

bool Object::loadMat(std::string path)
{
	float r, g, b;
	r = g = b = 0;
	std::ifstream in(path);
	std::string imgFile = "";
	if (in.is_open())
	{
		std::string line;
		while (true)
		{
			if (!(getline(in, line))) break;
			if (line.size() < 5)
				continue;

			if (line[0] == 'K' && line[1] == 'd')
			{
				std::istringstream iss(line);
				std::string token;

				iss >> token;

				if (token == "Kd")
				{

					iss >> token;
					r = atof(token.c_str()) * 255;

					iss >> token;
					g = atof(token.c_str()) * 255;

					iss >> token;

					b = atof(token.c_str()) * 255;
				}
			}
			
			if (line[0] == 'm')
			{
				std::istringstream iss(line);
				std::string token;

				iss >> token;

				if (token == "map_Kd")
				{
					iss >> token;
					imgFile = token;
					
				}
				
			}

		}
	}
	if (!loadBMP(imgFile))
	{
		unsigned char data[3];
		data[0] = b;

		
		data[1] = g;//ss.str().c_str()[0];

	
		data[2] = r;// ss.str().c_str()[0];


		// Create one OpenGL texture
		glGenTextures(1, &textureId);

		glActiveTexture(GL_TEXTURE0 + TEXTUREINDEXOFFSET);

		// "Bind" the newly created texture : all future texture functions will modify this texture
		glBindTexture(GL_TEXTURE_2D, textureId);

		// Give the image to OpenGL
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 1, 1, 0, GL_BGR, GL_UNSIGNED_BYTE, data);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	}

	textureHost = true;
	return true;
}

bool Object::loadBMP(std::string imagepath)
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

	glActiveTexture(GL_TEXTURE0 + TEXTUREINDEXOFFSET);

	// "Bind" the newly created texture : all future texture functions will modify this texture
	glBindTexture(GL_TEXTURE_2D, textureId);

	// Give the image to OpenGL
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_BGR, GL_UNSIGNED_BYTE, data);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

	delete[] data;

	return true;
}

int Object::getFaces()
{
	return faceCount;
}

void Object::updateVAO(std::vector<TriangleVertex> someVerts, std::vector<GLushort> someIndices)
{
	glBindBuffer(GL_ARRAY_BUFFER, vertexData);
	glBufferData(GL_ARRAY_BUFFER, sizeof(someVerts[0])* someVerts.size(), &someVerts[0], GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(someIndices[0]) * someIndices.size(), &someIndices[0], GL_STATIC_DRAW);
	faceCount = count;
	glEnableVertexAttribArray(indexBuffer);

	glBindVertexArray(vertexAttribute);
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	//pos
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(TriangleVertex), BUFFER_OFFSET(0));
	//uv
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(TriangleVertex), BUFFER_OFFSET(sizeof(float) * 3));
}