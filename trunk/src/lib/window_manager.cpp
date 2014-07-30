#include "lib/universal_include.h"
#include "lib/window_manager.h"

#include <windows.h>

#include "lib/input.h"
#include "lib/debug_utils.h"
#include "lib/window_manager_win32.h"


static HINSTANCE g_hInstance;


WindowManager *g_windowManager = NULL;


LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	if (!g_inputManager ||
		g_inputManager->EventHandler(message, wParam, lParam) == -1)
	{
		return DefWindowProc(hWnd, message, wParam, lParam);
	}

	return 0;
}


WindowManager::WindowManager()
{
	DebugAssert(g_windowManager == NULL);
	m_win32Specific = new WindowManagerWin32;
}


WindowManager::~WindowManager()
{
	DestroyWin();
	UnclipCursor();
}


void WindowManager::EnableOpenGL(int colourDepth, int zDepth)
{
	PIXELFORMATDESCRIPTOR pfd;
	int format;
	
	// Get the device context (DC)
	m_win32Specific->m_hDC = GetDC(m_win32Specific->m_hWnd);
	
	// Set the pixel format for the DC
	ZeroMemory(&pfd, sizeof(pfd));
	pfd.nSize = sizeof(pfd);
	pfd.nVersion = 1;
	pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
	pfd.iPixelType = PFD_TYPE_RGBA;
	pfd.cColorBits = colourDepth;
	pfd.cDepthBits = zDepth;
	pfd.iLayerType = PFD_MAIN_PLANE;
	format = ChoosePixelFormat(m_win32Specific->m_hDC, &pfd);
	SetPixelFormat(m_win32Specific->m_hDC, format, &pfd);
	
	// Create and enable the render context (RC)
	m_win32Specific->m_hRC = wglCreateContext(m_win32Specific->m_hDC);
	wglMakeCurrent(m_win32Specific->m_hDC, m_win32Specific->m_hRC);

	glClear(GL_COLOR_BUFFER_BIT);
}


void WindowManager::DisableOpenGL()
{
	wglMakeCurrent(NULL, NULL);
	wglDeleteContext(m_win32Specific->m_hRC);
	ReleaseDC(m_win32Specific->m_hWnd, m_win32Specific->m_hDC);
}


void WindowManager::ClipCursor()
{
	RECT r;
	GetWindowRect(m_win32Specific->m_hWnd, &r);
	r.left += 50;
	r.right -= 50;
	r.top += 50;
	r.bottom -= 50;
	::ClipCursor(&r);
}


void WindowManager::UnclipCursor()
{
	::ClipCursor(NULL);
}


bool WindowManager::CreateWin(int _width, int _height, bool _windowed, char const *_appName)
{
	m_screenW = _width;
	m_screenH = _height;
    m_windowed = _windowed;

	// Register window class
	WNDCLASS wc;
	wc.style = CS_OWNDC;
	wc.lpfnWndProc = WndProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = g_hInstance;
	wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
	wc.lpszMenuName = NULL;
	wc.lpszClassName = _appName;
	RegisterClass(&wc);

	const int colourDepth = 32;
	const int zDepth = 24;

	int posX, posY;
	unsigned int windowStyle = WS_POPUPWINDOW | WS_VISIBLE;

	if (_windowed)
	{
        windowStyle |= WS_CAPTION | WS_MINIMIZEBOX | WS_MAXIMIZEBOX;
		RECT windowRect = { 0, 0, _width, _height };
		AdjustWindowRect(&windowRect, windowStyle, false);
		int borderWidth = ((windowRect.right - windowRect.left) - _width) / 2;
		int titleHeight = ((windowRect.bottom - windowRect.top) - _height) - borderWidth * 2;
		_width += borderWidth * 2;
		_height += borderWidth * 2 + titleHeight;

		HWND desktopWindow = GetDesktopWindow();
		RECT desktopRect;
		GetWindowRect(desktopWindow, &desktopRect);
		int desktopWidth = desktopRect.right - desktopRect.left;
		int desktopHeight = desktopRect.bottom - desktopRect.top;

		if (_width > desktopWidth || _height > desktopHeight)
        {
            return false;
        }

		posX = (desktopRect.right - _width) / 2;
		posY = (desktopRect.bottom - _height) / 2;
	}
	else
	{
		DEVMODE devmode;
		devmode.dmSize = sizeof(DEVMODE);
		devmode.dmBitsPerPel = colourDepth;
		devmode.dmPelsWidth = _width;
		devmode.dmPelsHeight = _height;
		devmode.dmDisplayFrequency = 60;
		devmode.dmFields = DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT | DM_DISPLAYFREQUENCY;
		long result = ChangeDisplaySettings(&devmode, CDS_FULLSCREEN);
        if (result != DISP_CHANGE_SUCCESSFUL) return false;
        
        posX = 0;
		posY = 0;
	}

	// Create main window
	m_win32Specific->m_hWnd = CreateWindow(
		wc.lpszClassName, wc.lpszClassName, 
		windowStyle,
		posX, posY, _width, _height,
		NULL, NULL, g_hInstance, NULL);

    if (!m_win32Specific->m_hWnd)
		return false;

	ClipCursor();
	HideMousePointer();
	EnableOpenGL(colourDepth, zDepth);

    return true;
}


void WindowManager::DestroyWin()
{
	DisableOpenGL();
	DestroyWindow(m_win32Specific->m_hWnd);
}


void WindowManager::Flip()
{
	SwapBuffers(m_win32Specific->m_hDC);
}


void WindowManager::NastyPollForMessages()
{
	MSG msg;
	while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) 
	{
		// handle or dispatch messages
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	} 
}


void WindowManager::HideMousePointer()
{
	ShowCursor(false);
}


void WindowManager::UnhideMousePointer()
{
	ShowCursor(true);
}


int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, 
				   LPSTR cmdLine, int iCmdShow)
{
	g_hInstance = hInstance;
	g_windowManager	= new WindowManager();
	AppMain();
	return WM_QUIT;
}
