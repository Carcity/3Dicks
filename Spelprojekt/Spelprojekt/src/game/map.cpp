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

	worldObjs = new GameObject*[1];
	worldObjs[0] = new GameObject();
	worldObjs[0]->id = 1;
	size = 1;

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

