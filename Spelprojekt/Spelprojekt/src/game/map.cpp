#include "map.h"

Map::~Map()
{
	delete quadTree;
}

void Map::init(CameraControl* cc)
{
	vec3 rootBox[8] = {
		vec3(-40, 40, 40), vec3(40, 40, 40), vec3(40, 40, -40), vec3(-40, 40, -40),
		vec3(-40, -40, 40), vec3(40, -40, 40), vec3(40, -40, -40), vec3(-40, -40, -40)
	};
	quadTree = new QuadTree(3, rootBox, cc);
	background = new GameObject();
	background->rotateTo(3.141592654f / 2.0f, 0, 0);
	background->id = 0;

	GameObject objs[60];
	int c = -30;
	for (int i = 0; i < 15; i++)
	{
		objs[i].id = 1;
			objs[i].moveTo(c, 2, 30);
		quadTree->addObject(objs[i]);
		c += 4;
	}
	c = -30;
	for (int i = 15; i < 30; i++)
	{
		objs[i].id = 1;
		objs[i].moveTo(30, 2, c);
		quadTree->addObject(objs[i]);
		c += 4;
	}
	c = -30;
	for (int i = 30; i < 45; i++)
	{
		objs[i].id = 1;
		objs[i].moveTo(-30, 2, c);
		quadTree->addObject(objs[i]);
		c += 4;
	}
	c = -30;
	for (int i = 45; i < 60; i++)
	{
		objs[i].id = 1;
		objs[i].moveTo(c, 2, -30);
		quadTree->addObject(objs[i]);
		c += 4;
	}
}


vector<GameObject> Map::getObjects() const
{
	return quadTree->checkQuadTree();
}

const GameObject* Map::getBackground() const
{
	return background;
}