#include "StdAfx.h"
#include "TextProcess.h"


CTextProcess::CTextProcess()
{
	StartWatchClip();
}


void CTextProcess::StartWatchClip()
{
	if (IsActive == 0)
	{
		IsActive = 3;
		hWnds.Clip = SetClipboardViewer(hWnds.Main);
		Cl.TextRenderer->Paint();
	}
	else
	{
		IsActive = 2;
		hWnds.Clip = SetClipboardViewer(hWnds.Main);
	}
}

void CTextProcess::EndWatchClip()
{
	ChangeClipboardChain(hWnds.Main, NULL);
}
/*
std::wstring CTextProcess::eztrans_mt_proc(std::wstring &input)
{
	std::hash_map<int, std::wstring> h1;
	
	int i = 0;
	std::wstring::size_type nprev = 0;
	std::wstring::size_type npos = -1;
	for (;i < 10; i++)
	{
		nprev = npos + 1;
		npos = input.find(L"\n", nprev);
		if (npos != std::string::npos)
		{
			h1[i] = input.substr(nprev, npos-nprev);
		}
		else
		{
			h1[i] = input.substr(nprev);
			//MessageBox(0, h1[1].c_str(), 0, 0);
			break;
		}
	}
	//MessageBox(0, str, 0, 0);

	std::wstring output = L"END";

	std::hash_map<int, std::wstring> *pt;
	pt = &h1;
	for (int j = 0; j < i; j++)
	{
		HANDLE hThread = NULL;
		DWORD dwThreadID = NULL;
		hThread = (HANDLE)_beginthreadex(NULL, 0, this->ThreadFunction, pt, 0, (unsigned*)&dwThreadID);
		if (hThread == 0) MessageBox(0, L"_beginthreadex Error\n", 0, 0);
	}
	
	HANDLE hThread = NULL;
	DWORD dwThreadID = NULL;
	hThread = (HANDLE)_beginthreadex(NULL, 0, this->ThreadFunction, NULL, 0, (unsigned*)&dwThreadID);
	if (hThread == 0) MessageBox(0, L"_beginthreadex Error\n", 0, 0);
	return output;
}

unsigned int WINAPI CTextProcess::ThreadFunction(void *arg)
{
	return 0;
}
*/

std::wstring CTextProcess::TranslateText(HWND hWnd, const std::wstring &input)
{
	Elapsed_Prepare = 0;
	Elapsed_Translate = 0;

	std::wstring::size_type nprev = 0;
	std::wstring::size_type npos = -1;
	std::list<std::wstring> list;
	std::wstring output;
	int i = 0, length = input.size();
	std::wstring empty = L"Abort";

	if (nStatus != 0) return empty;
	nStatus = 1;

	PostMessage(hWnd, WM_COMMAND, IDM_TRANS_START, 0);

	for (; ; i++)
	{
		nprev = npos + 1;
		npos = input.find(L"\n", nprev);
		if (npos != std::string::npos)
		{
			list.push_back(input.substr(nprev, npos - nprev + 1));
		}
		else
		{
			list.push_back(input.substr(nprev));
			break;
		}
	}

	std::list<std::wstring>::iterator iter;
	if (Cl.Config->GetTransOneGo() && list.size() > 10)
	{
		std::wstring line;
		std::list<std::wstring> list2 = list;
		list.clear();
		int j = 1;
		for (i = 1, iter = list2.begin(); iter != list2.end(); iter++, i++)
		{
			if ((float)i > (float)(list2.size() * j / 10))
			{
				list.push_back(line);
				line = L"";
				j++;
			}

			line += (*iter);

			if (i == list2.size())
			{
				list.push_back(line);
				line = L"";
			}

		}
	}

	for (i=0, iter = list.begin(); iter != list.end(); iter++, i++)
	{
		if (nStatus == 2)
		{
			nStatus = 0;
			std::wstringstream logstream;
			logstream << L"번역 중지 (";
			logstream << i + 1;
			logstream << L"/";
			logstream << list.size();
			logstream << L")";

			proclog = logstream.str();
			PostMessage(hWnd, WM_COMMAND, IDM_TRANS_ABORT, (LPARAM)proclog.c_str());

			std::wstring abort_msg = L"Abort";
			return abort_msg;
		}

		output += eztrans_proc(*iter);

		std::wstringstream logstream;
		logstream << L"번역중... (";
		logstream << i + 1;
		logstream << L"/";
		logstream << list.size();
		logstream << L")";
		logstream << L"\r\n";
		logstream << L"Prepare : ";
		logstream << Elapsed_Prepare;
		logstream << L"ms\r\n";
		logstream << L"Translate : ";
		logstream << Elapsed_Translate;
		logstream << L"ms";

		proclog = logstream.str();
		PostMessage(hWnd, WM_COMMAND, IDM_TRANS_PROGRESS, (LPARAM)proclog.c_str());
	}

	proclog = output;
	PostMessage(hWnd, WM_COMMAND, IDM_TRANS_COMPLETE, (LPARAM)proclog.c_str());

	nStatus = 0;

	return output;
}

void CTextProcess::TranslateAbort()
{
	if (nStatus == 1) nStatus = 2;
	return;
}

std::wstring CTextProcess::eztrans_proc(const std::wstring &input)
{
	int nBufLen;
	char *szBuff, *szBuff2;
	wchar_t *lpszBuff;
	std::wstring szContext, output;

	int start, end;

	start = GetTickCount();

	szContext = HangulEncode(input);

	// 이지트랜스 오류 잡아주기
	// 「よろしければ今度２人でお話しなどできないでしょうか」
	szContext = replaceAll(szContext, L"できないでしょ", L"@X@でき@X@ないでしょ");
	szContext = replaceAll(szContext, L"きないでしょ", L"き@X@ないでしょ");
	szContext = replaceAll(szContext, L"でき@X@ないでしょ", L"できないでしょ");

	nBufLen = WideCharToMultiByte(932, 0, szContext.c_str(), -1, NULL, NULL, NULL, NULL);
	szBuff = (char *)HeapAlloc(AneHeap, 0, (nBufLen + 2) * 2);

	if (szBuff == NULL)
	{
		MessageBox(0, L"메모리 할당 실패", 0, 0);
		return false;
	}

	WideCharToMultiByte(932, 0, szContext.c_str(), -1, szBuff, nBufLen, NULL, NULL);

	end = GetTickCount();

	Elapsed_Prepare += (end - start);

	start = GetTickCount();

	szBuff2 = (char *)Cl.TransEngine->J2K_TranslateMMNT(0, szBuff);

	end = GetTickCount();

	Elapsed_Translate += (end - start);

	start = GetTickCount();

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

	end = GetTickCount();

	Elapsed_Prepare += (end - start);
	return output;
}

std::wstring CTextProcess::HangulEncode(const std::wstring &input)
{
	std::wstring output;
	wchar_t buf[8];

	std::wstring::const_iterator it = input.begin();
	for (; it != input.end(); it++)
	{
		if (*it == L'@' ||
			(*it >= 0x1100 && *it <= 0x11FF) || (*it >= 0x3130 && *it <= 0x318F) ||
			(*it >= 0xA960 && *it <= 0xA97F) || (*it >= 0xAC00 && *it <= 0xD7AF) ||
			(*it >= 0xD7B0 && *it <= 0xD7FF))
		{
			swprintf_s(buf, L"+x%04X", *it);
			output += buf;
		}
		else
		{
			output += *it;
		}
	}

	return output;
}
std::wstring CTextProcess::HangulDecode(const std::wstring &input)
{
	std::wstring output;
	wchar_t buf[8];
	std::wstring::const_iterator it = input.begin();
	for (DWORD count = 0; it != input.end(); it++, count++)
	{
		// @X = 삭제
		if (count + 2 < input.length() && (*it) == L'@' && *(it + 1) == L'X' && *(it + 2) == L'@')
		{
			it += 2;
			count += 2;
			continue;
		}
		else
			if (count + 5 < input.length() && *(it) == '+' && *(it + 1) == 'x' &&
			((*(it + 2) >= L'A' && *(it + 2) <= L'Z') || (*(it + 2) >= L'a' && *(it + 2) <= L'z') || (*(it + 2) >= L'0' && *(it + 2) <= L'9')) &&
			((*(it + 3) >= L'A' && *(it + 3) <= L'Z') || (*(it + 3) >= L'a' && *(it + 3) <= L'z') || (*(it + 3) >= L'0' && *(it + 3) <= L'9')) &&
			((*(it + 4) >= L'A' && *(it + 4) <= L'Z') || (*(it + 4) >= L'a' && *(it + 4) <= L'z') || (*(it + 4) >= L'0' && *(it + 4) <= L'9')) &&
			((*(it + 5) >= L'A' && *(it + 5) <= L'Z') || (*(it + 5) >= L'a' && *(it + 5) <= L'z') || (*(it + 5) >= L'0' && *(it + 5) <= L'9')))
		{
		buf[0] = *(it + 2);
		buf[1] = *(it + 3);
		buf[2] = *(it + 4);
		buf[3] = *(it + 5);
		buf[4] = 0x00;

		swscanf_s(buf, L"%04x", &buf[0]);

		output += buf;
		it += 5;
		count += 5;

		}
		else
		{
			output += (*it);
		}
	}
	return output;
}

bool CTextProcess::DoubleTextFix(std::wstring &input)
{
	std::wstring output;
	std::wstring text = input;
	bool nHardFix = false;

	std::wsmatch m;
	std::wregex regex(L"^([^「」『』（）()]+)(.*)$");

	if (std::regex_match(input, m, regex))
	{
		nHardFix = true;
		output = m[1];
		text = m[2];
	}

	for (unsigned int i = 0; i < text.size() / 2 * 2; i++)
	{
		if (text[i] == text[i + 1])
		{
			output += text[i];
			i++;
		}
		else if (text[i] == 0x0D || text[i] == 0x0A) 
		{
			output += text[i];
		}
		else if (nHardFix == true)
		{
			output += text[i];
		}
		else return false;
	}

	input = output;
	return true;
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
	std::wregex rx_name, rx_name2, rx_name3, rx_name4, rx_repeat;
	
	rx_repeat.assign(L"");

	// 늘어짐 문장 수정
	if (Cl.Config->GetRepeatTextProc()) DoubleTextFix(input);

	if (Cl.Config->GetRepeatTextProc())
	{
		rx_name.assign(L"^【([^】]+?)(?:】(?:【\\1)+|】\\1+|(?!([^】])\\2*】)\\1*】)([^【】].*)");
		rx_name2.assign(L"^([^「」『』（）()]+?)(?:[「」『』（）()](?:\\1)+|[「」『』（）()]\\1+|(?!([^「」『』（）()])\\1*[「」『』（）()])\\1*([「」『』（）()].*))"); 
		//rx_name.assign(L"【([^】]+?)(?:】(?:【\\1)+|】\\1+|(?!([^】])\\2*】)\\1*】)([^【】].*)");
		//rx_name2.assign(L"^([^「」『』（）()]+?)(「(?:[^」]+$|.+」$)|『(?:[^』]+$|.+』$)|\\(.*\\)$|（.*）$)");

		rx_name3.assign(L"^(.*[^【】])【([^】]+?)(?:】(?:【\\2】|【\\2)+|】\\2+|(?!([^】])\\3*】)\\2*】)$");
		rx_name4.assign(L"^(.*[「」『』（）()])([^「」『』（）()]+?)(?:[「」『』（）()](?:\\1)+|[「」『』（）()]\\1+|(?!([^「」『』（）()])\\1*[「」『』（）()])\\1*)$");
	}
	else
	{
		rx_name.assign(L"^【((?:[^】]+$|(.+)))】([^【】]+?)$");
		rx_name2.assign(L"^([^「」『』（）()]+?)(「(?:[^」]+$|.+」$)|『(?:[^』]+$|.+』$)|[(（].*[)）]$)");

		rx_name3.assign(L"^([^【】]+?)【((?:[^】]+$|(.+)))】$");
		rx_name4.assign(L"^(「(?:[^」]+$|.+」)|『(?:[^』]+$|.+』)|[(（].*[)）])([^「」『』（）()]+?)$");
	}

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
		wName = replaceAll(wName, wTab, wEmpty); //(이름)대사
		wName = replaceAll(wName, L"【】", wEmpty);
	}
	// 이름 처리
	else if (std::regex_match(input, m, rx_name2))
	{
		wName = m[1];
		wText = m[3];
		wName = replaceAll(wName, wSpace, wEmpty);
		wName = replaceAll(wName, wTab, wEmpty);
	}
	else
	{
		wName = wEmpty;
		wText = input;

		if (Cl.Config->GetReviseName())
		{
			if (Cl.Config->GetRepeatTextProc())
			{
				rx_name3.assign(L"^(.*[^【】])【([^】]+?)(?:】(?:【\\2】|【\\2)+|】\\2+|(?!([^】])\\3*】)\\2*】)$");
				rx_name4.assign(L"^(.*[「」『』（）()])([^「」『』（）()]+?)(?:[「」『』（）()](?:\\1)+|[「」『』（）()]\\1+|(?!([^「」『』（）()])\\1*[「」『』（）()])\\1*)$");
			}
			else
			{
				rx_name3.assign(L"^([^【】]+?)【((?:[^】]+$|(.+)))】$");
				rx_name4.assign(L"^(「(?:[^」]+$|.+」)|『(?:[^』]+$|.+』)|[(（].*[)）])([^「」『』（）()]+?)$");
			}

			// 【이름】
			if (std::regex_match(input, m, rx_name3))
			{
				wName = L"";
				wName += L"【";
				wName += m[2];
				wName += L"】";
				wText = m[1];

				wName = replaceAll(wName, wSpace, wEmpty);
				wName = replaceAll(wName, wTab, wEmpty); //(이름)대사
				wName = replaceAll(wName, L"【】", wEmpty);
			}
			// 이름 처리
			else if (std::regex_match(input, m, rx_name4))
			{
				wName = m[2];
				wText = m[1];
				wName = replaceAll(wName, wSpace, wEmpty);
				wName = replaceAll(wName, wTab, wEmpty);
			}
		}
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
	std::wstring wName, wNameT, wText, wTextT, wContext, wContextT;

	// 클립보드를 새로 등록했을 때 현재 저장되어 있는 클립보드 내용을 무시
	if (IsActive == 2)
	{
		IsActive = 1;
		return 0;
	}
	else if (IsActive == 3)
	{
		IsActive = 0;
		return 0;
	}


	OpenClipboard(hWnds.Clip);
	HANDLE hClipData = GetClipboardData(CF_UNICODETEXT);
	
	if (hClipData == NULL)
	{
		Cl.TextRenderer->Paint(); 
		CloseClipboard();
		return false;
	}

	wContext = (wchar_t*)GlobalLock(hClipData);
	GlobalUnlock(hClipData);

	wName = NameSplit(0, wContext);
	wNameT = eztrans_proc(wName);

	wText = NameSplit(1, wContext);
	wTextT = eztrans_proc(wText);

	wContextT = wNameT;
	wContextT += wTextT;

	*(Cl.TextRenderer->szName) = wName;
	*(Cl.TextRenderer->szNameT) = wNameT;
	*(Cl.TextRenderer->szText) = wText;
	*(Cl.TextRenderer->szTextT) = wTextT;
	*(Cl.TextRenderer->szContext) = wContext;
	*(Cl.TextRenderer->szContextT) = wContextT;
	
	CloseClipboard();
	
	IsActive = true;
	Cl.TextRenderer->Paint();

	// 임시 창 숨김 상태일때 클립보드 요청이 들어오면 창을 다시 띄운다
	if (Cl.Config->GetTempWinHide())
	{
		PostMessage(hWnds.Main, WM_COMMAND, IDM_TEMP_WINDOW_HIDE, 0);
		PostMessage(hWnds.Main, WM_COMMAND, IDM_SETTING_CHECK, 0);
	}

	return true;
}

CTextProcess::~CTextProcess()
{
	// 클립보드 감시 종료
	EndWatchClip();
}
