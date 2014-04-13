// 자주 쓰는 함수를 넣어두는 라이브러리 파일
#include "stdafx.h"
#include "Library.h"

void GetLoadPath(std::wstring &dir)
{
	wchar_t p[255];
	GetModuleFileNameW(hInst, p, 255);
	std::wstring f = p;
	dir = f.substr(0, f.rfind(L'\\'));
}

// 레지스트리에서 이지트랜스 경로를 얻어옵니다
bool GetEZTPathFromREG(std::wstring &dir)
{
	wchar_t szPath[255];
	HKEY key;
	DWORD dwType = REG_SZ;
	DWORD dwSize = 255;

	RegOpenKeyEx(HKEY_CURRENT_USER, L"Software\\ChangShin\\ezTrans", 0, KEY_READ, &key);

	if (RegQueryValueEx(key, L"FilePath", 0, &dwType, (LPBYTE)szPath, &dwSize) != ERROR_SUCCESS) return false;

	dir = szPath;
	return true;
}

bool ReadINI_Str(const wchar_t *key, const wchar_t *section, wchar_t *buf, wchar_t *file)
{
	int n = GetPrivateProfileString(section, key, NULL, buf, 255, file);
	if (n == 0) {
		//WriteLog(L"설정값 불러오기 | 정의되지 않은 인수 | %ls\n", key);
		return false;
	}
	//WriteLog(L"%ls = \"%ls\"\n", key, buf);
	return true;
}


bool WriteINI_Str(const wchar_t *key, const wchar_t *section, wchar_t *buf, wchar_t *file)
{
	int n = WritePrivateProfileString(section, key, buf, file);
	if (n == 0) {
		//WriteLog(L"설정값 불러오기 | 정의되지 않은 인수 | %ls\n", key);
		return false;
	}
	//WriteLog(L"%ls = \"%ls\"\n", key, buf);
	return true;
}
