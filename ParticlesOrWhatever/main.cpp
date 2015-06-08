//--------------------------------------------------------------------------------------
// BTH - Stefan Petersson 2014.
//--------------------------------------------------------------------------------------
#include <windows.h>

#include <gl/glew.h>
#include <gl/GL.h>
#include "bth_image.h"
#include "glm/glm/glm.hpp"
#include "glm/glm/gtx/transform.hpp"

#include <vector>
#include <fstream>
#include <time.h>

#pragma comment(lib, "opengl32.lib")
#pragma comment(lib, "glew32.lib")

#include "CameraControl.h"
#include "ParticleSystem.h"

using namespace glm;

HWND InitWindow(HINSTANCE hInstance);
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
HGLRC CreateOpenGLContext(HWND wndHandle);

mat4 Projection;

CameraControl camera;
ParticleSystem psys[2];


void linkErrorPrint(GLuint* shaderProgram)
{
	GLint success;
	glGetProgramiv(*shaderProgram, GL_LINK_STATUS, &success);
	if (success == GL_FALSE)
	{
		GLint maxLength = 0;
		glGetProgramiv(*shaderProgram, GL_INFO_LOG_LENGTH, &maxLength);

		// The maxLength includes the NULL character
		std::vector<GLchar> errorLog(maxLength);
		glGetProgramInfoLog(*shaderProgram, maxLength, &maxLength, &errorLog[0]);

		std::fstream myfile;
		myfile.open("errorLinkCheck.txt", std::fstream::out);
		for (int i = 0; i < maxLength; i++)
		{
			myfile << errorLog[i];
		}
		myfile.close();

		// Provide the infolog in whatever manor you deem best.
		// Exit with failure.
		glDeleteProgram(*shaderProgram); // Don't leak the shader.
		throw;
	}
}

void CameraSetup()
{
	Projection = perspective(3.14159f*0.45f, 640.0f / 480.0f, 0.5f, 100.0f); //Projection matrix
}

void SetViewport()
{
	glViewport(0, 0, 640, 480);
}

void Render()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	for (int c = 0; c < 2; c++)
	{
		glDepthMask(GL_FALSE);
		GLuint prog = psys[c].getShaderProgram();
		glUseProgram(prog);
		mat4 View = camera.getViewMatrix();
		GLuint loc = glGetUniformLocation(prog, "View");
		glUniformMatrix4fv(loc, 1, GL_FALSE, &View[0][0]);
		loc = glGetUniformLocation(prog, "Projection");
		glUniformMatrix4fv(loc, 1, GL_FALSE, &Projection[0][0]);
		loc = glGetUniformLocation(prog, "cRight");
		glUniform3fv(loc, 1, &camera.getRightVec()[0]);
		loc = glGetUniformLocation(prog, "cUp");
		glUniform3fv(loc, 1, &camera.getUpVec()[0]);

		int particles = psys[c].bindParticleSystem();

		glDrawArrays(GL_POINTS, 0, particles);
		glDepthMask(GL_TRUE);
	}
}

int WINAPI wWinMain( HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nCmdShow )
{
	MSG msg = { 0 };
	HWND wndHandle = InitWindow(hInstance); //1. Skapa fönster
	
	if (wndHandle)
	{
		HDC hDC = GetDC(wndHandle); 
		HGLRC hRC = CreateOpenGLContext(wndHandle); //2. Skapa och koppla OpenGL context
		srand(time(0));
		glewInit(); //3. Initiera The OpenGL Extension Wrangler Library (GLEW)
		glEnable(GL_DEPTH_TEST);
		glDepthMask(GL_TRUE);
		glDepthFunc(GL_LEQUAL);

		glEnable(GL_BLEND);
		glBlendFunc(GL_ONE, GL_ONE);

		SetViewport(); //4. Sätt viewport
		Particle model(vec3(0, 0, 0), vec3(0, 0.5, 0), vec4(0.1, 0.1, 0.005, 1), 25.0f, 0.3f);
		psys[0] = ParticleSystem(model, 300);

		model = Particle(vec3(5, 1, 0), vec3(-0.6, 0.6, 0), vec4(0.005, 0.01, 0.01, 1), 20.0f, 0.4f);
		psys[1] = ParticleSystem(model, 200);
		GLuint prog = psys[0].getShaderProgram();
		linkErrorPrint(&prog);

		CameraSetup();
		glClearColor(0, 0, 0, 1);
		ShowWindow(wndHandle, nCmdShow);

		while (WM_QUIT != msg.message)
		{
			if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
			{
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}

			else
			{
				Render(); //9. Rendera
				SwapBuffers(hDC); //10. Växla front- och back-buffer
				Sleep(20);
			}
		}

		wglMakeCurrent(NULL, NULL);
		ReleaseDC(wndHandle, hDC);
		wglDeleteContext(hRC);
		DestroyWindow(wndHandle);
	}

	return (int) msg.wParam;
}

HWND InitWindow(HINSTANCE hInstance)
{
	WNDCLASSEX wcex = { 0 };
	wcex.cbSize = sizeof(WNDCLASSEX); 
	wcex.style          = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc    = WndProc;
	wcex.hInstance      = hInstance;
	wcex.lpszClassName = L"WTFISTHIS";
	if( !RegisterClassEx(&wcex) )
		return false;

	RECT rc = { 0, 0, 640, 480 };
	AdjustWindowRect( &rc, WS_OVERLAPPEDWINDOW, FALSE );
	
	HWND handle = CreateWindow(
		L"WTFISTHIS",
		L"HELP ME WHATS HAPPENING",
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

LRESULT CALLBACK WndProc( HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam )
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
		PFD_DOUBLEBUFFER |               // double buffered
		PFD_TYPE_RGBA,                    // RGBA type  
		32,                               // 32-bit color depth  
		0, 0, 0, 0, 0, 0,                 // color bits ignored  
		0,                                // no alpha buffer  
		0,                                // shift bit ignored  
		0,                                // no accumulation buffer  
		0, 0, 0, 0,                       // accum bits ignored  
		0,                                // 0-bits for depth buffer <-- modified by Stefan      
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