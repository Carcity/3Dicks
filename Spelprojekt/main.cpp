//## Include ##
#include <windows.h>
#include "Core.h"
#include <vector>
#include <gl/glew.h>
#include <gl/GL.h>
#pragma comment(lib, "opengl32.lib")
#pragma comment(lib, "glew32.lib")
#include <ctime>
#include <cstdio>
#include <sstream>

//## Windows ##
HWND InitWindow(HINSTANCE hInstance);
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
HGLRC CreateOpenGLContext(HWND wndHandle);

//## User IN ##
#define WM_MOUSEMOVE 0x0200
POINT oldMpos;
bool setMposOnce = true;

Core* core;

void SetViewport(int, int, int, int);

int FPScount = 0;
clock_t start = clock();

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nCmdShow)
{
	MSG msg = { 0 };
	HWND wndHandle = InitWindow(hInstance); 

	if (wndHandle)
	{
		HDC hDC = GetDC(wndHandle);
		HGLRC hRC = CreateOpenGLContext(wndHandle); //Skapa och koppla OpenGL context

		glewInit(); //Initiera The OpenGL Extension Wrangler Library (GLEW)

		glEnable(GL_DEPTH_TEST); // enable depth buffer
		glDepthFunc(GL_LEQUAL);
		glDepthMask(GL_TRUE);
		glEnable(GL_CULL_FACE);
		glFrontFace(GL_CCW);


		SetViewport(0,0, 640, 480); //4. Sätt viewport

		core = new Core();

		core->rend->CreateShaders(); 

		ShowWindow(wndHandle, nCmdShow);

		GetCursorPos(&oldMpos);

		start = std::clock();

		while (WM_QUIT != msg.message)
		{
			if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
			{
				switch (msg.message)
				{
					case WM_MOUSEMOVE:
						if (core->rend->in->getShift())
						{
							POINT newMpos;
							GetCursorPos(&newMpos);
							core->rend->in->Mouse(newMpos.x - oldMpos.x, newMpos.y - oldMpos.y);
						}
						GetCursorPos(&oldMpos);
						break;
					case WM_KEYDOWN:
					{
						WPARAM param = msg.wParam;
						char c = MapVirtualKey(param, MAPVK_VK_TO_CHAR);
						core->rend->in->KeyDown(c);
						if (param == 16)
							core->rend->in->Shift(true);
						if (param == 32)
							core->rend->in->Space(true);
						if (param == 17)
							core->rend->in->Ctrl(true);
							break;
					}

					case WM_KEYUP:
					{
						WPARAM param = msg.wParam;
						char c = MapVirtualKey(param, MAPVK_VK_TO_CHAR);
						core->rend->in->KeyUp(c);
						if (param == 16)
							core->rend->in->Shift(false);
						if (param == 32)
							core->rend->in->Space(false);
						if (param == 17)
							core->rend->in->Ctrl(false);
						break;
					}
				}
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
			else
			{
				core->rend->in->Act();
				core->update();
				SwapBuffers(hDC); //10. Växla front- och back-buffer

				//show fps
				FPScount++;

				if ((std::clock() - start) / (double)CLOCKS_PER_SEC > 1)
				{
					start = std::clock();
					std::string s = std::to_string(FPScount);
					FPScount = 0;
					std::wstring stemp = std::wstring(s.begin(), s.end());
					LPCWSTR sw = stemp.c_str();
					SetWindowText(wndHandle, sw);
				}
			}
		}
		wglMakeCurrent(NULL, NULL);
		ReleaseDC(wndHandle, hDC);
		wglDeleteContext(hRC);
		DestroyWindow(wndHandle);
	}

	return (int)msg.wParam;
}

HWND InitWindow(HINSTANCE hInstance)
{
	WNDCLASSEX wcex = { 0 };
	wcex.cbSize = sizeof(WNDCLASSEX);
	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = WndProc;
	wcex.hInstance = hInstance;
	wcex.lpszClassName = L"BTH_GL_DEMO";
	if (!RegisterClassEx(&wcex))
		return false;

	RECT rc = { 0, 0, 640, 480 };
	AdjustWindowRect(&rc, WS_OVERLAPPEDWINDOW, FALSE);

	HWND handle = CreateWindow(
		L"BTH_GL_DEMO",
		L"BTH OpenGL Demo",
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		rc.right - rc.left,
		rc.bottom - rc.top,
		nullptr,
		nullptr,
		hInstance,
		nullptr);

	return handle;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	}
	return DefWindowProc(hWnd, message, wParam, lParam);
}

HGLRC CreateOpenGLContext(HWND wndHandle)
{
	//get handle to a device context (DC) for the client area
	//of a specified window or for the entire screen
	HDC hDC = GetDC(wndHandle);

	//details: http://msdn.microsoft.com/en-us/library/windows/desktop/dd318286(v=vs.85).aspx
	static  PIXELFORMATDESCRIPTOR pixelFormatDesc =
	{
		sizeof(PIXELFORMATDESCRIPTOR),    // size of this pfd  
		1,                                // version number  
		PFD_DRAW_TO_WINDOW |              // support window  
		PFD_SUPPORT_OPENGL |              // support OpenGL  
		PFD_DOUBLEBUFFER |                // double buffered
		//PFD_DEPTH_DONTCARE,               // disable depth buffer <-- added by Stefan
		PFD_TYPE_RGBA,                    // RGBA type  
		32,                               // 32-bit color depth  
		0, 0, 0, 0, 0, 0,                 // color bits ignored  
		0,                                // no alpha buffer  
		0,                                // shift bit ignored  
		0,                                // no accumulation buffer  
		0, 0, 0, 0,                       // accum bits ignored  
		//0,                                // 0-bits for depth buffer <-- modified by Stefan      
		0,                                // no stencil buffer  
		0,                                // no auxiliary buffer  
		PFD_MAIN_PLANE,                   // main layer  
		0,                                // reserved  
		0, 0, 0                           // layer masks ignored  
	};

	//attempt to match an appropriate pixel format supported by a
	//device context to a given pixel format specification.
	int pixelFormat = ChoosePixelFormat(hDC, &pixelFormatDesc);

	//set the pixel format of the specified device context
	//to the format specified by the iPixelFormat index.
	SetPixelFormat(hDC, pixelFormat, &pixelFormatDesc);

	//create a new OpenGL rendering context, which is suitable for drawing
	//on the device referenced by hdc. The rendering context has the same
	//pixel format as the device context.
	HGLRC hRC = wglCreateContext(hDC);

	//makes a specified OpenGL rendering context the calling thread's current
	//rendering context. All subsequent OpenGL calls made by the thread are
	//drawn on the device identified by hdc. 
	wglMakeCurrent(hDC, hRC);

	return hRC;
}

void SetViewport(int x1, int y1, int x2, int y2)
{
	glViewport(x1, y1, x2, y2);
}
