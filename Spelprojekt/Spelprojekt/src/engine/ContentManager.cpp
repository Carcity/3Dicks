#include "ContentManager.h"

ContentManager::~ContentManager()
{
	if (player)
		delete player;
	if (mapObjs)
	{
		for (int n = 0; n < mapObjCount; n++)
		{
			delete mapObjs[n];
		}
		delete[] mapObjs;
	}
}

void ContentManager::init()
{
	//load all player meshes
	loadPlayer();

	//load all map meshes
	loadMapObjs();

	//load all monster meshes

}

void ContentManager::loadPlayer()
{
	if (player) //only call once
		throw;
	//this gon be two objects
	Object** playerAn = new Object*[2];
	playerAn[0] = new Object("src/meshes/PlayerBase.v", "src/textures/black.bmp");
	playerAn[1] = new Object("src/meshes/PlayerWalk.v", "", playerAn[0], false, true);
	player = new Object("src/meshes/PlayerWalk.v", "src/textures/black.bmp");//new Animation(playerAn, 2, .5f);// = new Object("src/meshes/PlayerBase.v", "src/textures/HEIL.bmp");
}

void ContentManager::loadMapObjs()
{
	if (mapObjs) //only call once
		throw; 

	mapObjCount = 7; 
	mapObjs = new Object*[mapObjCount]();

	mapObjs[0] = new Object("src/meshes/PlaneVerticalF.v", "src/textures/grid.bmp");
	mapObjs[1] = new Object("src/meshes/BaseCubeColored.v", "src/textures/black.bmp");
	mapObjs[2] = new Object("src/meshes/BackWall.v", "src/textures/black.bmp");
	mapObjs[3] = new Object("src/meshes/GrimStatue2.v", "src/textures/black.bmp");
	mapObjs[4] = new Object("src/meshes/OrangeCube.v", "src/textures/black.bmp");
	mapObjs[5] = new Object("src/meshes/PlaneHorizontal.v", "src/textures/black.bmp");
	mapObjs[6] = new Object("src/meshes/SideBarrier.v", "src/textures/black.bmp");

}

int ContentManager::bindPlayer() const
{
	//player->updateWeight();
	player->bind();
	return player->getFaces();
}

int ContentManager::bindMapObj(int id) const
{
	mapObjs[id]->bind();
	return mapObjs[id]->getFaces();
}