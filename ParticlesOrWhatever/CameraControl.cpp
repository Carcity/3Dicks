#include "CameraControl.h"
#include <Windows.h>

CameraControl::CameraControl()
{
	side = up = 0.0f;
	position = vec3(0, 2, 0);
	direction = vec3(0, -2, 0); //Var vi tittar

	right = glm::vec3( //Vektor som pekar rakt ut till höger från kameran
		sin(side - 3.14f / 2.0f),
		0,
		cos(side - 3.14f / 2.0f)
		);
	head = cross(right, direction);
}

void CameraControl::updateVectors()
{
	const HWND hDesktop = GetDesktopWindow();
	RECT desktop; //#rekt
	GetWindowRect(hDesktop, &desktop);
	POINT pCur;
	GetCursorPos(&pCur);
	SetCursorPos(desktop.right / 2, desktop.bottom / 2);
	up += float(desktop.bottom / 2 - pCur.y) *0.0002f;
	if (up < -3.14f / 2.0f)
		up = -3.14f / 2.0f;
	if (up > 3.14f / 2.0f)
		up = 3.14f / 2.0f;
	side += float(desktop.right / 2 - pCur.x) *0.0002f;
	direction = vec3( //Var vi tittar
		cos(up) * sin(side),
		sin(up),
		cos(up) * cos(side)
		);
	right = glm::vec3( //Vektor som pekar rakt ut till höger från kameran
		sin(side - 3.14f / 2.0f),
		0,
		cos(side - 3.14f / 2.0f)
		);
	head = cross(right, direction);

	if (GetKeyState('A') && GetAsyncKeyState('A'))
	{
		position -= right * 0.2f;
	}
	if (GetKeyState('D') && GetAsyncKeyState('D'))
	{
		position += right * 0.2f;
	}
	if (GetKeyState('S') && GetAsyncKeyState('S'))
	{
		position -= direction * 0.2f;
	}
	if (GetKeyState('W') && GetAsyncKeyState('W'))
	{
		position += direction * 0.2f;
	}
}

mat4 CameraControl::getViewMatrix()
{
	updateVectors();
	return lookAt(
		position,
		position + direction,
		head);
}

vec3 CameraControl::getPosition()
{
	return position;
}

vec3 CameraControl::getDirection()
{
	return direction;
}

vec3 CameraControl::getRightVec()
{
	return right;
}

vec3 CameraControl::getUpVec()
{
	return head;
}

void CameraControl::setPosition(vec3 position)
{
	this->position = position;
}