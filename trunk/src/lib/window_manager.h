#ifndef INCLUDED_WINDOW_MANAGER_H
#define INCLUDED_WINDOW_MANAGER_H


class WindowManagerWin32;


class WindowManager
{
public:
	WindowManagerWin32 *m_win32Specific;

protected:
	int			m_screenW;
	int			m_screenH;
    bool        m_windowed;

	void EnableOpenGL(int colourDepth, int zDepth);
	void DisableOpenGL();
	
public:
	WindowManager();
	~WindowManager();

	int ScreenW() { return m_screenW; }
	int ScreenH() { return m_screenH; }

	bool CreateWin(int width, int height, bool windowed, char const *appName);
	void DestroyWin();

    void Flip();
	void NastyPollForMessages();

	void HideMousePointer();
	void UnhideMousePointer();

	void ClipCursor();	// Limit cursor movement to window bounds
	void UnclipCursor();
};


void AppMain();

extern WindowManager *g_windowManager;


#endif
