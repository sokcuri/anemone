#pragma once
class CFileWatch
{
public:
	CFileWatch();
	~CFileWatch();
	bool IsWatchConfig();
	void SetWatchConfig(bool b);
private:
	static CFileWatch *m_pThis;
	HANDLE hWatchThread;

	DWORD _FileChangeNotifyThread(LPVOID lpParam);
	MMRESULT _FileChangeNotifyProc(UINT m_nTimerID, UINT uiMsg, DWORD dwUser, DWORD dw1, DWORD d2);

	// Trampoline
	static DWORD WINAPI FileChangeNotifyThread(LPVOID lpParam)
	{
		return m_pThis->_FileChangeNotifyThread(lpParam);
	}

	static MMRESULT CALLBACK FileChangeNotifyProc(UINT m_nTimerID, UINT uiMsg, DWORD dwUser, DWORD dw1, DWORD d2)
	{
		return m_pThis->_FileChangeNotifyProc(m_nTimerID, uiMsg, dwUser, dw1, d2);
	}
	static bool watch_config;
	static bool watch_anedic;
	static bool init_wait;
};

