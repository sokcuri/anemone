#include "stdafx.h"
#include "Config.h"


CConfig::CConfig()
{
	wcscpy_s(cfg_name_font, L"¸¼Àº °íµñ");
	wcscpy_s(cfg_org_font, L"¸¼Àº °íµñ");
	wcscpy_s(cfg_trans_font, L"¸¼Àº °íµñ");
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
	if (buf[0] != NULL) (_wcsicmp(buf, L"OFF") != 0) ? SetBGSwitch(true) : SetBGSwitch(false);
	ReadINI_Str(L"BG_COLOR", L"TEXT", buf, (wchar_t*)INIPath.c_str());
	if (buf[0] != NULL) SetBGColor((DWORD)wcstod(buf, _T('\0')));

	ReadINI_Str(L"NAME_SWITCH", L"TEXT", buf, (wchar_t*)INIPath.c_str());
	if (buf[0] != NULL) (_wcsicmp(buf, L"OFF") != 0) ? SetTextSwitch(CFG_NAME, true) : SetTextSwitch(CFG_NAME, false);
	ReadINI_Str(L"NAME_ORG_SWITCH", L"TEXT", buf, (wchar_t*)INIPath.c_str());
	if (buf[0] != NULL) (_wcsicmp(buf, L"OFF") != 0) ? SetTextSwitch(CFG_NAME_ORG, true) : SetTextSwitch(CFG_NAME_ORG, false);
	ReadINI_Str(L"NAME_SHADOW", L"TEXT", buf, (wchar_t*)INIPath.c_str());
	if (buf[0] != NULL) (_wcsicmp(buf, L"OFF") != 0) ? SetTextShadow(CFG_NAME, true) : SetTextShadow(CFG_NAME, false);
	ReadINI_Str(L"NAME_FONT", L"TEXT", buf, (wchar_t*)INIPath.c_str());
	if (buf[0] != NULL) SetTextFont(CFG_NAME, buf);
	ReadINI_Str(L"NAME_SIZE_A", L"TEXT", buf, (wchar_t*)INIPath.c_str());
	if (buf[0] != NULL) SetTextSize(CFG_NAME, CFG_A, _wtoi(buf));
	ReadINI_Str(L"NAME_SIZE_B", L"TEXT", buf, (wchar_t*)INIPath.c_str());
	if (buf[0] != NULL) SetTextSize(CFG_NAME, CFG_B, _wtoi(buf));
	ReadINI_Str(L"NAME_SIZE_C", L"TEXT", buf, (wchar_t*)INIPath.c_str());
	if (buf[0] != NULL) SetTextSize(CFG_NAME, CFG_C, _wtoi(buf));
	ReadINI_Str(L"NAME_COLOR_A", L"TEXT", buf, (wchar_t*)INIPath.c_str());
	if (buf[0] != NULL) SetTextColor(CFG_NAME, CFG_A, (DWORD)wcstod(buf, _T('\0')));
	ReadINI_Str(L"NAME_COLOR_B", L"TEXT", buf, (wchar_t*)INIPath.c_str());
	if (buf[0] != NULL) SetTextColor(CFG_NAME, CFG_B, (DWORD)wcstod(buf, _T('\0')));
	ReadINI_Str(L"NAME_COLOR_C", L"TEXT", buf, (wchar_t*)INIPath.c_str());
	if (buf[0] != NULL) SetTextColor(CFG_NAME, CFG_C, (DWORD)wcstod(buf, _T('\0')));
	ReadINI_Str(L"NAME_COLOR_D", L"TEXT", buf, (wchar_t*)INIPath.c_str());
	if (buf[0] != NULL) SetTextColor(CFG_NAME, CFG_D, (DWORD)wcstod(buf, _T('\0')));

	ReadINI_Str(L"ORG_SWITCH", L"TEXT", buf, (wchar_t*)INIPath.c_str());
	if (buf[0] != NULL) (_wcsicmp(buf, L"OFF") != 0) ? SetTextSwitch(CFG_ORG, true) : SetTextSwitch(CFG_ORG, false);
	ReadINI_Str(L"ORG_SHADOW", L"TEXT", buf, (wchar_t*)INIPath.c_str());
	if (buf[0] != NULL) (_wcsicmp(buf, L"OFF") != 0) ? SetTextShadow(CFG_ORG, true) : SetTextShadow(CFG_ORG, false);
	ReadINI_Str(L"ORG_FONT", L"TEXT", buf, (wchar_t*)INIPath.c_str());
	if (buf[0] != NULL) SetTextFont(CFG_ORG, buf);
	ReadINI_Str(L"ORG_SIZE_A", L"TEXT", buf, (wchar_t*)INIPath.c_str());
	if (buf[0] != NULL) SetTextSize(CFG_ORG, CFG_A, _wtoi(buf));
	ReadINI_Str(L"ORG_SIZE_B", L"TEXT", buf, (wchar_t*)INIPath.c_str());
	if (buf[0] != NULL) SetTextSize(CFG_ORG, CFG_B, _wtoi(buf));
	ReadINI_Str(L"ORG_SIZE_C", L"TEXT", buf, (wchar_t*)INIPath.c_str());
	if (buf[0] != NULL) SetTextSize(CFG_ORG, CFG_C, _wtoi(buf));
	ReadINI_Str(L"ORG_COLOR_A", L"TEXT", buf, (wchar_t*)INIPath.c_str());
	if (buf[0] != NULL) SetTextColor(CFG_ORG, CFG_A, (DWORD)wcstod(buf, _T('\0')));
	ReadINI_Str(L"ORG_COLOR_B", L"TEXT", buf, (wchar_t*)INIPath.c_str());
	if (buf[0] != NULL) SetTextColor(CFG_ORG, CFG_B, (DWORD)wcstod(buf, _T('\0')));
	ReadINI_Str(L"ORG_COLOR_C", L"TEXT", buf, (wchar_t*)INIPath.c_str());
	if (buf[0] != NULL) SetTextColor(CFG_ORG, CFG_C, (DWORD)wcstod(buf, _T('\0')));
	ReadINI_Str(L"ORG_COLOR_D", L"TEXT", buf, (wchar_t*)INIPath.c_str());
	if (buf[0] != NULL) SetTextColor(CFG_ORG, CFG_D, (DWORD)wcstod(buf, _T('\0')));

	ReadINI_Str(L"TRANS_SWITCH", L"TEXT", buf, (wchar_t*)INIPath.c_str());
	if (buf[0] != NULL) (_wcsicmp(buf, L"OFF") != 0) ? SetTextSwitch(CFG_TRANS, true) : SetTextSwitch(CFG_TRANS, false);
	ReadINI_Str(L"TRANS_SHADOW", L"TEXT", buf, (wchar_t*)INIPath.c_str());
	if (buf[0] != NULL) (_wcsicmp(buf, L"OFF") != 0) ? SetTextShadow(CFG_TRANS, true) : SetTextShadow(CFG_TRANS, false);
	ReadINI_Str(L"TRANS_FONT", L"TEXT", buf, (wchar_t*)INIPath.c_str());
	if (buf[0] != NULL) SetTextFont(CFG_TRANS, buf);
	ReadINI_Str(L"TRANS_SIZE_A", L"TEXT", buf, (wchar_t*)INIPath.c_str());
	if (buf[0] != NULL) SetTextSize(CFG_TRANS, CFG_A, _wtoi(buf));
	ReadINI_Str(L"TRANS_SIZE_B", L"TEXT", buf, (wchar_t*)INIPath.c_str());
	if (buf[0] != NULL) SetTextSize(CFG_TRANS, CFG_B, _wtoi(buf));
	ReadINI_Str(L"TRANS_SIZE_C", L"TEXT", buf, (wchar_t*)INIPath.c_str());
	if (buf[0] != NULL) SetTextSize(CFG_TRANS, CFG_C, _wtoi(buf));
	ReadINI_Str(L"TRANS_COLOR_A", L"TEXT", buf, (wchar_t*)INIPath.c_str());
	if (buf[0] != NULL) SetTextColor(CFG_TRANS, CFG_A, (DWORD)wcstod(buf, _T('\0')));
	ReadINI_Str(L"TRANS_COLOR_B", L"TEXT", buf, (wchar_t*)INIPath.c_str());
	if (buf[0] != NULL) SetTextColor(CFG_TRANS, CFG_B, (DWORD)wcstod(buf, _T('\0')));
	ReadINI_Str(L"TRANS_COLOR_C", L"TEXT", buf, (wchar_t*)INIPath.c_str());
	if (buf[0] != NULL) SetTextColor(CFG_TRANS, CFG_C, (DWORD)wcstod(buf, _T('\0')));
	ReadINI_Str(L"TRANS_COLOR_D", L"TEXT", buf, (wchar_t*)INIPath.c_str());
	if (buf[0] != NULL) SetTextColor(CFG_TRANS, CFG_D, (DWORD)wcstod(buf, _T('\0')));
	return true;
}

bool CConfig::SaveConfig()
{
	return true;
}
