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

	RegKey(VK_F6, false, false, false, IDM_TEMP_CLICK_THOUGH);
	RegKey(VK_F7, false, false, false, IDM_WND_BORDER_MODE);

	// 원문 문장/이름 출력
	RegKey(VK_F8, false, false, false, IDM_PRINT_ORGTEXT);
	RegKey(VK_F8, false, true, false, IDM_PRINT_ORGNAME);

	// 이름 분리 표시
	RegKey(VK_F9, false, false, false, IDM_SEPERATE_NAME);

	RegKey(VK_F10, false, false, false, IDM_WINDOW_SETTING, true);

	// 종료
	RegKey(VK_F12, false, false, false, IDM_TERMINATE_ANEMONE);

	// 이전글 / 최근글 보기
	RegKey(VK_NUMPAD9, false, false, false, IDM_TEXT_PREV);
	RegKey(VK_NUMPAD3, false, false, false, IDM_TEXT_NEXT);

	// 글자 크기 UP/DOWN
	RegKey(VK_SUBTRACT, false, false, false, IDM_TEXTSIZE_MINUS);
	RegKey(VK_ADD, false, false, false, IDM_TEXTSIZE_PLUS);

	// 창 잠깐 숨기기 / 창 완전 숨기기
	RegKey(VK_NUMPAD5, false, false, false, IDM_TEMP_WINDOW_HIDE, true);
	RegKey(VK_MULTIPLY, false, false, false, IDM_WINDOW_VISIBLE, true);

	// 창 이동
	RegKey(VK_NUMPAD8, false, false, false, IDM_WNDMOVE_TOP);
	RegKey(VK_NUMPAD2, false, false, false, IDM_WNDMOVE_BOTTOM);
	RegKey(VK_NUMPAD4, false, false, false, IDM_WNDMOVE_LEFT);
	RegKey(VK_NUMPAD6, false, false, false, IDM_WNDMOVE_RIGHT);

	// 창 크기 조절
	RegKey(VK_NUMPAD8, false, true, false, IDM_WNDSIZE_TOP);
	RegKey(VK_NUMPAD2, false, true, false, IDM_WNDSIZE_BOTTOM);
	RegKey(VK_NUMPAD4, false, true, false, IDM_WNDSIZE_LEFT);
	RegKey(VK_NUMPAD6, false, true, false, IDM_WNDSIZE_RIGHT);

	// 단문 번역/파일 번역창
	RegKey(VK_F11, false, false, true, IDM_WINDOW_TRANS);
	RegKey(VK_F11, false, true, true, IDM_WINDOW_FILETRANS);

	// 전역 단축키
	RegKey(VK_F12, true, false, true, IDM_EXTERN_HOTKEY, true);

	// 사전 오픈
	RegKey(VK_INSERT, false, false, true, IDM_OPENDIC, true);

	// 자석모드
	RegKey(VK_OEM_3, false, false, false, IDM_MAGNETIC_MODE, true);

	// 마우스 클릭
	RegKey(VK_NUMPAD0, false, false, false, IDM_MOUSE_LCLICK, true);
	RegKey(VK_DECIMAL, false, false, false, IDM_MOUSE_RCLICK, true);

	// 창 번역
	RegKey(VK_F9, false, false, true, IDM_TRANSTEXT_WNDMENU);
	RegKey(VK_F10, false, false, true, IDM_TRANSTEXT_WNDTEXT);

	// 클립보드 감시
	RegKey(VK_SCROLL, false, false, false, IDM_CLIPBOARD_SWITCH, true);

	return true;
}

inline void CHotkey::RegKey(int code, bool bCtrl, bool bAlt, bool bShift, int func, bool bAlways)
{
	struct _key_map kmap;
	kmap.Alt = bAlt;
	kmap.Shift = bShift;
	kmap.Ctrl = bCtrl;
	kmap.Code = code;
	kmap.func = func;
	kmap.Always = bAlways;
	key_map.push_back(kmap);
}

inline void CHotkey::UnregKey(int code)
{
	std::vector<_key_map>::iterator it = key_map.begin();
	for (; it != key_map.end(); it++)
	{
		if ((*it).Code == code)
		{
			key_map.erase(it);
			it = key_map.begin();
		}
	}
}

inline void CHotkey::UnregFuncKey(int func)
{
	std::vector<_key_map>::iterator it = key_map.begin();
	for (; it != key_map.end(); it++)
	{
		if ((*it).func == func)
		{
			key_map.erase(it);
			it = key_map.begin();
		}
	}
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
			std::vector<_key_map>::iterator it = key_map.begin();
			for (; it != key_map.end(); it++)
			{
				if ((*it).Code == pHookKey->vkCode)
				{
					bool bCtrl = ((GetKeyState(VK_CONTROL) & 0x8000) ? true : false);
					bool bShift = ((GetKeyState(VK_SHIFT) & 0x8000) ? true : false);
					bool bAlt = ((GetKeyState(VK_MENU) & 0x8000) ? true : false);
					
					if ((*it).Alt != bAlt) continue;
					if ((*it).Shift != bShift) continue;
					if ((*it).Ctrl != bCtrl) continue;
					
					if ((*it).Always == true || Cl.Config->GetExternHotkey() &&
						(Cl.Config->GetWindowVisible() || !Cl.Config->GetWindowVisible() &&
						(Cl.Config->GetTempWinHide() || !Cl.Config->GetTempWinHide() &&
						!Cl.Config->GetHideWinUnlockHotkey())))
					{
						SendMessage(hWnds.Main, WM_COMMAND, MAKELONG((*it).func, 0), 0);
						SendMessage(hWnds.Main, WM_COMMAND, IDM_SETTING_CHECK, 0);
						return -1;
					}
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