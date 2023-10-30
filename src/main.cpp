#include "Platform.h"

#include <SOIL/SOIL.h>

#include "GameState.h"
#include "Menu.h"

class Camera;

GameState state;
Menu menu;
int paused = 0;
RECT rt;
int width, height;

class MouseState {
public:

	void SetCenter() {
		xMid = GetSystemMetrics(SM_CXSCREEN) / 2;
		yMid = GetSystemMetrics(SM_CYSCREEN) / 2;
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
	int xMid;
	int yMid;
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

	GetClientRect(hWnd, &rt);
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

			switch (menu.getSelection()) {
			case MENU_ACTION_NEWGAME:
				if (state.loaded) state.clean();
				state.loadNew(rootDir + firstLevel);
				paused = 0;
				break;
			case MENU_ACTION_EXIT:
				running = 0;
				break;
			default:
				break;
			}
			menu.render(width, height);
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

	width = rt.right - rt.left;
	height = rt.bottom - rt.top;
	mouse.SetCenter();

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
					paused = 0;
					break;
			}
			break;
		case WM_LBUTTONDOWN:
			menu.handleButtonDown();
			break;
		case WM_MOUSEMOVE:
			menu.handleMouseMove(LOWORD(lParam), HIWORD(lParam));
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
				case VK_SNAPSHOT:
					SOIL_save_screenshot("screenshot.bmp", SOIL_SAVE_TYPE_BMP,
						0, 0, width, height);
					break;
			}
			break;
		case WM_LBUTTONDOWN:
			state.FirePrimaryWeapon();
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