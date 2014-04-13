#include "stdafx.h"
#include "Hotkey.h"

CHotkey *CHotkey::m_pThis = NULL;

CHotkey::CHotkey()
{
	InstallHook();
	LoadKeyMap();
}

CHotkey::~CHotkey()
{
	RemoveHook();
}

bool CHotkey::LoadKeyMap()
{
	key_map.erase(key_map.begin(), key_map.end());

	RegKey(IDM_TERMINATE_ANEMONE, VK_F12, true, true, true);
	return true;
}

inline void CHotkey::RegKey(int func, int code, bool bCtrl, bool bAlt, bool bShift)
{
	struct _key_map kmap;
	kmap.Alt = bAlt;
	kmap.Shift = bShift;
	kmap.Ctrl = bCtrl;
	kmap.Code = code;
	kmap.func = func;
	key_map.push_back(kmap);
}

bool CHotkey::SaveKeyMap()
{
	//key_map.erase(key_map.begin(), key_map.end());
	return true;
}

bool CHotkey::InstallHook()
{
	m_pThis = this;
	m_hHook = ::SetWindowsHookEx(WH_KEYBOARD_LL, _KeyboardProc, hInst, NULL);
	return true;
}

bool CHotkey::RemoveHook()
{
	if (m_hHook != NULL)
	{
		UnhookWindowsHookEx(m_hHook);
		m_hHook = NULL;
	}
	return true;
}

LRESULT CHotkey::KeyboardProc(int nCode, WPARAM wParam, LPARAM lParam)
{
	if (nCode >= 0)
	{
		PKBDLLHOOKSTRUCT pHookKey = (PKBDLLHOOKSTRUCT)lParam;


		switch(wParam)
		{
		case 256:	// WM_KEYDOWN
		case 260:	// WM_SYSKEYDOWN
			for (DWORD i = 0; i < key_map.size(); i++)
			{
				if (key_map[i].Code == pHookKey->vkCode)
				{
					bool bCtrl = ((GetAsyncKeyState(VK_CONTROL) & 0x8000) ? false : true);
					bool bShift = ((GetAsyncKeyState(VK_SHIFT) & 0x8000) ? false : true);
					bool bAlt = ((GetAsyncKeyState(VK_MENU) & 0x8000) ? false : true);
					
					if (key_map[i].Alt == true && bAlt) continue;
					else
					if (key_map[i].Shift == true && bShift) continue;
					else
					if (key_map[i].Ctrl == true && bCtrl) continue;
					
					SendMessage(hWnds.Main, WM_COMMAND, key_map[i].func, 0);
					return -1;
				}
			}

			break;
		}
		/*
		switch (wParam)
		{
			// pHookKey.flags = 0x20 ALT
			// pHookKey.flags = 0x00 CTRL
			// pHookKey.flags = 0x01 NONE


		case 256: // WM_KEYDOWN
			//case 257: // WM_KEYUP
		case 260: // WM_SYSKEYDOWN
			//case 261: // WM_SYSKEYUP
			if (g_UseHotKey == true)
			{
				if ((IsWindowVisible(hMainWnd) || !IsWindowVisible(hMainWnd) && f_hidewin_nohotkey == false))
				{
					if (pHookKey->vkCode == VK_F6)
					{
						PostMessage(hMainWnd, WM_HOTKEY, IDH_F6_WNDTRANSPARENT, 0);
						return 1;
					}
					if (pHookKey->vkCode == VK_F7)
					{
						PostMessage(hMainWnd, WM_HOTKEY, IDH_F7_BACKGROUNDTP, 0);
						return 1;
					}
					if (pHookKey->vkCode == VK_F8)
					{
						PostMessage(hMainWnd, WM_HOTKEY, IDH_F8_TOGGLEJPNSTR, pHookKey->flags);
						return 1;
					}
					if (pHookKey->vkCode == VK_F9)
					{
						PostMessage(hMainWnd, WM_HOTKEY, IDH_F9_LOGWND, pHookKey->flags);
						return 1;
					}
					if (pHookKey->vkCode == VK_F10)
					{
						PostMessage(hMainWnd, WM_HOTKEY, IDH_F10_SETTINGWND, pHookKey->flags);
						return 1;
					}

					if (pHookKey->vkCode == VK_NUMPAD9)
					{
						PostMessage(hMainWnd, WM_HOTKEY, IDH_N9_REVIEWPREV, 0);
						return 1;
					}

					if (pHookKey->vkCode == VK_NUMPAD3)
					{
						PostMessage(hMainWnd, WM_HOTKEY, IDH_N3_REVIEWNEXT, 0);
						return 1;
					}

					if (pHookKey->vkCode == VK_OEM_3 && !(GetKeyState(VK_SHIFT) & 0x8000))
					{
						PostMessage(hMainWnd, WM_HOTKEY, IDH_MAGNETICMODE, 0);
						return 1;
					}

					if (pHookKey->vkCode == VK_SCROLL)
					{
						PostMessage(hMainWnd, WM_HOTKEY, IDH_CLIPBOARDWATCH, 0);
						return 1;
					}

					if (pHookKey->vkCode == VK_ADD)
					{
						PostMessage(hMainWnd, WM_HOTKEY, IDH_TEXTSIZEUP, 0);
						return 1;
					}

					if (pHookKey->vkCode == VK_SUBTRACT)
					{
						PostMessage(hMainWnd, WM_HOTKEY, IDH_TEXTSIZEDOWN, 0);
						return 1;
					}

					if (pHookKey->vkCode == VK_DIVIDE)
					{
						PostMessage(hMainWnd, WM_HOTKEY, IDH_REFRESHWND, 0);
						return 1;
					}

					// 방향키 창이동 / ALT PRESS, 방향키 창크기 조절
					if (pHookKey->vkCode == VK_NUMPAD4)
					{
						PostMessage(hMainWnd, WM_HOTKEY, IDH_N4_LEFT, pHookKey->flags & 0x20);
						return 1;
					}

					if (pHookKey->vkCode == VK_NUMPAD6)
					{
						PostMessage(hMainWnd, WM_HOTKEY, IDH_N6_RIGHT, pHookKey->flags & 0x20);
						return 1;
					}

					if (pHookKey->vkCode == VK_NUMPAD8)
					{
						PostMessage(hMainWnd, WM_HOTKEY, IDH_N8_TOP, pHookKey->flags & 0x20);
						return 1;
					}

					if (pHookKey->vkCode == VK_NUMPAD2)
					{
						PostMessage(hMainWnd, WM_HOTKEY, IDH_N2_BOTTOM, pHookKey->flags & 0x20);
						return 1;
					}
				}

				// 창 숨김시 단축키 일시 해제가 작동중일때 NUM * 키는 예외적으로 허용
				if (pHookKey->vkCode == VK_MULTIPLY)
				{
					PostMessage(hMainWnd, WM_HOTKEY, IDH_WNDSHOWTOGGLE, 0);
					return 1;
				}

			}

			if (pHookKey->vkCode == VK_F10 && (GetKeyState(VK_SHIFT) & 0x8000) && ((pHookKey->flags & 0x20) == 0x20))
			{
				PostMessage(hMainWnd, WM_HOTKEY, IDH_USE_HOTKEY, 0);
				return 1;
			}

			break;
			
		}*/
	}
	return CallNextHookEx(m_hHook, nCode, wParam, lParam);


}