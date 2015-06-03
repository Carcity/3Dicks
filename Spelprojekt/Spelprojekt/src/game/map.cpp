#include "map.h"

Map::~Map()
{
	delete background;
	for (int i = 0; i < size; i++)
		delete worldObjs[i];
	delete[] worldObjs;
}

void Map::init()
{
	background = new GameObject();
	background->id = 0;
	background->moveTo(0, 2, 0);

	worldObjs = new GameObject*[6];
	worldObjs[0] = new GameObject();
	worldObjs[0]->id = 1;
	worldObjs[1] = new GameObject();
	worldObjs[1]->id = 2;
	worldObjs[2] = new GameObject();
	worldObjs[2]->id = 3;
	worldObjs[3] = new GameObject();
	worldObjs[3]->id = 4;
	worldObjs[4] = new GameObject();
	worldObjs[4]->id = 5;
	worldObjs[5] = new GameObject();
	worldObjs[5]->id = 6;
	size = 6;

}


GameObject** Map::getObjects() const
{
	return worldObjs;
}

const GameObject* Map::getBackground() const
{
	return background;
}

int Map::getSize() const
{
	return size;
}

