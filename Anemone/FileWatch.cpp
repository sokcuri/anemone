#include "stdafx.h"
#include "FileWatch.h"

#define cbBuffer 1024

CFileWatch *CFileWatch::m_pThis = NULL;

CFileWatch::CFileWatch()
{
	TurnOn();
}

void CFileWatch::TurnOn()
{
	SECURITY_ATTRIBUTES ThreadAttributes;
	ThreadAttributes.bInheritHandle = false;
	ThreadAttributes.lpSecurityDescriptor = NULL;
	ThreadAttributes.nLength = sizeof(SECURITY_ATTRIBUTES);

	hWatchThread = CreateThread(&ThreadAttributes, 0, FileChangeNotifyThread, NULL, 0, NULL);
	if (hWatchThread == NULL)
	{
		MessageBox(0, L"쓰레드 생성 작업을 실패했습니다.", 0, MB_ICONERROR);
	}
}

void CFileWatch::TurnOff()
{
	TerminateThread(hWatchThread, 0);
}

CFileWatch::~CFileWatch()
{
	TerminateThread(hWatchThread, 0);
}

DWORD CFileWatch::_FileChangeNotifyThread(LPVOID lpParam)
{
	HWND hwnd = (HWND)lpParam;

	std::wstring Path;
	GetLoadPath(Path);

	HANDLE hDir = CreateFile(Path.c_str(), GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE,
		0, OPEN_EXISTING, FILE_FLAG_BACKUP_SEMANTICS, 0);
	
	BYTE* pBuffer[cbBuffer];
	BOOL bWatchSubtree = FALSE;
	DWORD dwNotifyFilter = FILE_NOTIFY_CHANGE_FILE_NAME | FILE_NOTIFY_CHANGE_DIR_NAME |
		FILE_NOTIFY_CHANGE_ATTRIBUTES | FILE_NOTIFY_CHANGE_SIZE |
		FILE_NOTIFY_CHANGE_LAST_WRITE | FILE_NOTIFY_CHANGE_CREATION;
	DWORD bytesReturned;

	FILE_NOTIFY_INFORMATION* pfni;
	FILE_NOTIFY_INFORMATION* Prev_pfni;
	DWORD Prev_TickCount = -1;
	wchar_t Prev_FileName[260];

	wchar_t temp[MAX_PATH] = { 0 };
	for (;;)
	{
		BOOL fOk = ReadDirectoryChangesW(hDir, pBuffer, cbBuffer,
			bWatchSubtree, dwNotifyFilter, &bytesReturned, 0, 0);
		if (!fOk)
		{
			DWORD dwLastError = GetLastError();
			break;
		}

		pfni = (FILE_NOTIFY_INFORMATION*)pBuffer;


		do {
			memcpy(temp, pfni->FileName, pfni->FileNameLength);
			temp[pfni->FileNameLength / 2] = 0;

			_wcslwr_s(temp, pfni->FileNameLength);

			if (GetTickCount() - Prev_TickCount >= 1000 ||
				wcscmp(Prev_FileName, temp) != 0)
			{
				if (wcscmp(temp, L"anemone.ini") == 0)
				{
					Cl.Config->LoadConfig();
					PostMessage(hWnds.Main, WM_COMMAND, IDM_SETTING_CHECK, 0);
				}

				if (wcscmp(temp, L"anedic.txt") == 0)
				{
					MessageBox(0, L"anedic", 0, 0);
					//Cl.TextProcess->LoadDictionary();
				}

				if (wcscmp(temp, L"anedic2.txt") == 0)
				{
					MessageBox(0, L"anedic2", 0, 0);
				}

				if (wcscmp(temp, L"anedic3.txt") == 0)
				{
					MessageBox(0, L"anedic3", 0, 0);
				}
			}

			Prev_TickCount = GetTickCount();
			wcsncpy_s(Prev_FileName, temp, 255);

			Prev_pfni = pfni;
			pfni = (FILE_NOTIFY_INFORMATION*)((PBYTE)pfni + pfni->NextEntryOffset);
		} while (Prev_pfni->NextEntryOffset > 0);
	}
	return 0;
}