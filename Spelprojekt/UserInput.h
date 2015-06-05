#ifndef USER_INPUT
#define USER_INPUT

#include "glm\glm\glm.hpp"
#include "glm\glm\gtc\matrix_transform.hpp"
#include "glm\glm\gtc\quaternion.hpp"

using namespace glm;

class UserInput
{
private:
	mat4* viewMatrix;
	vec3 pos;
	vec3 target;
	float angleH;
	float angleV;
	vec3 up;

	mat3 rotH;
	mat3 rotV;

	float speed;

	bool space;
	bool ctrl;
	bool shift;
	bool W;
	bool A;
	bool S;
	bool D;

	float toDegree(float val);
	float toRadian(float val);
public:
	UserInput(mat4* mat, vec3 posIn, vec3 lookIn, vec3 upIn);
	void KeyDown(char c);
	void KeyUp(char c);
	void Mouse(int x, int y);
	void Act();
	void Shift(bool);
	void Space(bool);
	void Ctrl(bool);
	bool getShift();
	void Init();
	void Update();
	
	vec3* GetPos();
};

#endif