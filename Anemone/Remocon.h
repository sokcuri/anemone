#pragma once
class CRemocon
{
public:
	CRemocon();
	~CRemocon();
	bool CreateInstance();

	static DWORD bgColor;
	static bool m_bTracking;
	static int m_nBtnStatus;
	
private:
	static CRemocon *m_pThis;
	bool Paint();
	LRESULT _WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

	// Trampoline
	static LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
	{
		return m_pThis->_WndProc(hWnd, message, wParam, lParam);
	}
};

