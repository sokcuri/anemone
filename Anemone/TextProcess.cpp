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

bool CTextProcess::OnDrawClipboard()
{
	char *szBuff, *szBuff2;
	wchar_t *lpszBuff;
	int nBufLen;

	std::wstring szClipText;
	std::wstring szNameOrg, szNameTrans;
	std::wstring szContextOrg, szContextTrans;

	OpenClipboard(hWnds.Clip);
	HANDLE hClipData = GetClipboardData(CF_UNICODETEXT);
	
	// 클립보드 얻어오기 실패 (또는 맞지 않는 규격)
	if (hClipData == NULL)
	{
		Cl.TextRenderer->Paint(); 
		CloseClipboard();
		ChangeClipboardChain(hWnds.Main, NULL);
		hWnds.Clip = SetClipboardViewer(hWnds.Main);

		MessageBox(0, L"클립보드 얻어오기 실패", 0, 0);
		return false;
	}

	szClipText = (wchar_t*)GlobalLock(hClipData);
	GlobalUnlock(hClipData);

	szContextOrg = szClipText;
	nBufLen = WideCharToMultiByte(932, 0, szContextOrg.c_str(), -1, NULL, NULL, NULL, NULL);
	szBuff = (char *)malloc((nBufLen+2)*2);

	// 메모리 할당 실패
	if (szBuff == NULL)
	{
		MessageBox(0, L"메모리 할당 실패", 0, 0);
		return false;
	}

	WideCharToMultiByte(932, 0, szContextOrg.c_str(), -1, szBuff, nBufLen, NULL, NULL);

	szBuff2 = (char *)Cl.TransEngine->J2K_TranslateMMNT(0, szBuff);
	free(szBuff);

	nBufLen = MultiByteToWideChar(949, 0, szBuff2, -1, NULL, NULL);
	lpszBuff = (wchar_t *)malloc((nBufLen+2)*2);

	// 메모리 할당 실패
	if (lpszBuff == NULL)
	{
		MessageBox(0, L"메모리 할당 실패", 0, 0);
		return false;
	}

	MultiByteToWideChar(949, 0, szBuff2, -1, lpszBuff, nBufLen);

	szContextTrans = lpszBuff;
	free(lpszBuff);
	Cl.TransEngine->J2K_FreeMem(szBuff2);

	*(Cl.TextRenderer->szContextOrg) = szContextOrg;
	*(Cl.TextRenderer->szContextTrans) = szContextTrans;
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
