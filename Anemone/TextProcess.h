#pragma once

class CTextProcess
{
public:
	CTextProcess();
	~CTextProcess();

	void StartWatchClip();
	void EndWatchClip();
	void ResetWatchClip();
	void StartHookMonitor();
	void EndHookMonitor();
	bool OnDrawClipboard();
	bool OnDrawClipboardByHooker(wchar_t *lpwszstr);
	//std::wstring eztrans_mt_proc(std::wstring &input);
	//unsigned int WINAPI ThreadFunction(void *arg);
	bool TranslateText(HWND hWnd, const std::wstring &input, int nOutputType);
	void TranslateAbort();
	std::wstring eztrans_proc(const std::wstring &input);
	bool LoadDictionary();
private:
	static CTextProcess *m_pThis;
	std::wstring HangulEncode(const std::wstring &input);
	std::wstring HangulDecode(const std::wstring &input);
	std::wstring NameSplit(int nCode, std::wstring &input);
	bool DoubleTextFix(std::wstring &input);
	bool DoubleSentenceFix(std::wstring &input);

	bool bLoadDic = false;
	int nStatus = 0;
	std::wstring proclog;
	bool g_DicTopPriority;

	bool _LoadDic(const wchar_t *dicFile);
	void *_PatchUDic(const wchar_t *dicFile);
	bool _UnPatchUDic(const wchar_t *dicFile, void *offile);
	void ApplyForceAneDicAll(std::wstring &input);
	void ApplyForceAneDic(std::wstring &input);
	bool ProcessText(std::wstring &wContext);

	HANDLE hHookMonitorThread;
	DWORD WINAPI _HookMonitorProc(LPVOID lpParam);

	// Trampoline
	static DWORD WINAPI HookMonitorProc(LPVOID lpParam)
	{
		return m_pThis->_HookMonitorProc(lpParam);
	}

};

