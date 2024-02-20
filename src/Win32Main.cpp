#include "Platform.h"

#include "GameState.h"

typedef BOOL(WINAPI * PFNWGLSWAPINTERVALEXTPROC) (int interval);
typedef int (WINAPI * PFNWGLGETSWAPINTERVALEXTPROC) (void);

HDC hdc = NULL;
HGLRC hglrc = NULL;
BOOL contextIsCurrent = FALSE;

GameState state;

class Timer {
public:
	Timer() {
		QueryPerformanceFrequency(&counterFreq);
		QueryPerformanceCounter(&lastCounter);
		counter = lastCounter;
	}

	double reset() {
		LARGE_INTEGER elapsed;

		QueryPerformanceCounter(&counter);
		elapsed.QuadPart = counter.QuadPart - lastCounter.QuadPart;
		elapsed.QuadPart *= 1000000;
		elapsed.QuadPart /= counterFreq.QuadPart;
		lastCounter = counter;
		return (static_cast<double>(elapsed.QuadPart)) / 1000000.0;
	}

protected:
	LARGE_INTEGER counterFreq;
	LARGE_INTEGER lastCounter;
	LARGE_INTEGER counter;
};

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

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK GameProc(HWND, UINT, WPARAM, LPARAM);
int MainLoop(cmd_line_args&);
void EnableVSync();

int APIENTRY WinMain(__in HINSTANCE hInstance,
                     __in_opt HINSTANCE hPrevInstance,
                     __in PSTR pCmdLine,
                     __in int nCmdShow)
{
	cmd_line_args args;
	if (ParseCmdLine(args, pCmdLine)) {
		return 1;
	}

	WNDCLASS wc = {};
	wc.style = CS_OWNDC | CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc = WndProc;
	wc.hInstance = hInstance;
	wc.lpszClassName = L"GameWindow";

	if (!RegisterClass(&wc)) {
		return 1;
	}

	int width = GetSystemMetrics(SM_CXSCREEN);
	int height = GetSystemMetrics(SM_CYSCREEN);
	state.setWindowSize(width, height);

	HWND hWnd = CreateWindow(L"GameWindow", L"MereEngine",
		WS_POPUP | WS_VISIBLE, 0, 0, width, height,
		NULL, NULL, hInstance, NULL);

	if (!hWnd) {
		return 1;
	}

	RAWINPUTDEVICE rid;
	rid.usUsagePage = 0x01;
	rid.usUsage = 0x02;
	rid.dwFlags = 0;
	rid.hwndTarget = hWnd;

	if (!RegisterRawInputDevices(&rid, 1, sizeof(rid))) {
		DestroyWindow(hWnd);
		return 1;
	}

	int exitCode = MainLoop(args);

	DestroyWindow(hWnd);

	return exitCode;
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

				EnableVSync();
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
		case WM_INPUT:
			return GameProc(hWnd, message, wParam, lParam);
		default:
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
					state.handleEscape();
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
			}
			break;
		case WM_LBUTTONDOWN:
			state.handleLeftButtonDown();
			break;
		case WM_MOUSEMOVE:
			state.handleMouseMove(LOWORD(lParam), HIWORD(lParam));
			break;
		case WM_INPUT:
			{
				UINT dwSize = sizeof(RAWINPUT);
				static BYTE lpb[sizeof(RAWINPUT)];

				GetRawInputData((HRAWINPUT)lParam, RID_INPUT, lpb, &dwSize, sizeof(RAWINPUTHEADER));

				RAWINPUT* raw = (RAWINPUT*)lpb;

				if (raw->header.dwType == RIM_TYPEMOUSE) {
					if (raw->data.mouse.usFlags == MOUSE_MOVE_RELATIVE) {
						int relativeX = raw->data.mouse.lLastX;
						int relativeY = raw->data.mouse.lLastY;
						state.handleRelativeMouseMove(relativeX, relativeY);
					}
				}
			}
			break;
	}
	return 0;
}

int MainLoop(cmd_line_args& args)
{
	wglUseFontBitmaps(hdc, 0, 256, 1000);
	glListBase(1000);

	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glEnable(GL_DEPTH_TEST);
	glClearDepth(1.0f);
	glDepthFunc(GL_LEQUAL);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);

	std::string rootDir = args.root;
	std::string firstLevel = args.level;
	state.init(rootDir);
	state.loadNew(rootDir + firstLevel);

	MSG msg;

	ShowCursor(FALSE);

	Timer timer;
	double timeElapsed = timer.reset();

	for (;;) {

		while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
			if (msg.message == WM_QUIT) {
				goto exit;
			}
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		if (state.run(timeElapsed)) {
			goto exit;
		}

		SwapBuffers(hdc);
		timeElapsed = timer.reset();
	}

exit:

	glDeleteLists(1000, 256);

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