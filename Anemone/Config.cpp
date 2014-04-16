#include "stdafx.h"
#include "Config.h"


CConfig::CConfig()
{
}


CConfig::~CConfig()
{
}

bool CConfig::LoadConfig()
{
	std::wstring INIPath;
	GetLoadPath(INIPath);
	INIPath += L"\\";
	INIPath += L"Anemone.ini";

	wchar_t buf[255];
	ReadINI_Str(L"BG_SWITCH", L"TEXT", buf, (wchar_t*)INIPath.c_str());
	if (buf[0] != NULL) (_wcsicmp(buf, L"OFF") != 0) ? cfg_bg_switch = true : cfg_bg_switch = false;

	ReadINI_Str(L"BG_COLOR", L"TEXT", buf, (wchar_t*)INIPath.c_str());
	if (buf[0] != NULL) cfg_bg_color = (DWORD)wcstod(buf, _T('\0'));

	// dword to tchar
	// _itow(dwSomeNum , wcsSomeStr, 10);
	
	return true;
}

bool CConfig::SaveConfig()
{
	return true;
}
