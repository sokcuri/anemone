#include "StdAfx.h"
#include "TextProcess.h"


CTextProcess::CTextProcess()
{
	Cl.TextProcess->StartWatchClip();
}


void CTextProcess::StartWatchClip()
{
	if (IsActive == 0)
	{
		IsActive = 3;
		hWnds.Clip = SetClipboardViewer(hWnds.Main);
		Cl.TextRenderer->Paint();
		if (!Cl.Config->GetClipSwitch()) EndWatchClip();
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
	int i = 0, length = input.length();
	std::wstring empty = L"Abort";

	if (nStatus != 0) return empty;
	nStatus = 1;

	SendMessage(hWnd, WM_COMMAND, IDM_TRANS_START, 0);

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
			SendMessage(hWnd, WM_COMMAND, IDM_TRANS_ABORT, (LPARAM)proclog.c_str());

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
		SendMessage(hWnd, WM_COMMAND, IDM_TRANS_PROGRESS, (LPARAM)proclog.c_str());
	}

	proclog = output;
	SendMessage(hWnd, WM_COMMAND, IDM_TRANS_COMPLETE, (LPARAM)proclog.c_str());

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
	/*
	std::wregex regex(L"(?:(.*[^「『（(])([「『（(].*))$");

	if (std::regex_match(input, m, regex))
	{
		nHardFix = true;
		output = m[1];
		text = m[2];
		if (m[2] == L"") text = input;
	}
	else text = input;
	*/

	//if (text.size() < 8) return false;

	for (unsigned int i = 0; i < text.size(); i++)
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
		else
		{
			// 「テティィナナ 같은 경우 처리 (앞 1글자가 짤렸을 경우) / 강에서만 작동
			if (i == 0 && Cl.Config->GetRepeatTextProc() > 3)
			{
				bool IsNext = false;
				for (i = 1; ; i++)
				{
					if (i == text.size() || i == (text.size() / 2) - (text.size() % 2))
					{
						IsNext = true;
						break;
					}
					else if (text[i] == text[i + 1]) i++;
					else
					{
						i = 0;
						break;
					}
				}

				if (IsNext == true)
				{
					output = text[0];
					i = 0;
					continue;
				}
			}
			// 4자 (중복 8자)도 안되면 문제가 있을 수 있음. 무조건 리턴
			//if (i < 4 * 2) return false;

			for (; i < text.size(); i++)
				output += text[i];
		}
	}

	input = output;
	return true;
}

bool CTextProcess::DoubleSentenceFix(std::wstring &input)
{
	int nResult = 1;
	std::wstring str = input;
	int index = str.size() / 2;
	bool Is0D = false, Is0A = false;

	for (unsigned int i = 0; i < str.length(); i++, index++)
	{
		if (i == str.length() / 2)
		{
			nResult = 1;
			break;
		}

		if (str[i] != str[index])
		{
			nResult = 0;
			break;
		}
	}

	if (nResult == 0)
	{
		index = str.length() / 2 + 1;

		for (unsigned int i = 0; i < str.length(); i++, index++)
		{
			if (index == str.length() && str.length() / 2 - i < 1)
			{
				nResult = 2;
				break;
			}

			if (str[i] != str[index])
			{
				nResult = 0;

				// 「BBB」AAAAAA「BBB」같은 경우 처리

				std::wstring str_end = str.substr(str.length() / 2, str.length() / 2);
				std::wstring str_first = str.substr(0, str.length() / 2);
				
				for (unsigned int i = 0; i < str.length() / 2; i++, index--)
				{
					str_first = str_first[str_first.length() - 1] + str_first.substr(0, str_first.length() - 1);

					if (str_end == str_first)
					{
						input = str_first;
						return true;
					}
				}
				break;
			}
		}
	}

	if (nResult == 1)
	{
		// 1234 12345
		if (str.length() % 2 == 1)
		{
			str = str.substr(str.length() / 2, str.length() / 2 + 1);
		}
		// 1234 1234
		else
		{
			str = str.substr(0, str.length() / 2);
		}
	}

	// 12345 1234
	else if (nResult == 2)
	{
		str = str.substr(0, str.length() / 2 + 1);
	}
	else if (nResult == 3)
	{
		str = str.substr(str.length() / 2, str.length() / 2);
	}

	input = str;
	return true;
}

std::wstring CTextProcess::NameSplit(int nCode, std::wstring &input)
{
	// 이름인식 최대길이
	DWORD nNameLen = 10;

	/*
	* [2013-12-13] 이름인식 정규식 by MYC Gamer
	* [2013-07-23] 이름인식 정규식 V2 (대사 끝에 붙어 있는 이름 인식, 일반 괄호 반복 인식)
	*
	*【AA】【AA】【AA】【AA】BB
	*【AA】【AA】BB
	*【AA】【AA【AABB
	*【AA】AAAABB
	*【AACAACAAC】BB
	*
	*/
	std::wregex reg_front_r1, reg_front_r2, reg_front_r3, reg_front_r4, reg_front_r5;
	std::wregex reg_tail_r1, reg_tail_r2, reg_tail_r3, reg_tail_r4, reg_tail_r5;
	
	if (Cl.Config->GetRepeatTextProc() > 0)
	{
		reg_front_r1.assign(L"^【([^】]+?)(?:】(?:【\\1)+|】\\1+|(?!([^】])\\2*】)\\1*】)([^【】].*)");
		reg_front_r2.assign(L"^([^「]+?)(?:[「](?:\\1)+|[」]\\1+|(?!([^」])\\1*[」])\\1*([「](.*」|[^」]+?)$))");
		reg_front_r3.assign(L"^([^『]+?)(?:[『](?:\\1)+|[』]\\1+|(?!([^』])\\1*[』])\\1*([『](.*』|[^』]+?)$))");
		reg_front_r4.assign(L"^([^(]+?)(?:[(](?:\\1)+|[)]\\1+|(?!([^)])\\1*[)])\\1*([(].*[)]))");
		reg_front_r5.assign(L"^([^（]+?)(?:[（](?:\\1)+|[）]\\1+|(?!([^）])\\1*[）])\\1*([（].*[）]))");
		//rx_head5.assign(L"^([^「」『』（）()]+?)(?:[「『（(](?:\\1)+|[」』）)]\\1+|(?!([^」』）)])\\1*[」』）)])\\1*([「」『』（）()].*[）)]))");
		//rx_name.assign(L"【([^】]+?)(?:】(?:【\\1)+|】\\1+|(?!([^】])\\2*】)\\1*】)([^【】].*)");
		//rx_name2.assign(L"^([^「」『』（）()]+?)(「(?:[^」]+$|.+」$)|『(?:[^』]+$|.+』$)|\\(.*\\)$|（.*）$)");
		//rx_name4.assign(L"^([「」『』（）()].*[「」『』（）()])([^「」『』（）()]+?)(?:[「『（(](?:\\2)+|[」』）)]\\2+|(?!([^」』）)])\\2*[」』）)])\\2*)");

		reg_tail_r1.assign(L"^(.*[^【】])【([^】]+?)(?:】(?:【\\2】|【\\2)+|】\\2+|(?!([^】])\\3*】)\\2*】)$");
		reg_tail_r2.assign(L"^([「].*[」]|[^「]+?[^「][」])([^「]+?)(?:[「](?:\\2)+|[」]\\2+|(?!([^」])\\2*[」])\\2*)$");
		reg_tail_r3.assign(L"^([『].*[』]|[^『]+?[^『][』])([^『]+?)(?:[『](?:\\2)+|[』]\\2+|(?!([^』])\\2*[』])\\2*)$");
		reg_tail_r4.assign(L"^([(].*[)])([^(]+?)(?:[(](?:\\2)+|[)]\\2+|(?!([^)])\\2*[)])\\2*)");
		reg_tail_r5.assign(L"^([（].*[）])([^（]+?)(?:[（](?:\\2)+|[）]\\2+|(?!([^）])\\2*[）])\\2*)");
	}
	else
	{
		reg_front_r1.assign(L"^【((?:[^】]+$|(.+)))】([^【】]+?)$");
		reg_front_r2.assign(L"^([^「」『』（）()]+?)()(「(?:[^」]+$|.+」$)|『(?:[^』]+$|.+』$)|[(（].*[)）]$)");

		reg_tail_r1.assign(L"^([^【】]+?)【((?:[^】]+$|(.+)))】$");
		reg_tail_r2.assign(L"^(「(?:[^」]+$|.+」)|『(?:[^』]+$|.+』)|[(（].*[)）])([^「」『』（）()]+?)$");
	}

	std::wsmatch m;

	std::wstring wName, wText;
	std::wstring wNameTrans, wTextTrans;
	std::wstring wSpace = L" ";
	std::wstring wTab = L"　";
	std::wstring wEmpty = L"";

	// 【이름】
	if (std::regex_match(input, m, reg_front_r1))
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
	else if (std::regex_match(input, m, reg_front_r2) || 
			std::regex_match(input, m, reg_front_r3) || 
			std::regex_match(input, m, reg_front_r4) || 
			std::regex_match(input, m, reg_front_r5))
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
			// 【이름】
			if (std::regex_match(input, m, reg_tail_r1))
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
			else if (std::regex_match(input, m, reg_tail_r2) || 
					std::regex_match(input, m, reg_tail_r3) || 
					std::regex_match(input, m, reg_tail_r4) || 
					std::regex_match(input, m, reg_tail_r5))
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

	// 클립보드 감시가 꺼져있는지 확인
	if (!Cl.Config->GetClipSwitch())
	{
		Cl.TextProcess->EndWatchClip();
		return 0;
	}

	// 창 숨김시 클립보드 비활성화 옵션 확인
	else if (!Cl.Config->GetWindowVisible() && !Cl.Config->GetTempWinHide() && Cl.Config->GetHideWinUnWatchClip())
	{
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

	// 클립보드 인식 길이가 넘어가면 버리기
	if (wContext.length() > (unsigned int)Cl.Config->GetClipLength())
	{
		CloseClipboard();
		return false;
	}

	// 반복 처리
	if (Cl.Config->GetRepeatTextProc() > 2)
		DoubleSentenceFix(wContext);
	if (Cl.Config->GetRepeatTextProc() > 1)
		DoubleTextFix(wContext);

	// 이름 분리
	wName = NameSplit(0, wContext);
	if (Cl.Config->GetRepeatTextProc() > 2)
		DoubleTextFix(wName);

	// 대사 분리
	wText = NameSplit(1, wContext);
	if (Cl.Config->GetRepeatTextProc() > 1)
		DoubleTextFix(wText);

	// 이름이나 대사에 반복되는 부분 처리
	if (Cl.Config->GetRepeatTextProc() > 3)
	{
		DoubleSentenceFix(wName);
		DoubleSentenceFix(wText);
	}

	wContext = wName;
	wContext += wText;

	wNameT = eztrans_proc(wName);
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
