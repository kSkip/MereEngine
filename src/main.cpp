#include "Platform.h"

#include <SOIL/SOIL.h>

#include "GameState.h"
#include "MenuState.h"

class Camera;

GameState state;
MenuState mainmenu;
int paused = 0;

class MouseState {
public:

	void SetCenter() {
		int width = GetSystemMetrics(SM_CXSCREEN);
		int height = GetSystemMetrics(SM_CYSCREEN);

		xMid = width / 2;
		yMid = height / 2;
		xPos = xMid;
		yPos = yMid;
		SetCursorPos(xMid, yMid);
	}

	void Reset() {
		if (xPos != xMid && yPos != yMid) {
			xPos = xMid;
			yPos = yMid;
			SetCursorPos(xMid, yMid);
		}
	}

	void Update(POINT & pt) {
		xPos = pt.x;
		yPos = pt.y;
	}

	int PosX() {
		return xPos;
	}

	int PosY() {
		return yPos;
	}

	int MotionX() {
		return xPos - xMid;
	}

	int MotionY() {
		return yPos - yMid;
	}

private:
	int left;
	int top;
	int xMid;
	int yMid;
	int xPrevPos;
	int yPrevPos;
	int xPos;
	int yPos;
};

MouseState mouse;

std::string rootDir = "C:\\Users\\kane\\source\\repos\\MereEngineDemo\\";
std::string firstLevel = "Data\\Test.level";
#define MAX_FRAME_RATE 200.0f

bool Init(HWND);
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK MenuProc(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK GameProc(HWND, UINT, WPARAM, LPARAM);

int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
	PSTR pCmdLine, int nCmdShow)
{
	WNDCLASS wc = {};
	wc.style = CS_OWNDC | CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc = WndProc;
	wc.hInstance = hInstance;
	wc.lpszClassName = L"GameWindow";

	RegisterClass(&wc);

	PIXELFORMATDESCRIPTOR pfd = {
		sizeof(PIXELFORMATDESCRIPTOR),
		1,
		PFD_DRAW_TO_WINDOW |
		PFD_SUPPORT_OPENGL |
		PFD_DOUBLEBUFFER |
		PFD_TYPE_RGBA,
		24,
		0, 0, 0, 0, 0, 0,
		0,
		0,
		0,
		0, 0, 0, 0,
		32,
		0,
		0,
		PFD_MAIN_PLANE,
		0,
		0, 0, 0
	};

	HWND hWnd = CreateWindow(L"GameWindow", L"MereEngine",
		WS_OVERLAPPEDWINDOW | WS_VISIBLE,
		CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
		NULL, NULL, hInstance, NULL);

	HDC hdc = GetDC(hWnd);

	int pixel_format = ChoosePixelFormat(hdc, &pfd);

	SetPixelFormat(hdc, pixel_format, &pfd);

	HGLRC hglrc = wglCreateContext(hdc);

	BOOL is_current = wglMakeCurrent(hdc, hglrc);

	GLenum err = glewInit();
	if (err != GLEW_OK) {
		return 1;
	}

	wglUseFontBitmaps(hdc, 0, 256, 1000);
	glListBase(1000);

	Init(hWnd);

	MSG msg;
	int running = 1;

	ShowCursor(FALSE);
	int cursorVisible = 0;

	LARGE_INTEGER counterFreq;
	QueryPerformanceFrequency(&counterFreq);
	LARGE_INTEGER lastCounter, counter, elapsed;

	QueryPerformanceCounter(&lastCounter);
	double deltatime = 0.0;

	while (running) {

		while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
			if (msg.message == WM_QUIT) {
				running = 0;
			}
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		if (paused) {
			if (!cursorVisible) {
				ShowCursor(TRUE);
				cursorVisible = 1;
			}

			/*if (mainmenu.currentAction == MENU_ACTION_NEWGAME)
			{
				// create a new game from beginning
				if (state.loaded) state.clean();

				try {
					state.loadNew(rootDir + firstLevel);
				}
				catch (std::exception& e)
				{
					std::cerr << e.what() << "\n";
					return false;
				}
			}
			if (mainmenu.currentAction == MENU_ACTION_NULL)
			{
				break;
			}
			mainmenu.renderMenu();*/
		}
		else {
			if (cursorVisible) {
				ShowCursor(FALSE);
				cursorVisible = 0;
			}

			state.move(deltatime);
			state.render();
		}

		SwapBuffers(hdc);

		// Manual swap control
		// TODO: utilize vertical sync
		Sleep(7);

		QueryPerformanceCounter(&counter);
		elapsed.QuadPart = counter.QuadPart - lastCounter.QuadPart;
		elapsed.QuadPart *= 1000000;
		elapsed.QuadPart /= counterFreq.QuadPart;
		deltatime = ((double)elapsed.QuadPart) / 1000000.0;
		lastCounter = counter;
	}

	glDeleteLists(1000, 256);

	wglMakeCurrent(NULL, NULL);

	ReleaseDC(hWnd, hdc);

	wglDeleteContext(hglrc);

	DestroyWindow(hWnd);

	return 0;
}

bool Init(HWND hWnd)
{
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f); // Clear color to black, no alpha
	glEnable(GL_DEPTH_TEST);              // Enable the depth buffer
	glClearDepth(1.0f);                   // Clear all of the depth buffer
	glDepthFunc(GL_LEQUAL);		          // Depth function; Overwrite if new value <= current value
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);

	RECT rt;
	GetClientRect(hWnd, &rt);
	int width = rt.right - rt.left;
	int height = rt.bottom - rt.top;
	mouse.SetCenter();
	/*
	try
	{
		mainmenu.init(rootDir, width, height);
	}
	catch (std::exception & e)
	{
		std::cerr << e.what() << "\n";
		return false;
	}*/

	try
	{
		state.init(rootDir, width, height);
	}
	catch (std::exception & e)
	{
		std::cerr << e.what() << "\n";
		return false;
	}

	state.loadNew(rootDir + firstLevel);

	return true;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message) {
		case WM_CREATE:
			return 0;
		case WM_CLOSE:
			return 0;
		case WM_KEYDOWN:
		case WM_KEYUP:
		case WM_LBUTTONDOWN:
		case WM_MOUSEMOVE:
			if (paused) {
				return MenuProc(hWnd, message, wParam, lParam);
			}
			else {
				return GameProc(hWnd, message, wParam, lParam);
			}
			break;
		default:
			break;
	}
	return DefWindowProc(hWnd, message, wParam, lParam);
}

LRESULT CALLBACK MenuProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message) {
		case WM_KEYDOWN:
			switch (wParam) {
				case VK_ESCAPE:
					//mainmenu.currentAction = MENU_ACTION_NULL;
					paused = 0;
					break;
			}
			break;
		case WM_LBUTTONDOWN:
			mainmenu.handleButtonDown();
			break;
		case WM_MOUSEMOVE:
			//mouse.Update(lParam);
			//mainmenu.handleMouseMove(mouse.MotionX(), mouse.MotionY());
			break;
	}
	return DefWindowProc(hWnd, message, wParam, lParam);
}

LRESULT CALLBACK GameProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message) {
		case WM_KEYDOWN:
			switch (wParam) {
				case VK_ESCAPE:
					paused = 1;
					break;
				case 0x57: // W
					state.movingForward(true);
					break;
				case 0x53: // S
					state.movingBackward(true);
					break;
				case 0x41: // A
					state.movingLeft(true);
					break;
				case 0x44: // D
					state.movingRight(true);
					break;
				case VK_SPACE:
					state.handleSpacebar();
					break;
				case VK_SNAPSHOT:
					//SOIL_save_screenshot("screenshot.bmp", SOIL_SAVE_TYPE_BMP, 0, 0,
					//	width, height);
					break;
			}
			break;
		case WM_KEYUP:
			switch (wParam) {
				case 0x57: // W
					state.movingForward(false);
					break;
				case 0x53: // S
					state.movingBackward(false);
					break;
				case 0x41: // A
					state.movingLeft(false);
					break;
				case 0x44: // D
					state.movingRight(false);
					break;
			}
			break;
		case WM_LBUTTONDOWN:
			/*
			 * This case handles the player's use
			 * of a weapon
			 */
			//if (!player->playerIsFiring) {

				//GameObject* newobject = new Bullet(player->getHead()+0.1f*glm::normalize(player->getOrigin()-player->getHead()),player->getHead()+100.0f*glm::normalize(player->getOrigin()-player->getHead()),player);

				/*char meshFile[50];
				char boundsFile[50];
				sprintf(meshFile,"Data/bullet.3ds");
				sprintf(boundsFile,"Data/bullet.dat");
				GameObject* newobject = new ProjectileObject(meshFile,boundsFile,player->getHead()+2.0f*glm::normalize(player->getOrigin()-player->getHead()),5.0f*glm::normalize(player->getOrigin()-player->getHead()),glm::normalize(player->getOrigin()-player->getHead()),0.0f,this);
				*/

				//levelObjects.push_back(newobject);
				//opaqueObjects.push_back(newobject);

				//player->fire();

			//}
			break;
		case WM_MOUSEMOVE:
			POINT pt;
			pt.x = LOWORD(lParam);
			pt.y = HIWORD(lParam);
			ClientToScreen(hWnd, &pt);
			mouse.Update(pt);
			state.handleMouseMove(mouse.MotionX(), mouse.MotionY());
			mouse.Reset();
			break;
	}
	return 0;
}