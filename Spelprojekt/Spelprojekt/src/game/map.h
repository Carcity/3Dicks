#ifndef MAP_H
#define MAP_H


#include "gameObject.h"
class Map
{
private:
	GameObject** worldObjs;
	GameObject* background;
	int size;

public:
	Map(){};
	~Map();
	void init();
	GameObject** getObjects() const;
	int getSize() const;
	const GameObject* getBackground() const;
};

#endif