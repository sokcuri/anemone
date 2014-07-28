#include "StdAfx.h"
#include "TextProcess.h"


CTextProcess::CTextProcess()
{
	Cl.TextProcess->LoadDictionary();
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
	std::wstring wName, wNameT, wNameR, wText, wTextT, wTextR, wContext, wContextT;

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

	wNameR = wName;
	wTextR = wText;

	// 아네모네 사전 우선 적용을 켰을 경우
	if (Cl.Config->GetForceAneDic()) ApplyForceAneDicAll(wNameR);
	if (Cl.Config->GetForceAneDic()) ApplyForceAneDicAll(wTextR);

	wNameT = eztrans_proc(wNameR);
	wTextT = eztrans_proc(wTextR);

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

void CTextProcess::ApplyForceAneDicAll(std::wstring &input)
{
	std::wstring str = input;
	
	// 개별사전 우선적용
	for (unsigned int i = 0; i<AneDic.size(); i++)
	{
		str = replaceAll(str, AneDic[i].wjpn, AneDic[i].wkor);
	}

	input = str;
}

void *CTextProcess::_PatchUDic(const wchar_t *dicFile)
{
	FILE *fp;
	//char buf[1024];
	int nLine = 0;
	int nLen = 0;

	// JKDIC 구조체 초기화
	//JKDIC.clear();

	if (_wfopen_s(&fp, dicFile, L"rb") != 0)
	{
		MessageBox(0, L"UserDict.jk 파일이 없습니다", 0, 0);
		return false;
	}
	//	jkdicStruct jS;
	/*
	fseek(fp, 0, SEEK_END);
	int fLength = ftell(fp);
	fseek(fp, 0, SEEK_SET);*/
	fseek(fp, 0L, SEEK_END);
	int filesize = ftell(fp);
	fseek(fp, 0, SEEK_SET);

	void *m_loc = malloc(filesize);
	fread(m_loc, sizeof(char), filesize, fp);

	/*
	while(1)
	{
	nLine++;
	if (fread(buf, sizeof(char), 1, fp) == 0) break;
	if (buf[0] == 0x00) jS.hidden = false;
	else jS.hidden = true;
	if (fread(buf, sizeof(char), 31, fp) == 0) break;
	memcpy(jS.jpword_sjis, buf, 31);
	if (fread(buf, sizeof(char), 31, fp) == 0) break;
	memcpy(jS.krword_euckr, buf, 31);
	if (fread(buf, sizeof(char), 5, fp) == 0) break;
	memcpy(jS.part_of_speech, buf, 5);
	if (fread(buf, sizeof(char), 42, fp) == 0) break;
	memcpy(jS.attributes, buf, 42);

	JKDIC.push_back(jS);
	}
	*/
	fclose(fp);


	if (_wfopen_s(&fp, dicFile, L"wb") != 0)
	{
		MessageBox(0, L"UserDict.jk 파일에 쓰기 권한이 없습니다", 0, 0);
		return false;
	}

	for (unsigned int i = 0; i<AneDic.size(); i++)
	{
		if (strstr(AneDic[i].jpn, "?") != 0) continue;

		fwrite(&AneDic[i].hidden, sizeof(char), 1, fp);
		fwrite(&AneDic[i].jpn, sizeof(char), 31, fp);
		fwrite(&AneDic[i].kor, sizeof(char), 31, fp);
		fwrite(&AneDic[i].part, sizeof(char), 5, fp);
		fwrite(&AneDic[i].attr, sizeof(char), 42, fp);
	}

	fwrite(m_loc, sizeof(char), filesize, fp);

	/*
	for (unsigned int i=0; i<JKDIC.size(); i++)
	{
	fwrite(&JKDIC[i].hidden, sizeof(char), 1, fp);
	fwrite(&JKDIC[i].jpword_sjis, sizeof(char), 31, fp);
	fwrite(&JKDIC[i].krword_euckr, sizeof(char), 31, fp);
	fwrite(&JKDIC[i].part_of_speech, sizeof(char), 5, fp);
	fwrite(&JKDIC[i].attributes, sizeof(char), 42, fp);
	}*/
	fclose(fp);

	//WriteLog(L"[DatUserDictRead] %d개의 DAT 사용자 사전 \"UserDict.jk\"를 읽었습니다.\n", UserDict.size());

	OLDFILEINFO *ret = new OLDFILEINFO;

	ret->filesize = filesize;
	ret->m_loc = m_loc;

	return ret;
}
bool CTextProcess::_UnPatchUDic(const wchar_t *dicFile, void *param)
{
	FILE *fp;
	OLDFILEINFO *offile = (OLDFILEINFO *)param;
	int nLine = 0;
	int nLen = 0;

	//DeleteFile(g_J2K_UserDic);

	if (_wfopen_s(&fp, dicFile, L"wb") != 0)
	{
		MessageBox(0, L"UserDict.jk 파일에 쓰기 권한이 없습니다", 0, 0);
		return false;
	}

	fwrite(offile->m_loc, sizeof(char), offile->filesize, fp);
	/*
	for (unsigned int i=0; i<JKDIC.size(); i++)
	{
	fwrite(&JKDIC[i].hidden, sizeof(char), 1, fp);
	fwrite(&JKDIC[i].jpword_sjis, sizeof(char), 31, fp);
	fwrite(&JKDIC[i].krword_euckr, sizeof(char), 31, fp);
	fwrite(&JKDIC[i].part_of_speech, sizeof(char), 5, fp);
	fwrite(&JKDIC[i].attributes, sizeof(char), 42, fp);
	}*/
	fclose(fp);

	free(offile->m_loc);
	delete offile;
	return true;
}

bool CTextProcess::_LoadDic(const wchar_t *dicFile)
{
	std::vector<aneDicStruct> NewAneDic;
	FILE *fp;
	int nLine;
	wchar_t wstr[1024];
	wchar_t wjpn[1024];
	wchar_t wkor[1024];
	wchar_t wpart[1024];
	wchar_t wattr[1024];

	char jpn[31];
	char kor[31];
	char attr[42];

	// 사전 파일 읽기

	if (_wfopen_s(&fp, dicFile, L"rt,ccs=UTF-8") != 0)
	{
		//MessageBox(0, L"사용자 사전을 열 수 없습니다", 0, 0);
		return false;
	}
	nLine = 0;

	// 한줄씩 읽기
	while (fgetws(wstr, 1000, fp) != NULL)
	{
		int nLength = wcslen(wstr);
		int nBom = 0;
		nLine++;

		// 주석 처리
		if (wstr[0] == L'/' && wstr[1] == L'/')
		{
			//fwprintf(out, L"[주석]\n");
			continue;
		}

		memset(wjpn, 0, 1024);
		memset(wkor, 0, 1024);
		memset(wpart, 0, 1024);
		memset(wattr, 0, 1024);


		int tab = 0;

		for (int i = 0, prev = 0; i <= nLength; i++)
		{
			/*
			// 탭을 여러개 넣었을 떄의 처리
			if (i > 0 && wstr[i - 1] == L'\t' && wstr[i] == L'\t')
			{
				prev++;
				continue;
			}*/

			// 탭을 만나거나 EOF를 만나면
			if (wstr[i] == L'\t' || wstr[i] == L'\n' || (wstr[i - 1] == L'/' && wstr[i] == L'/') || i == nLength)
			{
				switch (tab)
				{
				case 0: // jpn
					wcsncpy_s(wjpn, wstr, i);
					tab++;
					prev = i + 1;
					break;

				case 1: // kor
					wcsncpy_s(wkor, wstr + prev, i - prev);
					tab++;
					prev = i + 1;
					break;

				case 2: // part
					wcsncpy_s(wpart, wstr + prev, i - prev);
					tab++;
					prev = i + 1;
					break;

				case 3: // attr
					wcsncpy_s(wattr, wstr + prev, i - prev);
					tab++;
					prev = i + 1;
					break;
				}

				// 주석을 만나면 종료
				if (wstr[i - 1] == L'/' && wstr[i] == L'/') break;

			}
		}

		// 탭이 1개 이상 없으면 옳바르지 않은 형식
		if (tab < 2) continue;

		// 유효성 검사
		if (wjpn[0] == L'') continue;
		/*
		// 우선 적용 단어인 경우
		if (wpart[0] == L'2')
		{
			aneFDicStruct FDIC;
			FDIC.jpn = wjpn;
			FDIC.kor = wkor;
			AneFDic.push_back(FDIC);
			continue;
		}
		*/
		aneDicStruct DIC;

		memset(DIC.wjpn, 0, sizeof(wchar_t) * 31);
		memset(DIC.wkor, 0, sizeof(wchar_t) * 31);
		memset(DIC.jpn, 0, sizeof(char) * 31);
		memset(DIC.kor, 0, sizeof(char) * 31);
		memset(DIC.part, 0, sizeof(char) * 5);
		memset(DIC.attr, 0, sizeof(char) * 42);

		int n_jpword;
		int nLen;

		if ((n_jpword = WideCharToMultiByte(932, 0, wjpn, -1, NULL, NULL, NULL, NULL)) > 31)
		{
			//WriteLog(L"[UserDictRead] 오류 | 일어 단어의 길이는 15자 (30Byte)를 넘을 수 없습니다. 해당 단어는 무시됩니다. 현재 길이 : %dByte\n", n_jpword);
			//WriteLog(L"[UserDictRead] 오류 | [%s:%d] : %s | %s | %s | %s\n", FindFileData.cFileName, nLine, wjpword_sjis, wkrword_euckr, wpart_of_speech, wattributes);
			continue;
		}
		WideCharToMultiByte(932, 0, wjpn, -1, jpn, n_jpword, NULL, NULL);

		if ((nLen = WideCharToMultiByte(949, 0, wkor, -1, NULL, NULL, NULL, NULL)) > 31)
		{
			//WriteLog(L"[UserDictRead] 오류 | 한글 단어의 길이는 15자 (30Byte)를 넘을 수 없습니다. 해당 단어는 무시됩니다. 현재 길이 : %dByte\n", nLen);
			//WriteLog(L"[UserDictRead] 오류 | [%s:%d] : %s | %s | %s | %s\n", FindFileData.cFileName, nLine, wjpword_sjis, wkrword_euckr, wpart_of_speech, wattributes);
			continue;
		}
		WideCharToMultiByte(949, 0, wkor, -1, kor, nLen, NULL, NULL);

		int part = 0;

		//WideCharToMultiByte_REAL(932,0,wpart_of_speech,-1,part_of_speech,5,NULL,NULL);

		if ((nLen = WideCharToMultiByte(CP_ACP, 0, wattr, -1, NULL, NULL, NULL, NULL)) > 42)
		{
			//WriteLog(L"[UserDictRead] 오류 | 단어 속성은 42Byte를 넘을 수 없습니다. 해당 단어는 무시됩니다. 현재 길이 : %dByte\n", nLen);
			//WriteLog(L"[UserDictRead] 오류 | [%s:%d] : %s | %s | %s | %s\n", FindFileData.cFileName, nLine, wjpword_sjis, wkrword_euckr, wpart_of_speech, wattributes);
			continue;
		}
		WideCharToMultiByte(CP_ACP, 0, wattr, -1, attr, nLen, NULL, NULL);

		DIC.hidden = 0x00;
		strncpy_s(DIC.jpn, jpn, 31);
		strncpy_s(DIC.kor, kor, 31);
		//strncpy(jS.part_of_speech, part_of_speech, 5);
		if (wpart[0] == L'0')
			strncpy_s(DIC.part, "A9D0", 5); // 상용어구 (0)
		else strncpy_s(DIC.part, "I110", 5); // 명사 (1 또는 입력없음)
		strncpy_s(DIC.attr, attr, 42);

		wcscpy_s(DIC.wjpn, wjpn);
		wcscpy_s(DIC.wkor, wkor);
		NewAneDic.push_back(DIC);
	}
	AneDic.clear();
	AneDic = NewAneDic;
	fclose(fp);
	return true;
}

bool CTextProcess::LoadDictionary()
{
	std::wstring AneDic;
	std::wstring DicJK;

	GetLoadPath(AneDic, L"\\AneDic.txt");
	GetEZTPath(DicJK, L"\\Dat\\UserDict.jk");

	if (_LoadDic(AneDic.c_str()))
	{
		OLDFILEINFO *offile = (OLDFILEINFO *)_PatchUDic(DicJK.c_str());
		Cl.TransEngine->J2K_ReloadUserDict();
		_UnPatchUDic(DicJK.c_str(), offile);
	}
	return true;
}

CTextProcess::~CTextProcess()
{
	// 클립보드 감시 종료
	EndWatchClip();
}
