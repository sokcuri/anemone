#pragma once
class CTextProcess
{
public:
	CTextProcess();
	~CTextProcess();

	void StartWatchClip();
	void EndWatchClip();
	bool OnDrawClipboard();
private:
	std::wstring eztrans_proc(std::wstring &input);
	std::wstring HangulEncode(std::wstring &input);
	std::wstring HangulDecode(std::wstring &input);
	std::wstring NameSplit(int nCode, std::wstring &input);
};

