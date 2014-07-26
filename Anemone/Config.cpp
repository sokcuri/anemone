#include "stdafx.h"
#include "Config.h"


CConfig::CConfig()
{
	wcscpy_s(cfg_name_font, L"¸¼Àº °íµñ");
	wcscpy_s(cfg_org_font, L"¸¼Àº °íµñ");
	wcscpy_s(cfg_trans_font, L"¸¼Àº °íµñ");

	LoadConfig();
}


CConfig::~CConfig()
{
}

bool CConfig::LoadConfig()
{
	std::wstring INIPath;
	GetLoadPath(INIPath);
	INIPath += L"\\anemone.ini";

	wchar_t buf[255];

	ReadINI_Str(L"CLIPBOARD_SWITCH", L"CONFIG", buf, (wchar_t*)INIPath.c_str());
	if (buf[0] != NULL) (_wcsicmp(buf, L"OFF") != 0) ? SetClipSwitch(true) : SetClipSwitch(false);
	ReadINI_Str(L"CLIPBOARD_LENGTH", L"CONFIG", buf, (wchar_t*)INIPath.c_str());
	if (buf[0] != NULL) SetClipLength(_wtoi(buf));

	ReadINI_Str(L"WNDBORDER_SWITCH", L"CONFIG", buf, (wchar_t*)INIPath.c_str());
	if (buf[0] != NULL) (_wcsicmp(buf, L"OFF") != 0) ? SetWndBorderMode(true) : SetWndBorderMode(false);
	ReadINI_Str(L"MAGNETIC_MINIMIZE", L"CONFIG", buf, (wchar_t*)INIPath.c_str());
	if (buf[0] != NULL) (_wcsicmp(buf, L"OFF") != 0) ? SetMagneticMinimize(true) : SetMagneticMinimize(false);

	ReadINI_Str(L"HIDEWIN_NOWATCH_CLIP", L"CONFIG", buf, (wchar_t*)INIPath.c_str());
	if (buf[0] != NULL) (_wcsicmp(buf, L"OFF") != 0) ? SetHideWinUnWatchClip(true) : SetHideWinUnWatchClip(false);
	ReadINI_Str(L"HIDEWIN_UNLOCK_HOTKEY", L"CONFIG", buf, (wchar_t*)INIPath.c_str());
	if (buf[0] != NULL) (_wcsicmp(buf, L"OFF") != 0) ? SetHideWinUnlockHotkey(true) : SetHideWinUnlockHotkey(false);

	ReadINI_Str(L"BG_SWITCH", L"CONFIG", buf, (wchar_t*)INIPath.c_str());
	if (buf[0] != NULL) (_wcsicmp(buf, L"OFF") != 0) ? SetBGSwitch(true) : SetBGSwitch(false);
	ReadINI_Str(L"BG_COLOR", L"CONFIG", buf, (wchar_t*)INIPath.c_str());
	if (buf[0] != NULL) SetBGColor((DWORD)wcstoul(buf, NULL, 16));

	ReadINI_Str(L"TEXT_ALIGN", L"CONFIG", buf, (wchar_t*)INIPath.c_str());
	if (buf[0] != NULL) SetTextAlign((DWORD)wcstoul(buf, NULL, 16));

	ReadINI_Str(L"SHADOW_X", L"CONFIG", buf, (wchar_t*)INIPath.c_str());
	if (buf[0] != NULL) SetShadowX(_wtoi(buf));
	ReadINI_Str(L"SHADOW_Y", L"CONFIG", buf, (wchar_t*)INIPath.c_str());
	if (buf[0] != NULL) SetShadowY(_wtoi(buf));

	ReadINI_Str(L"TEXT_MARGIN_X", L"CONFIG", buf, (wchar_t*)INIPath.c_str());
	if (buf[0] != NULL) SetTextMarginX(_wtoi(buf));
	ReadINI_Str(L"TEXT_MARGIN_Y", L"CONFIG", buf, (wchar_t*)INIPath.c_str());
	if (buf[0] != NULL) SetTextMarginY(_wtoi(buf));
	ReadINI_Str(L"NAME_MARGIN_X", L"CONFIG", buf, (wchar_t*)INIPath.c_str());
	if (buf[0] != NULL) SetTextMarginName(_wtoi(buf));

	ReadINI_Str(L"NAME_SWITCH", L"CONFIG", buf, (wchar_t*)INIPath.c_str());
	if (buf[0] != NULL) (_wcsicmp(buf, L"OFF") != 0) ? SetTextSwitch(CFG_NAME, true) : SetTextSwitch(CFG_NAME, false);
	ReadINI_Str(L"NAME_ORG_SWITCH", L"CONFIG", buf, (wchar_t*)INIPath.c_str());
	if (buf[0] != NULL) (_wcsicmp(buf, L"OFF") != 0) ? SetTextSwitch(CFG_NAME_ORG, true) : SetTextSwitch(CFG_NAME_ORG, false);
	ReadINI_Str(L"NAME_SHADOW", L"CONFIG", buf, (wchar_t*)INIPath.c_str());
	if (buf[0] != NULL) (_wcsicmp(buf, L"OFF") != 0) ? SetTextShadow(CFG_NAME, true) : SetTextShadow(CFG_NAME, false);
	ReadINI_Str(L"NAME_FONT", L"CONFIG", buf, (wchar_t*)INIPath.c_str());
	if (buf[0] != NULL) SetTextFont(CFG_NAME, buf);
	ReadINI_Str(L"NAME_SIZE_A", L"CONFIG", buf, (wchar_t*)INIPath.c_str());
	if (buf[0] != NULL) SetTextSize(CFG_NAME, CFG_A, _wtoi(buf));
	ReadINI_Str(L"NAME_SIZE_B", L"CONFIG", buf, (wchar_t*)INIPath.c_str());
	if (buf[0] != NULL) SetTextSize(CFG_NAME, CFG_B, _wtoi(buf));
	ReadINI_Str(L"NAME_SIZE_C", L"CONFIG", buf, (wchar_t*)INIPath.c_str());
	if (buf[0] != NULL) SetTextSize(CFG_NAME, CFG_C, _wtoi(buf));
	ReadINI_Str(L"NAME_SIZE_S", L"CONFIG", buf, (wchar_t*)INIPath.c_str());
	if (buf[0] != NULL) SetTextSize(CFG_NAME, CFG_S, _wtoi(buf));
	ReadINI_Str(L"NAME_COLOR_A", L"CONFIG", buf, (wchar_t*)INIPath.c_str());
	if (buf[0] != NULL) SetTextColor(CFG_NAME, CFG_A, (DWORD)wcstoul(buf, NULL, 16));
	ReadINI_Str(L"NAME_COLOR_B", L"CONFIG", buf, (wchar_t*)INIPath.c_str());
	if (buf[0] != NULL) SetTextColor(CFG_NAME, CFG_B, (DWORD)wcstoul(buf, NULL, 16));
	ReadINI_Str(L"NAME_COLOR_C", L"CONFIG", buf, (wchar_t*)INIPath.c_str());
	if (buf[0] != NULL) SetTextColor(CFG_NAME, CFG_C, (DWORD)wcstoul(buf, NULL, 16));
	ReadINI_Str(L"NAME_COLOR_S", L"CONFIG", buf, (wchar_t*)INIPath.c_str());
	if (buf[0] != NULL) SetTextColor(CFG_NAME, CFG_S, (DWORD)wcstoul(buf, NULL, 16));

	ReadINI_Str(L"ORG_SWITCH", L"CONFIG", buf, (wchar_t*)INIPath.c_str());
	if (buf[0] != NULL) (_wcsicmp(buf, L"OFF") != 0) ? SetTextSwitch(CFG_ORG, true) : SetTextSwitch(CFG_ORG, false);
	ReadINI_Str(L"ORG_SHADOW", L"CONFIG", buf, (wchar_t*)INIPath.c_str());
	if (buf[0] != NULL) (_wcsicmp(buf, L"OFF") != 0) ? SetTextShadow(CFG_ORG, true) : SetTextShadow(CFG_ORG, false);
	ReadINI_Str(L"ORG_FONT", L"CONFIG", buf, (wchar_t*)INIPath.c_str());
	if (buf[0] != NULL) SetTextFont(CFG_ORG, buf);
	ReadINI_Str(L"ORG_SIZE_A", L"CONFIG", buf, (wchar_t*)INIPath.c_str());
	if (buf[0] != NULL) SetTextSize(CFG_ORG, CFG_A, _wtoi(buf));
	ReadINI_Str(L"ORG_SIZE_B", L"CONFIG", buf, (wchar_t*)INIPath.c_str());
	if (buf[0] != NULL) SetTextSize(CFG_ORG, CFG_B, _wtoi(buf));
	ReadINI_Str(L"ORG_SIZE_C", L"CONFIG", buf, (wchar_t*)INIPath.c_str());
	if (buf[0] != NULL) SetTextSize(CFG_ORG, CFG_C, _wtoi(buf));
	ReadINI_Str(L"ORG_SIZE_S", L"CONFIG", buf, (wchar_t*)INIPath.c_str());
	if (buf[0] != NULL) SetTextSize(CFG_ORG, CFG_S, _wtoi(buf));
	ReadINI_Str(L"ORG_COLOR_A", L"CONFIG", buf, (wchar_t*)INIPath.c_str());
	if (buf[0] != NULL) SetTextColor(CFG_ORG, CFG_A, (DWORD)wcstoul(buf, NULL, 16));
	ReadINI_Str(L"ORG_COLOR_B", L"CONFIG", buf, (wchar_t*)INIPath.c_str());
	if (buf[0] != NULL) SetTextColor(CFG_ORG, CFG_B, (DWORD)wcstoul(buf, NULL, 16));
	ReadINI_Str(L"ORG_COLOR_C", L"CONFIG", buf, (wchar_t*)INIPath.c_str());
	if (buf[0] != NULL) SetTextColor(CFG_ORG, CFG_C, (DWORD)wcstoul(buf, NULL, 16));
	ReadINI_Str(L"ORG_COLOR_S", L"CONFIG", buf, (wchar_t*)INIPath.c_str());
	if (buf[0] != NULL) SetTextColor(CFG_ORG, CFG_S, (DWORD)wcstoul(buf, NULL, 16));

	ReadINI_Str(L"TRANS_SWITCH", L"CONFIG", buf, (wchar_t*)INIPath.c_str());
	if (buf[0] != NULL) (_wcsicmp(buf, L"OFF") != 0) ? SetTextSwitch(CFG_TRANS, true) : SetTextSwitch(CFG_TRANS, false);
	ReadINI_Str(L"TRANS_SHADOW", L"CONFIG", buf, (wchar_t*)INIPath.c_str());
	if (buf[0] != NULL) (_wcsicmp(buf, L"OFF") != 0) ? SetTextShadow(CFG_TRANS, true) : SetTextShadow(CFG_TRANS, false);
	ReadINI_Str(L"TRANS_FONT", L"CONFIG", buf, (wchar_t*)INIPath.c_str());
	if (buf[0] != NULL) SetTextFont(CFG_TRANS, buf);
	ReadINI_Str(L"TRANS_SIZE_A", L"CONFIG", buf, (wchar_t*)INIPath.c_str());
	if (buf[0] != NULL) SetTextSize(CFG_TRANS, CFG_A, _wtoi(buf));
	ReadINI_Str(L"TRANS_SIZE_B", L"CONFIG", buf, (wchar_t*)INIPath.c_str());
	if (buf[0] != NULL) SetTextSize(CFG_TRANS, CFG_B, _wtoi(buf));
	ReadINI_Str(L"TRANS_SIZE_C", L"CONFIG", buf, (wchar_t*)INIPath.c_str());
	if (buf[0] != NULL) SetTextSize(CFG_TRANS, CFG_C, _wtoi(buf));
	ReadINI_Str(L"TRANS_SIZE_S", L"CONFIG", buf, (wchar_t*)INIPath.c_str());
	if (buf[0] != NULL) SetTextSize(CFG_TRANS, CFG_S, _wtoi(buf));
	ReadINI_Str(L"TRANS_COLOR_A", L"CONFIG", buf, (wchar_t*)INIPath.c_str());
	if (buf[0] != NULL) SetTextColor(CFG_TRANS, CFG_A, (DWORD)wcstoul(buf, NULL, 16));
	ReadINI_Str(L"TRANS_COLOR_B", L"CONFIG", buf, (wchar_t*)INIPath.c_str());
	if (buf[0] != NULL) SetTextColor(CFG_TRANS, CFG_B, (DWORD)wcstoul(buf, NULL, 16));
	ReadINI_Str(L"TRANS_COLOR_C", L"CONFIG", buf, (wchar_t*)INIPath.c_str());
	if (buf[0] != NULL) SetTextColor(CFG_TRANS, CFG_C, (DWORD)wcstoul(buf, NULL, 16));
	ReadINI_Str(L"TRANS_COLOR_S", L"CONFIG", buf, (wchar_t*)INIPath.c_str());
	if (buf[0] != NULL) SetTextColor(CFG_TRANS, CFG_S, (DWORD)wcstoul(buf, NULL, 16));

	ReadINI_Str(L"REPEAT_TEXT_FIX", L"CONFIG", buf, (wchar_t*)INIPath.c_str());
	if (buf[0] != NULL) SetRepeatTextProc((DWORD)wcstoul(buf, NULL, 16));
	ReadINI_Str(L"NAME_END_FIX", L"CONFIG", buf, (wchar_t*)INIPath.c_str());
	if (buf[0] != NULL) (_wcsicmp(buf, L"OFF") != 0) ? SetReviseName(true) : SetReviseName(false);

	ReadINI_Str(L"UPDATE_NOTIFY", L"CONFIG", buf, (wchar_t*)INIPath.c_str());
	if (buf[0] != NULL) (_wcsicmp(buf, L"OFF") != 0) ? SetUpdateNotify(true) : SetUpdateNotify(false);
	return true;
}

bool CConfig::SaveConfig()
{
	std::wstring INIPath;
	GetLoadPath(INIPath);
	INIPath += L"\\anemone.ini";

	wchar_t buf[255];
	wcscpy(buf, (GetClipSwitch() ? L"ON" : L"OFF"));
	WriteINI_Str(L"CLIPBOARD_SWITCH", L"CONFIG", buf, (wchar_t*)INIPath.c_str());
	wsprintf(buf, L"%d", GetClipLength());
	WriteINI_Str(L"CLIPBOARD_LENGTH", L"CONFIG", buf, (wchar_t*)INIPath.c_str());

	wcscpy(buf, (GetWndBorderMode() ? L"ON" : L"OFF"));
	WriteINI_Str(L"WNDBORDER_SWITCH", L"CONFIG", buf, (wchar_t*)INIPath.c_str());
	wcscpy(buf, (GetMagneticMinimize() ? L"ON" : L"OFF"));
	WriteINI_Str(L"MAGNETIC_MINIMIZE", L"CONFIG", buf, (wchar_t*)INIPath.c_str());

	wcscpy(buf, (GetHideWinUnWatchClip() ? L"ON" : L"OFF"));
	WriteINI_Str(L"HIDEWIN_NOWATCH_CLIP", L"CONFIG", buf, (wchar_t*)INIPath.c_str());
	wcscpy(buf, (GetHideWinUnlockHotkey() ? L"ON" : L"OFF"));
	WriteINI_Str(L"HIDEWIN_UNLOCK_HOTKEY", L"CONFIG", buf, (wchar_t*)INIPath.c_str());

	wcscpy(buf, (GetBGSwitch() ? L"ON" : L"OFF"));
	WriteINI_Str(L"BG_SWITCH", L"CONFIG", buf, (wchar_t*)INIPath.c_str());
	wsprintf(buf, L"%08X", GetBGColor());
	WriteINI_Str(L"BG_COLOR", L"CONFIG", buf, (wchar_t*)INIPath.c_str());

	wsprintf(buf, L"%d", GetTextAlign());
	WriteINI_Str(L"TEXT_ALIGN", L"CONFIG", buf, (wchar_t*)INIPath.c_str());

	wsprintf(buf, L"%d", GetShadowX());
	WriteINI_Str(L"SHADOW_X", L"CONFIG", buf, (wchar_t*)INIPath.c_str());
	wsprintf(buf, L"%d", GetShadowY());
	WriteINI_Str(L"SHADOW_Y", L"CONFIG", buf, (wchar_t*)INIPath.c_str());

	wsprintf(buf, L"%d", GetTextMarginX());
	WriteINI_Str(L"TEXT_MARGIN_X", L"CONFIG", buf, (wchar_t*)INIPath.c_str());
	wsprintf(buf, L"%d", GetTextMarginY());
	WriteINI_Str(L"TEXT_MARGIN_Y", L"CONFIG", buf, (wchar_t*)INIPath.c_str());
	wsprintf(buf, L"%d", GetTextMarginName());
	WriteINI_Str(L"NAME_MARGIN_X", L"CONFIG", buf, (wchar_t*)INIPath.c_str());

	wcscpy(buf, (GetTextSwitch(CFG_NAME) ? L"ON" : L"OFF"));
	WriteINI_Str(L"NAME_SWITCH", L"CONFIG", buf, (wchar_t*)INIPath.c_str());
	wcscpy(buf, (GetTextSwitch(CFG_NAME_ORG) ? L"ON" : L"OFF"));
	WriteINI_Str(L"NAME_ORG_SWITCH", L"CONFIG", buf, (wchar_t*)INIPath.c_str());
	wcscpy(buf, (GetTextShadow(CFG_NAME) ? L"ON" : L"OFF"));
	WriteINI_Str(L"NAME_SHADOW", L"CONFIG", buf, (wchar_t*)INIPath.c_str());
	wsprintf(buf, L"%s", GetTextFont(CFG_NAME));
	WriteINI_Str(L"NAME_FONT", L"CONFIG", buf, (wchar_t*)INIPath.c_str());
	wsprintf(buf, L"%d", GetTextSize(CFG_NAME, CFG_A));
	WriteINI_Str(L"NAME_SIZE_A", L"CONFIG", buf, (wchar_t*)INIPath.c_str());
	wsprintf(buf, L"%d", GetTextSize(CFG_NAME, CFG_B));
	WriteINI_Str(L"NAME_SIZE_B", L"CONFIG", buf, (wchar_t*)INIPath.c_str());
	wsprintf(buf, L"%d", GetTextSize(CFG_NAME, CFG_C));
	WriteINI_Str(L"NAME_SIZE_C", L"CONFIG", buf, (wchar_t*)INIPath.c_str());
	wsprintf(buf, L"%d", GetTextSize(CFG_NAME, CFG_S));
	WriteINI_Str(L"NAME_SIZE_S", L"CONFIG", buf, (wchar_t*)INIPath.c_str());
	wsprintf(buf, L"%08X", GetTextColor(CFG_NAME, CFG_A));
	WriteINI_Str(L"NAME_COLOR_A", L"CONFIG", buf, (wchar_t*)INIPath.c_str());
	wsprintf(buf, L"%08X", GetTextColor(CFG_NAME, CFG_B));
	WriteINI_Str(L"NAME_COLOR_B", L"CONFIG", buf, (wchar_t*)INIPath.c_str());
	wsprintf(buf, L"%08X", GetTextColor(CFG_NAME, CFG_C));
	WriteINI_Str(L"NAME_COLOR_C", L"CONFIG", buf, (wchar_t*)INIPath.c_str());
	wsprintf(buf, L"%08X", GetTextColor(CFG_NAME, CFG_S));
	WriteINI_Str(L"NAME_COLOR_S", L"CONFIG", buf, (wchar_t*)INIPath.c_str());

	wcscpy(buf, (GetTextSwitch(CFG_ORG) ? L"ON" : L"OFF"));
	WriteINI_Str(L"ORG_SWITCH", L"CONFIG", buf, (wchar_t*)INIPath.c_str());
	wcscpy(buf, (GetTextShadow(CFG_ORG) ? L"ON" : L"OFF"));
	WriteINI_Str(L"ORG_SHADOW", L"CONFIG", buf, (wchar_t*)INIPath.c_str());
	wsprintf(buf, L"%s", GetTextFont(CFG_ORG));
	WriteINI_Str(L"ORG_FONT", L"CONFIG", buf, (wchar_t*)INIPath.c_str());
	wsprintf(buf, L"%d", GetTextSize(CFG_ORG, CFG_A));
	WriteINI_Str(L"ORG_SIZE_A", L"CONFIG", buf, (wchar_t*)INIPath.c_str());
	wsprintf(buf, L"%d", GetTextSize(CFG_ORG, CFG_B));
	WriteINI_Str(L"ORG_SIZE_B", L"CONFIG", buf, (wchar_t*)INIPath.c_str());
	wsprintf(buf, L"%d", GetTextSize(CFG_ORG, CFG_C));
	WriteINI_Str(L"ORG_SIZE_C", L"CONFIG", buf, (wchar_t*)INIPath.c_str());
	wsprintf(buf, L"%d", GetTextSize(CFG_ORG, CFG_S));
	WriteINI_Str(L"ORG_SIZE_S", L"CONFIG", buf, (wchar_t*)INIPath.c_str());
	wsprintf(buf, L"%08X", GetTextColor(CFG_ORG, CFG_A));
	WriteINI_Str(L"ORG_COLOR_A", L"CONFIG", buf, (wchar_t*)INIPath.c_str());
	wsprintf(buf, L"%08X", GetTextColor(CFG_ORG, CFG_B));
	WriteINI_Str(L"ORG_COLOR_B", L"CONFIG", buf, (wchar_t*)INIPath.c_str());
	wsprintf(buf, L"%08X", GetTextColor(CFG_ORG, CFG_C));
	WriteINI_Str(L"ORG_COLOR_C", L"CONFIG", buf, (wchar_t*)INIPath.c_str());
	wsprintf(buf, L"%08X", GetTextColor(CFG_ORG, CFG_S));
	WriteINI_Str(L"ORG_COLOR_S", L"CONFIG", buf, (wchar_t*)INIPath.c_str());

	wcscpy(buf, (GetTextSwitch(CFG_TRANS) ? L"ON" : L"OFF"));
	WriteINI_Str(L"TRANS_SWITCH", L"CONFIG", buf, (wchar_t*)INIPath.c_str());
	wcscpy(buf, (GetTextShadow(CFG_TRANS) ? L"ON" : L"OFF"));
	WriteINI_Str(L"TRANS_SHADOW", L"CONFIG", buf, (wchar_t*)INIPath.c_str());
	wsprintf(buf, L"%s", GetTextFont(CFG_TRANS));
	WriteINI_Str(L"TRANS_FONT", L"CONFIG", buf, (wchar_t*)INIPath.c_str());
	wsprintf(buf, L"%d", GetTextSize(CFG_TRANS, CFG_A));
	WriteINI_Str(L"TRANS_SIZE_A", L"CONFIG", buf, (wchar_t*)INIPath.c_str());
	wsprintf(buf, L"%d", GetTextSize(CFG_TRANS, CFG_B));
	WriteINI_Str(L"TRANS_SIZE_B", L"CONFIG", buf, (wchar_t*)INIPath.c_str());
	wsprintf(buf, L"%d", GetTextSize(CFG_TRANS, CFG_C));
	WriteINI_Str(L"TRANS_SIZE_C", L"CONFIG", buf, (wchar_t*)INIPath.c_str());
	wsprintf(buf, L"%d", GetTextSize(CFG_TRANS, CFG_S));
	WriteINI_Str(L"TRANS_SIZE_S", L"CONFIG", buf, (wchar_t*)INIPath.c_str());
	wsprintf(buf, L"%08X", GetTextColor(CFG_TRANS, CFG_A));
	WriteINI_Str(L"TRANS_COLOR_A", L"CONFIG", buf, (wchar_t*)INIPath.c_str());
	wsprintf(buf, L"%08X", GetTextColor(CFG_TRANS, CFG_B));
	WriteINI_Str(L"TRANS_COLOR_B", L"CONFIG", buf, (wchar_t*)INIPath.c_str());
	wsprintf(buf, L"%08X", GetTextColor(CFG_TRANS, CFG_C));
	WriteINI_Str(L"TRANS_COLOR_C", L"CONFIG", buf, (wchar_t*)INIPath.c_str());
	wsprintf(buf, L"%08X", GetTextColor(CFG_TRANS, CFG_S));
	WriteINI_Str(L"TRANS_COLOR_S", L"CONFIG", buf, (wchar_t*)INIPath.c_str());

	wsprintf(buf, L"%d", GetRepeatTextProc());
	WriteINI_Str(L"REPEAT_TEXT_FIX", L"CONFIG", buf, (wchar_t*)INIPath.c_str());
	wcscpy(buf, (GetReviseName() ? L"ON" : L"OFF"));
	WriteINI_Str(L"NAME_END_FIX", L"CONFIG", buf, (wchar_t*)INIPath.c_str());

	wcscpy(buf, (GetUpdateNotify() ? L"ON" : L"OFF"));
	WriteINI_Str(L"UPDATE_NOTIFY", L"CONFIG", buf, (wchar_t*)INIPath.c_str());
	return true;
}
