#pragma once
class CHotkey
{
public:
	CHotkey();
	~CHotkey();

	bool InstallHook();
	bool RemoveHook();

	bool LoadKeyMap();
	bool SaveKeyMap();

private:
	static CHotkey *m_pThis;
	HANDLE hHotkeyThread;
	HHOOK m_hHook;

	DWORD _HotkeyThread(LPVOID lpParam);

	// Trampoline
	static DWORD WINAPI HotkeyThread(LPVOID lpParam)
	{
		return m_pThis->_HotkeyThread(lpParam);
	}
	// Trampoline
	static LRESULT CALLBACK _KeyboardProc(int code, WPARAM wParam, LPARAM lParam)
	{
		return m_pThis->KeyboardProc(code, wParam, lParam);
	}

	inline void RegKey(int code, bool bCtrl, bool bAlt, bool bShift, int func, bool bAlways = false);
	inline void UnregKey(int code);
	inline void UnregFuncKey(int func);

	LRESULT KeyboardProc(int code, WPARAM wParam, LPARAM lParam);
};

