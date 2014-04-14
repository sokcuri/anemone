#include "StdAfx.h"
#include "TextProcess.h"


CTextProcess::CTextProcess()
{
	StartWatchClip();
}


void CTextProcess::StartWatchClip()
{
	hWnds.Clip = SetClipboardViewer(hWnds.Main);
}

void CTextProcess::EndWatchClip()
{
	ChangeClipboardChain(hWnds.Clip, NULL);
}


std::wstring CTextProcess::eztrans_proc(std::wstring &input)
{
	int nBufLen;
	char *szBuff, *szBuff2;
	wchar_t *lpszBuff;
	std::wstring szContext, output;

	szContext = HangulEncode(input);

	nBufLen = WideCharToMultiByte(932, 0, szContext.c_str(), -1, NULL, NULL, NULL, NULL);
	szBuff = (char *)HeapAlloc(AneHeap, 0, (nBufLen + 2) * 2);

	if (szBuff == NULL)
	{
		MessageBox(0, L"메모리 할당 실패", 0, 0);
		return false;
	}

	WideCharToMultiByte(932, 0, szContext.c_str(), -1, szBuff, nBufLen, NULL, NULL);

	szBuff2 = (char *)Cl.TransEngine->J2K_TranslateMMNT(0, szBuff);
	HeapFree(AneHeap, 0, szBuff);

	nBufLen = MultiByteToWideChar(949, 0, szBuff2, -1, NULL, NULL);
	lpszBuff = (wchar_t *)HeapAlloc(AneHeap, 0, (nBufLen + 2) * 2);

	if (lpszBuff == NULL)
	{
		MessageBox(0, L"메모리 할당 실패", 0, 0);
		return false;
	}

	MultiByteToWideChar(949, 0, szBuff2, -1, lpszBuff, nBufLen);

	output = lpszBuff;
	HeapFree(AneHeap, 0, lpszBuff);
	Cl.TransEngine->J2K_FreeMem(szBuff2);

	output = HangulDecode(output);

	return output;
}

std::wstring CTextProcess::HangulEncode(std::wstring &input)
{
	std::wstring output;
	wchar_t buf[8];

	for (DWORD i = 0; i<input.size(); i++)
	{
		if (input[i] == L'\\')
		{
			output += L"\\\\";
			continue;
		}
		else
		if (input[i] == L'@')
		{
			output += L"@@";
			continue;
		}
		else
		if ((input[i] >= 0x1100 && input[i] <= 0x11FF) || (input[i] >= 0x3130 && input[i] <= 0x318F) ||
			(input[i] >= 0xA960 && input[i] <= 0xA97F) || (input[i] >= 0xAC00 && input[i] <= 0xD7AF) ||
			(input[i] >= 0xD7B0 && input[i] <= 0xD7FF))
		{
			swprintf_s(buf, L"\\x%04X", input[i]);
			output += buf;
		}
		else
		{
			output += input[i];
		}
	}

	return output;
}
std::wstring CTextProcess::HangulDecode(std::wstring &input)
{
	std::wstring output;
	wchar_t buf[8];

	for (DWORD i = 0; i<input.size(); i++)
	{
		if ((input[i] == L'\\' || input[i] == L'@') && input[i] == input[i+1])
		{
			if (!(input[i] == L'@' && input[i + 1] == L'X')) // @X = 삭제
			{
				output += input[i];
				i++;
			}
			continue;
		}
		else
		if (i + 5 < input.size() && input[i] == '\\' && input[i + 1] == 'x' &&
		   ((input[i + 2] >= L'A' && input[i + 2] <= L'Z') || (input[i + 2] >= L'a' && input[i + 2] <= L'z') || (input[i + 2] >= L'0' && input[i + 2] <= L'9')) && 
		   ((input[i + 3] >= L'A' && input[i + 3] <= L'Z') || (input[i + 3] >= L'a' && input[i + 3] <= L'z') || (input[i + 3] >= L'0' && input[i + 3] <= L'9')) &&
		   ((input[i + 4] >= L'A' && input[i + 4] <= L'Z') || (input[i + 4] >= L'a' && input[i + 4] <= L'z') || (input[i + 4] >= L'0' && input[i + 4] <= L'9')) &&
		   ((input[i + 5] >= L'A' && input[i + 5] <= L'Z') || (input[i + 5] >= L'a' && input[i + 5] <= L'z') || (input[i + 5] >= L'0' && input[i + 5] <= L'9')))
		{
			buf[0] = input[i + 2];
			buf[1] = input[i + 3];
			buf[2] = input[i + 4];
			buf[3] = input[i + 5];
			buf[4] = 0x00;

			swscanf_s(buf, L"%04x", &buf[0]);
			//WriteLog(L"bytearray %d: %02x\n", 0, buf[0]);

			output += buf;
			i += 5;
		}
		else
		{
			output += input[i];
		}
	}
	return output;
}
std::wstring CTextProcess::NameSplit(int nCode, std::wstring &input)
{
	// 이름인식 최대길이
	DWORD nNameLen = 10;

	/*
	* [2013-12-13] 이름인식 정규식 by MYC Gamer
	*
	*【AA】【AA】【AA】【AA】BB
	*【AA】【AA】BB
	*【AA】【AA【AABB
	*【AA】AAAABB
	*【AACAACAAC】BB
	*
	*/

	std::wregex rx_name(L"【([^】]+?)(?:】(?:【\\1】|【\\1)+|】\\1+|(?!([^】])\\2*】)\\1*】)([^【】].*)");
	std::wregex rx_name2(L"^([^「」『』（）()]+?)(「(?:[^」]+$|.+」$)|『(?:[^』]+$|.+』$)|\\(.*\\)$|（.*）$)");
	std::wsmatch m;

	std::wstring wName, wText;
	std::wstring wNameTrans, wTextTrans;
	std::wstring wSpace = L" ";
	std::wstring wTab = L"　";
	std::wstring wEmpty = L"";

	// 【이름】
	if (std::regex_match(input, m, rx_name))
	{
		wName = L"";
		wName += L"【";
		wName += m[1];
		wName += L"】";
		wText = m[3];

		wName = replaceAll(wName, wSpace, wEmpty);
		wName = replaceAll(wName, wTab, wEmpty);//(이름)대사
		wName = replaceAll(wName, L"【】", wEmpty);
	}
	// 이름 처리
	else if (std::regex_match(input, m, rx_name2))
	{
		wName = m[1];
		wText = m[2];
		wName = replaceAll(wName, wSpace, wEmpty);
		wName = replaceAll(wName, wTab, wEmpty);
	}
	// 이름이 없을때
	else
	{
		wName = wEmpty;
		wText = input;
	}

	// 이름인식 길이를 넘어서면 이름 X
	if (wName.length() > 0 && nNameLen < wName.length())
	{
		wName = wEmpty;
		wText = input;
	}

	if (nCode == 0) return wName;
	return wText;
}

bool CTextProcess::OnDrawClipboard()
{
	std::wstring wName, wNameT, wText, wTextT, wContext;

	OpenClipboard(hWnds.Clip);
	HANDLE hClipData = GetClipboardData(CF_UNICODETEXT);
	
	if (hClipData == NULL)
	{
		Cl.TextRenderer->Paint(); 
		CloseClipboard();

		//MessageBox(0, L"클립보드 얻어오기 실패", 0, 0);
		return false;
	}

	wContext = (wchar_t*)GlobalLock(hClipData);
	GlobalUnlock(hClipData);

	wName = NameSplit(0, wContext);
	wNameT = eztrans_proc(wName);

	wText = NameSplit(1, wContext);
	wTextT = eztrans_proc(wText);

	*(Cl.TextRenderer->szName) = wName;
	*(Cl.TextRenderer->szNameT) = wNameT;
	*(Cl.TextRenderer->szText) = wText;
	*(Cl.TextRenderer->szTextT) = wTextT;
	*(Cl.TextRenderer->szContext) = wContext;
	//SendMessage(hWnds.Main, WM_PAINT, 0, 0);
	
	Cl.TextRenderer->Paint();

	CloseClipboard();

	ChangeClipboardChain(hWnds.Main, NULL);
	hWnds.Clip = SetClipboardViewer(hWnds.Main);
	return true;
}

CTextProcess::~CTextProcess()
{
	// 클립보드 감시 종료
	EndWatchClip();
}
