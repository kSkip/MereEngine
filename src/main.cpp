#include "Platform.h"

#include <SOIL/SOIL.h>

#include "GameState.h"
#include "Menu.h"

typedef BOOL(WINAPI * PFNWGLSWAPINTERVALEXTPROC) (int interval);
typedef int (WINAPI * PFNWGLGETSWAPINTERVALEXTPROC) (void);

HDC hdc = NULL;
HGLRC hglrc = NULL;
BOOL contextIsCurrent = FALSE;

GameState state;
Menu menu;
int paused = 0;
int width = 0;
int height = 0;

struct mouse {
	int xMid;
	int yMid;
	int xPos;
	int yPos;

	void setCenter() {
		xMid = GetSystemMetrics(SM_CXSCREEN) / 2;
		yMid = GetSystemMetrics(SM_CYSCREEN) / 2;
		xPos = xMid;
		yPos = yMid;
		SetCursorPos(xMid, yMid);
	}

	void reset() {
		if (xPos != xMid && yPos != yMid) {
			xPos = xMid;
			yPos = yMid;
			SetCursorPos(xMid, yMid);
		}
	}

	int motionX() {
		return xPos - xMid;
	}

	int motionY() {
		return yPos - yMid;
	}

} Mouse;

#define NUM_ARGS 2

union cmd_line_args {
	struct {
		char *root;
		char *level;
	};
	char *argv[NUM_ARGS];
};

// The windows API does not provide a CommandLineToArgvA.
// This parser is used instead of adding WideCharToMultiByte.
// It expects to find exactly NUM_ARGS number of arguments.
int ParseCmdLine(cmd_line_args & args, char *cmdLine)
{
	char *str = cmdLine;
	int i = 0, j = 0;
	while (cmdLine[i] && j < NUM_ARGS) {
		if (cmdLine[i] == ' ') {
			cmdLine[i++] = 0;
			args.argv[j++] = str;
			str += i;
		}
		i++;
	}
	if (j == NUM_ARGS - 1) {
		args.argv[j] = str;
		return 0;
	}
	else {
		return 1;
	}
}

void EnableVSync();
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK MenuProc(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK GameProc(HWND, UINT, WPARAM, LPARAM);

int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
	PSTR pCmdLine, int nCmdShow)
{
	cmd_line_args args;
	if (ParseCmdLine(args, pCmdLine)) {
		return 1;
	}
	std::string rootDir = args.root;
	std::string firstLevel = args.level;

	WNDCLASS wc = {};
	wc.style = CS_OWNDC | CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc = WndProc;
	wc.hInstance = hInstance;
	wc.lpszClassName = L"GameWindow";

	if (!RegisterClass(&wc)) {
		return 1;
	}

	HWND hWnd = CreateWindow(L"GameWindow", L"MereEngine",
		WS_OVERLAPPEDWINDOW | WS_VISIBLE,
		CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
		NULL, NULL, hInstance, NULL);

	if (!hWnd) {
		return 1;
	}

	wglUseFontBitmaps(hdc, 0, 256, 1000);
	glListBase(1000);

	state.init(rootDir, width, height);
	state.loadNew(rootDir + firstLevel);

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

		QueryPerformanceCounter(&counter);
		elapsed.QuadPart = counter.QuadPart - lastCounter.QuadPart;
		elapsed.QuadPart *= 1000000;
		elapsed.QuadPart /= counterFreq.QuadPart;
		deltatime = ((double)elapsed.QuadPart) / 1000000.0;
		lastCounter = counter;
	}

	glDeleteLists(1000, 256);

	DestroyWindow(hWnd);

	return 0;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message) {
		case WM_CREATE:
			{
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

				if ((hdc = GetDC(hWnd)) == NULL) {
					return -1;
				}

				int pixelFormat = ChoosePixelFormat(hdc, &pfd);

				if (!SetPixelFormat(hdc, pixelFormat, &pfd)) {
					return -1;
				}

				if ((hglrc = wglCreateContext(hdc)) == NULL) {
					return -1;
				}

				contextIsCurrent = wglMakeCurrent(hdc, hglrc);
				if (!contextIsCurrent) {
					return -1;
				}

				if (glewInit() != GLEW_OK) {
					return -1;
				}

				EnableVSync(); // Sets the swap interval to 1 if not already set

				RECT rt;
				GetClientRect(hWnd, &rt);
				width = rt.right - rt.left;
				height = rt.bottom - rt.top;

				glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
				glEnable(GL_DEPTH_TEST);
				glClearDepth(1.0f);
				glDepthFunc(GL_LEQUAL);
				glEnable(GL_BLEND);
				glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
				glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);

				Mouse.setCenter();
			}
			return 0;
		case WM_DESTROY:
			if (contextIsCurrent) {
				wglMakeCurrent(NULL, NULL);
			}
			if (hdc) {
				ReleaseDC(hWnd, hdc);
			}
			if (hglrc) {
				wglDeleteContext(hglrc);
			}
			return 0;
		case WM_CLOSE:
			PostQuitMessage(0);
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
			Mouse.xPos = pt.x;
			Mouse.yPos = pt.y;
			state.handleMouseMove(Mouse.motionX(), Mouse.motionY());
			Mouse.reset();
			break;
	}
	return 0;
}

void EnableVSync()
{
	PFNWGLSWAPINTERVALEXTPROC wglSwapIntervalEXT = \
		(PFNWGLSWAPINTERVALEXTPROC)wglGetProcAddress("wglSwapIntervalEXT");
	PFNWGLGETSWAPINTERVALEXTPROC wglGetSwapIntervalEXT = \
		(PFNWGLGETSWAPINTERVALEXTPROC)wglGetProcAddress("wglGetSwapIntervalEXT");

	int swapInterval = wglGetSwapIntervalEXT();
	if (swapInterval == 0) {
		// Enable vertical sync
		wglSwapIntervalEXT(1);
	}
}