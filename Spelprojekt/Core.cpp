#include "Core.h"

Core::Core()
{
	rend = new Render();
	//need better way to set camera start
	rend->in = new UserInput(&rend->viewMatrix, glm::vec3(0, 0, -4), glm::vec3(0, 0, -3), glm::vec3(0, 1, 0));
#ifdef _DEBUG
	{GLenum err = glGetError(); if (err)
		int x = 0; }
#endif
}

int Core::update()
{
	//init
	if (rend->obj == 0)
	{
		rend->obj = new Object("PlaneHorizontal.v", "");
		rend->obj2 = new Object("sphere2.v", "");
		MakeLights();
	}

	//rotate light
	rend->rottemp = 0.01;
	glm::mat3 rot = glm::mat3(cos(rend->rottemp), 0.0f, -sin(rend->rottemp), 0.0f, 1.0f, 0.0f, sin(rend->rottemp), 0.0f, cos(rend->rottemp));
	rend->spotLights[0].Position = rend->spotLights[0].Position* rot;
	rend->spotLights[0].Direction = rend->spotLights[0].Direction* rot;

	//shadowmap pass
	glCullFace(GL_FRONT);
	rend->ShadowMapPass();
	glCullFace(GL_BACK);
	
	rend->render();

#ifdef _DEBUG
	{GLenum err = glGetError();if (err)
			int x = 0;}
#endif
	return 1;
}

void Core::MakeLights()
{
	rend->nrSpotLights = 1;
	//set lights
	rend->spotLights = new SpotLight[rend->nrSpotLights];

	//one new ligt
	rend->spotLights[0].Color = vec3(1.0f, 1.0f, 1.0f); 
	rend->spotLights[0].Position = vec3(5.0f, 2.0f, 5.0f);
	rend->spotLights[0].Direction = normalize(vec3(-3.0f, -1.0f, -3.0f));
	rend->spotLights[0].DiffuseIntensity = 1.00f;
	rend->spotLights[0].AmbientIntensity = 0.2f;
	rend->spotLights[0].Cutoff = 0.90f;

#ifdef _DEBUG
	{GLenum err = glGetError(); if (err)
		int x = 0; }
#endif

	//make light 2
	//rend->spotLights[1].Color = vec3(0.0f, 0.0f, 1.0f);
	//rend->spotLights[1].Position = vec3(2.0f, 4.0f, 2.0f);

	////make light 3
	//rend->spotLights[2].Color = vec3(0.0f, 0.0f, 1.0f);
	//rend->spotLights[2].Position = vec3(1.0f, 3.0f, 1.0f);

	//rend->spotLights[3].Color = vec3(0.0f, 1.0f, 1.0f);
	//rend->spotLights[3].Position = vec3(2.5f, 3.0f, 2.5f);

	//rend->spotLights[4].Color = vec3(1.0f, 0.0f, 1.0f);
	//rend->spotLights[4].Position = vec3(-2.5f, 3.0f, -2.5f);

	//rend->spotLights[5].Color = vec3(1.0f, 1.0f, 0.0f);
	//rend->spotLights[5].Position = vec3(2.5f, 3.0f, -2.5f);

	//rend->spotLights[6].Color = vec3(1.0f, 1.0f, 1.0f);
	//rend->spotLights[6].Position = vec3(-2.5f, 3.0f, 2.5f);
	//
}

//void Core::InitShadowMaps()
//{
//	rend->nrSpotLightsShadow = 1;
//
//	rend->shadowMap = new ShadowMapFBO[rend->nrSpotLightsShadow];
//
//	for (int n = 0; n < rend->nrSpotLightsShadow; n++)
//		rend->shadowMap[0].Init(WINDOW_WITH, WINDOW_HEIGHT);
//}
