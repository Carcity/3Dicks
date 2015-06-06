#ifndef QUADTREE_H
#define QUADTREE_H

#include "../game/gameObject.h"
#include "CameraControl.h"
#include <vector>

using namespace glm;
using std::vector;

#define NWU 0
#define NEU 1
#define SEU 2
#define SWU 3
#define NWD 4
#define NED 5
#define SED 6
#define SWD 7

struct Quad
{
	vec3 corners[8];
	bool leaf;
	Quad* children[4];
	int nrOfObjects;
	int maxObj;
	GameObject* objects[10];
	Quad()
	{
		leaf = false;
		for (int c = 0; c < 4; c++)
		{
			children[c] = nullptr;
		}
		nrOfObjects = 0;
		maxObj = 10;
	}
	vec3 getCenter()
	{
		vec3 add = vec3(0, 0, 0);
		for (int c = 0; c < 8; c++)
		{
			add = vec3(add.x + corners[c].x, add.y + corners[c].y, add.z + corners[c].z);
		}
		vec3 center = vec3(add.x / 8, add.y / 8, add.z / 8);
		return center;
	}
	void addObject(GameObject &obj)
	{
		if (nrOfObjects != maxObj)
		{
			objects[nrOfObjects] = new GameObject(obj);
			nrOfObjects++;
		}
	}
	~Quad()
	{
		if (!leaf)
		{
			for (int c = 0; c < 4; c++)
			{
				delete children[c];
			}
		}
	}
};

struct Plane
{
	vec3 point, normal;
	vec3 center;
	float d;
	Plane()
	{
		point = normal = vec3(0, 0, 0);
		d = 0.0f;
	}
	Plane(vec3 v1, vec3 v2, vec3 v3)
	{
		vec3 aux1, aux2;
		
		aux1 = v1 - v2;
		aux2 = v3 - v2;

		center = vec3((v1.x + v3.x) * 0.5f, (v1.y + v3.y) * 0.5f, (v1.z + v3.z) * 0.5f);
		normal = cross(aux2, aux1);

		normal = normalize(normal);
		point = v2;
		d = -(normal.x * point.x + normal.y * point.y + normal.z * point.z);
	}
	float distance(vec3 p)
	{
		return ((normal.x * p.x) + (normal.y * p.y) + (normal.z * p.z));
	}
};

class QuadTree
{
private:
	CameraControl* cam;
	Quad root;
	int levels;
	void createQuads(Quad* prevRoot, int level);
	void checkQuads(Quad* quad, Plane pl[6], vector<GameObject>* ret);
	void copyChildren(Quad* quad, const Quad* copy);
public:
	QuadTree();
	QuadTree(int levels, vec3 rootBox[8], CameraControl* cc);
	bool addObject(GameObject &obj);
	vector<GameObject> checkQuadTree();
	QuadTree& operator=(const QuadTree& copy);

	void getFrustumCorners(vec3 container[]);
};

#endif