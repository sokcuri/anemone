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
	TurnOff();
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
	wchar_t Prev_FileName[MAX_PATH];

	wchar_t fileName[MAX_PATH] = { 0 };
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
			memcpy(fileName, pfni->FileName, pfni->FileNameLength);
			fileName[pfni->FileNameLength / 2] = 0;

			_wcslwr_s(fileName, pfni->FileNameLength);

			if (GetTickCount() - Prev_TickCount >= 50 ||
				wcscmp(Prev_FileName, fileName) != 0)
			{
				if (wcscmp(fileName, L"anemone.ini") == 0)
				{
					PostMessage(hWnds.Main, WM_COMMAND, ID_LOAD_CONFIG, 0);
				}

				if (wcscmp(fileName, L"anedic.txt") == 0)
				{
					PostMessage(hWnds.Main, WM_COMMAND, ID_LOAD_DICTIONARY, 0);
				}
			}

			Prev_TickCount = GetTickCount();
			wcsncpy_s(Prev_FileName, fileName, 255);

			Prev_pfni = pfni;
			pfni = (FILE_NOTIFY_INFORMATION*)((PBYTE)pfni + pfni->NextEntryOffset);
		} while (Prev_pfni->NextEntryOffset > 0);
	}
	return 0;
}