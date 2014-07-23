#pragma once
class CTextProcess
{
public:
	CTextProcess();
	~CTextProcess();

	void StartWatchClip();
	void EndWatchClip();
	bool OnDrawClipboard();
	//std::wstring eztrans_mt_proc(std::wstring &input);
	//unsigned int WINAPI ThreadFunction(void *arg);
	std::wstring TranslateText(HWND hWnd, const std::wstring &input);
	void CTextProcess::TranslateAbort();
	std::wstring eztrans_proc(const std::wstring &input);
private:
	std::wstring HangulEncode(const std::wstring &input);
	std::wstring HangulDecode(const std::wstring &input);
	std::wstring NameSplit(int nCode, std::wstring &input);
	bool DoubleTextFix(std::wstring &input);
	bool DoubleSentenceFix(std::wstring &input);

	int nStatus = 0;
	std::wstring proclog;
};

