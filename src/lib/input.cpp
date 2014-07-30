#include "lib/universal_include.h"
#include "lib/input.h"

#include <stdlib.h>
#include "lib/debug_utils.h"
#include "lib/window_manager.h"
#include "lib/window_manager_win32.h"


InputManager *g_inputManager = NULL;
signed char g_keyDeltas[KEY_MAX];
signed char g_keys[KEY_MAX];
  

InputManager::InputManager()
:	m_windowHasFocus(true),
    m_lmb(false),
    m_mmb(false),
    m_rmb(false),
    m_lmbOld(false),
    m_mmbOld(false),
    m_rmbOld(false),
	m_rawLmbClicked(false),
    m_lmbClicked(false),
    m_mmbClicked(false),
    m_rmbClicked(false),
    m_lmbUnClicked(false),
    m_mmbUnClicked(false),
    m_rmbUnClicked(false),
    m_mouseX(0),
    m_mouseY(0),
    m_mouseZ(0),
	m_accumulatedMouseVelX(0),
	m_accumulatedMouseVelY(0),
    m_mouseOldZ(0),
    m_mouseVelX(0),
    m_mouseVelY(0),
    m_mouseVelZ(0)
{
	memset(g_keys, 0, KEY_MAX);
	memset(g_keyDeltas, 0, KEY_MAX);
	memset(m_keyNewDeltas, 0, KEY_MAX);

	DebugAssert(g_windowManager->m_win32Specific->m_hWnd);

#ifndef HID_USAGE_PAGE_GENERIC
#define HID_USAGE_PAGE_GENERIC         ((USHORT) 0x01)
#endif
#ifndef HID_USAGE_GENERIC_MOUSE
#define HID_USAGE_GENERIC_MOUSE        ((USHORT) 0x02)
#endif

	RAWINPUTDEVICE Rid[1];
	Rid[0].usUsagePage = HID_USAGE_PAGE_GENERIC; 
	Rid[0].usUsage = HID_USAGE_GENERIC_MOUSE; 
	Rid[0].dwFlags = RIDEV_INPUTSINK;   
	Rid[0].hwndTarget = g_windowManager->m_win32Specific->m_hWnd;
	RegisterRawInputDevices(Rid, 1, sizeof(Rid[0]));
}


// Returns 0 if the event is handled here, -1 otherwise
int InputManager::EventHandler(unsigned int message, unsigned int wParam, int lParam)
{
	switch (message) 
	{
		case WM_SIZING:
		case WM_WINDOWPOSCHANGING:
		case WM_WINDOWPOSCHANGED:
		case WM_DESTROY:
		case WM_CREATE:
		case WM_COMMAND:
		case WM_SETCURSOR:		// Cursor move
		case WM_NCHITTEST:		// Mouse move or button
			return -1;
	
		case WM_CANCELMODE:
			return 0;

		case WM_CLOSE:
			PostQuitMessage( 0 );
			return 0;

		case WM_SETFOCUS:
			g_windowManager->ClipCursor();
			g_windowManager->HideMousePointer();
			m_windowHasFocus = true;
			return 0;

		case WM_KILLFOCUS:
			m_windowHasFocus = false;
			g_windowManager->UnclipCursor();
			g_windowManager->UnhideMousePointer();
			UnregisterHotKey(g_windowManager->m_win32Specific->m_hWnd, 0);
			return 0;

		case WM_LBUTTONDOWN:
			m_lmb = true;
			break;
		case WM_LBUTTONUP:
			m_lmb = false;
			break;
		case WM_MBUTTONDOWN:
			m_mmb = true;
			break;
		case WM_MBUTTONUP:
			m_mmb = false;
			break;
		case WM_RBUTTONDOWN:
			m_rmb = true;
			break;
		case WM_RBUTTONUP:
			m_rmb = false;
			break;

		case WM_MOUSEWHEEL:
		{
			int move = (short)HIWORD(wParam) / 120;
			m_mouseZ += move;
			break;
		}

		case WM_MOUSEMOVE:
		{
			short newPosX = lParam & 0xFFFF;
			short newPosY = short(lParam >> 16);
			m_mouseX = newPosX;
			m_mouseY = newPosY;
			break;
		}

		case WM_INPUT: 
		{
			UINT dwSize = 40;
			static BYTE lpb[40];

			GetRawInputData((HRAWINPUT)lParam, RID_INPUT, 
				lpb, &dwSize, sizeof(RAWINPUTHEADER));

			RAWINPUT* raw = (RAWINPUT*)lpb;

			if (raw->header.dwType == RIM_TYPEMOUSE) 
			{
				int xPosRelative = raw->data.mouse.lLastX;
				int yPosRelative = raw->data.mouse.lLastY;
				m_accumulatedMouseVelX += xPosRelative;
				m_accumulatedMouseVelY += yPosRelative;
			} 
			break;
		}

		case WM_HOTKEY:
			if (wParam == 0)
			{
				g_keys[KEY_TAB] = 1;
				m_keyNewDeltas[KEY_TAB] = 1;
			}
			break;
			
		case WM_SYSKEYUP:
			if (wParam == KEY_ALT)
				g_keys[wParam] = 0;
			else
				g_keys[wParam] = 0;
			break;
			
		case WM_SYSKEYDOWN:
		{
			int flags = (short)HIWORD(lParam);
			if (wParam == KEY_ALT)
			{
				g_keys[wParam] = 1;
			}
			else if (flags & KF_ALTDOWN)
			{
				m_keyNewDeltas[wParam] = 1;
				g_keys[wParam] = 1;
				if (wParam == KEY_F4)
					exit(0);
			}
			break;
		}

		case WM_KEYUP:
		{
			DebugAssert(wParam >= 0 && wParam < KEY_MAX);
			if (g_keys[wParam] != 0)
			{
//				m_keyNewDeltas[wParam] = -1;
				g_keys[wParam] = 0;
			}
			break;
		}

		case WM_KEYDOWN:
		{
			DebugAssert(wParam >= 0 && wParam < KEY_MAX);

			if (g_keys[wParam] != 1)
			{
				m_keyNewDeltas[wParam] = 1;
				g_keys[wParam] = 1;
			}                
			break;
		}

		default:
			return -1;
	}

	return 0;
}


void InputManager::Advance()
{
	memcpy(g_keyDeltas, m_keyNewDeltas, KEY_MAX);
	memset(m_keyNewDeltas, 0, KEY_MAX);

	m_lmbClicked = m_lmb == true && m_lmbOld == false;
	m_mmbClicked = m_mmb == true && m_mmbOld == false;
	m_rmbClicked = m_rmb == true && m_rmbOld == false;
	m_rawLmbClicked = m_lmbClicked;
	m_lmbUnClicked = m_lmb == false && m_lmbOld == true;
	m_mmbUnClicked = m_mmb == false && m_mmbOld == true;
	m_rmbUnClicked = m_rmb == false && m_rmbOld == true;
	m_lmbOld = m_lmb;
	m_mmbOld = m_mmb;
	m_rmbOld = m_rmb;

	m_mouseVelX = m_accumulatedMouseVelX;
	m_mouseVelY = m_accumulatedMouseVelY;
	m_mouseVelZ = m_mouseZ - m_mouseOldZ;
	m_accumulatedMouseVelX = 0;
	m_accumulatedMouseVelY = 0;
	m_mouseOldZ = m_mouseZ;
}


void InputManager::PollForMessages()
{
	g_windowManager->NastyPollForMessages();
}
