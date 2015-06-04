#ifndef MAP_H
#define MAP_H
#include "../engine/QuadTree.h"
#include "gameObject.h"
#include "../engine/CameraControl.h"
#include <vector>

using std::vector;

class Map
{
private:
	QuadTree* quadTree;
	GameObject* background;
public:
	Map(){};
	~Map();
	void init(CameraControl* cc);
	vector<GameObject> getObjects() const;
	const GameObject* getBackground() const;

	QuadTree* getQuadTree() const
	{
		return quadTree;
	}
};

#endif