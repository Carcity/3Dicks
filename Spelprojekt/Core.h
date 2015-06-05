#ifndef CORE_H
#define CORE_H

#include "UserInput.h"
#include "Render.h"

class Core
{
private:
	void MakeLights();
	void InitShadowMaps();
public:
	Core();
	Render* rend;
	int update();
	unsigned int objectCounter = 0;
	unsigned int WINDOW_WITH = 640;
	unsigned int WINDOW_HEIGHT = 480;
};

#endif
