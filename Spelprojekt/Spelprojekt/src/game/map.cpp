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

	GameObject poop;
	poop.id = 1;
	poop.moveTo(5, 2, 5);
	quadTree->addObject(poop);
	GameObject poop2;
	poop2.id = 1;
	poop2.moveTo(-5, 2, 5);
	quadTree->addObject(poop2);
}


vector<GameObject> Map::getObjects() const
{
	return quadTree->checkQuadTree();
}

const GameObject* Map::getBackground() const
{
	return background;
}