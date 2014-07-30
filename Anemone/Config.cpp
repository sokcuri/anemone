#include "stdafx.h"
#include "Config.h"


CConfig::CConfig()
{
	wcscpy_s(cfg_name_font, L"맑은 고딕");
	wcscpy_s(cfg_org_font, L"맑은 고딕");
	wcscpy_s(cfg_trans_font, L"맑은 고딕");
}


CConfig::~CConfig()
{
}

bool CConfig::LoadConfig()
{
	std::wstring INIPath;
	GetLoadPath(INIPath, L"\\anemone.ini");

	wchar_t buf[255];

	ReadINI_Str(L"CLIPBOARD_SWITCH", L"CONFIG", buf, (wchar_t*)INIPath.c_str());
	if (buf[0] != NULL) (_wcsicmp(buf, L"OFF") != 0) ? SetClipSwitch(true) : SetClipSwitch(false);
	ReadINI_Str(L"CLIPBOARD_LENGTH", L"CONFIG", buf, (wchar_t*)INIPath.c_str());
	if (buf[0] != NULL) SetClipLength(_wtoi(buf));

	ReadINI_Str(L"WNDBORDER_SWITCH", L"CONFIG", buf, (wchar_t*)INIPath.c_str());
	if (buf[0] != NULL) (_wcsicmp(buf, L"OFF") != 0) ? SetWndBorderMode(true) : SetWndBorderMode(false);
	ReadINI_Str(L"WNDBORDER_SIZE", L"CONFIG", buf, (wchar_t*)INIPath.c_str());
	if (buf[0] != NULL) SetWndBorderSize(_wtoi(buf));
	ReadINI_Str(L"WNDBORDER_COLOR", L"CONFIG", buf, (wchar_t*)INIPath.c_str());
	if (buf[0] != NULL) SetWndBorderColor((DWORD)wcstoul(buf, NULL, 16));

	ReadINI_Str(L"MAGNETIC_MINIMIZE", L"CONFIG", buf, (wchar_t*)INIPath.c_str());
	if (buf[0] != NULL) (_wcsicmp(buf, L"OFF") != 0) ? SetMagneticMinimize(true) : SetMagneticMinimize(false);

	ReadINI_Str(L"HOOKER_MONITOR_SWITCH", L"CONFIG", buf, (wchar_t*)INIPath.c_str());
	if (buf[0] != NULL) (_wcsicmp(buf, L"OFF") != 0) ? SetHookMonitor(true) : SetHookMonitor(false);
	ReadINI_Str(L"HOOKER_MONITOR_INTERVAL", L"CONFIG", buf, (wchar_t*)INIPath.c_str());
	if (buf[0] != NULL) SetHookInterval(_wtoi(buf));

	ReadINI_Str(L"HIDEWIN_NOWATCH_CLIP", L"CONFIG", buf, (wchar_t*)INIPath.c_str());
	if (buf[0] != NULL) (_wcsicmp(buf, L"OFF") != 0) ? SetHideWinUnWatchClip(true) : SetHideWinUnWatchClip(false);
	ReadINI_Str(L"HIDEWIN_UNLOCK_HOTKEY", L"CONFIG", buf, (wchar_t*)INIPath.c_str());
	if (buf[0] != NULL) (_wcsicmp(buf, L"OFF") != 0) ? SetHideWinUnlockHotkey(true) : SetHideWinUnlockHotkey(false);

	ReadINI_Str(L"PREV_SEARCH_NUMBER", L"CONFIG", buf, (wchar_t*)INIPath.c_str());
	if (buf[0] != NULL) (_wcsicmp(buf, L"OFF") != 0) ? SetPrevSearchNum(true) : SetPrevSearchNum(false);

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
	ReadINI_Str(L"NAME_FONT_STYLE", L"CONFIG", buf, (wchar_t*)INIPath.c_str());
	if (buf[0] != NULL) SetFontStyle(CFG_NAME, _wtoi(buf));
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
	ReadINI_Str(L"ORG_FONT_STYLE", L"CONFIG", buf, (wchar_t*)INIPath.c_str());
	if (buf[0] != NULL) SetFontStyle(CFG_ORG, _wtoi(buf));
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
	ReadINI_Str(L"TRANS_FONT_STYLE", L"CONFIG", buf, (wchar_t*)INIPath.c_str());
	if (buf[0] != NULL) SetFontStyle(CFG_TRANS, _wtoi(buf));
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
	ReadINI_Str(L"FILETRANS_OUTPUT_TYPE", L"CONFIG", buf, (wchar_t*)INIPath.c_str());
	if (buf[0] != NULL) SetFileTransOutput(_wtoi(buf));
	ReadINI_Str(L"ANEDIC_FORCEAPPLY", L"CONFIG", buf, (wchar_t*)INIPath.c_str());
	if (buf[0] != NULL) (_wcsicmp(buf, L"OFF") != 0) ? SetForceAneDic(true) : SetForceAneDic(false);
	ReadINI_Str(L"WINDOW_MOVE_POINT", L"CONFIG", buf, (wchar_t*)INIPath.c_str());
	if (buf[0] != NULL) SetWindowMovePoint(_wtoi(buf));
	return true;
}

bool CConfig::SaveConfig()
{
	Cl.FileWatch->TurnOff();

	std::wstring INIPath;
	GetLoadPath(INIPath, L"\\anemone.ini");

	wchar_t buf[255];
	wcscpy(buf, (GetClipSwitch() ? L"ON" : L"OFF"));
	WriteINI_Str(L"CLIPBOARD_SWITCH", L"CONFIG", buf, (wchar_t*)INIPath.c_str());
	wsprintf(buf, L"%d", GetClipLength());
	WriteINI_Str(L"CLIPBOARD_LENGTH", L"CONFIG", buf, (wchar_t*)INIPath.c_str());

	wcscpy(buf, (GetWndBorderMode() ? L"ON" : L"OFF"));
	WriteINI_Str(L"WNDBORDER_SWITCH", L"CONFIG", buf, (wchar_t*)INIPath.c_str());
	wsprintf(buf, L"%d", GetWndBorderSize());
	WriteINI_Str(L"WNDBORDER_SIZE", L"CONFIG", buf, (wchar_t*)INIPath.c_str());
	wsprintf(buf, L"%08X", GetWndBorderColor());
	WriteINI_Str(L"WNDBORDER_COLOR", L"CONFIG", buf, (wchar_t*)INIPath.c_str());
	
	wcscpy(buf, (GetMagneticMinimize() ? L"ON" : L"OFF"));
	WriteINI_Str(L"MAGNETIC_MINIMIZE", L"CONFIG", buf, (wchar_t*)INIPath.c_str());

	wcscpy(buf, (GetHookMonitor() ? L"ON" : L"OFF"));
	WriteINI_Str(L"HOOKER_MONITOR_SWITCH", L"CONFIG", buf, (wchar_t*)INIPath.c_str());
	wsprintf(buf, L"%d", GetHookInterval());
	WriteINI_Str(L"HOOKER_MONITOR_INTERVAL", L"CONFIG", buf, (wchar_t*)INIPath.c_str());

	wcscpy(buf, (GetHideWinUnWatchClip() ? L"ON" : L"OFF"));
	WriteINI_Str(L"HIDEWIN_NOWATCH_CLIP", L"CONFIG", buf, (wchar_t*)INIPath.c_str());
	wcscpy(buf, (GetHideWinUnlockHotkey() ? L"ON" : L"OFF"));
	WriteINI_Str(L"HIDEWIN_UNLOCK_HOTKEY", L"CONFIG", buf, (wchar_t*)INIPath.c_str());
	
	wcscpy(buf, (GetPrevSearchNum() ? L"ON" : L"OFF"));
	WriteINI_Str(L"PREV_SEARCH_NUMBER", L"CONFIG", buf, (wchar_t*)INIPath.c_str());

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
	wsprintf(buf, L"%d", GetFontStyle(CFG_NAME));
	WriteINI_Str(L"NAME_FONT_STYLE", L"CONFIG", buf, (wchar_t*)INIPath.c_str());
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
	wsprintf(buf, L"%d", GetFontStyle(CFG_ORG));
	WriteINI_Str(L"ORG_FONT_STYLE", L"CONFIG", buf, (wchar_t*)INIPath.c_str());
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
	wsprintf(buf, L"%d", GetFontStyle(CFG_TRANS));
	WriteINI_Str(L"TRANS_FONT_STYLE", L"CONFIG", buf, (wchar_t*)INIPath.c_str());
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
	wsprintf(buf, L"%d", GetFileTransOutput());
	WriteINI_Str(L"FILETRANS_OUTPUT_TYPE", L"CONFIG", buf, (wchar_t*)INIPath.c_str());
	wcscpy(buf, (GetForceAneDic() ? L"ON" : L"OFF"));
	WriteINI_Str(L"ANEDIC_FORCEAPPLY", L"CONFIG", buf, (wchar_t*)INIPath.c_str());
	wsprintf(buf, L"%d", GetWindowMovePoint());
	WriteINI_Str(L"WINDOW_MOVE_POINT", L"CONFIG", buf, (wchar_t*)INIPath.c_str());
	
	Cl.FileWatch->TurnOn();
	return true;
}

bool CConfig::LoadWndConfig()
{
	FILE *fp;
	wchar_t buff[1024];
	std::wstring WndConfig;
	int nLine = 0;

	GetLoadPath(WndConfig, L"\\WndInfo.ini");

	if (_wfopen_s(&fp, WndConfig.c_str(), L"rt,ccs=UTF-8") != 0)
	{
		//MessageBox(0, L"사용자 사전을 열 수 없습니다", 0, 0);
		return false;
	}

	// 정규식 패턴
	// ^\\[([0-9]+)x([0-9]+)\\]
	// ([A-Z]+)\=([0-9]+)\|([0-9]+)\|([0-9]+)\|([0-9]+)

	std::wregex regex_sect(L"^\\[([0-9]+)x([0-9]+)\\]");
	std::wregex regex_field(L"([A-Z]+)\=([0-9]+)\|([0-9]+)\|([0-9]+)\|([0-9]+)");

	std::wsmatch m;

	/*
	<< res_x res_y type x y cx cy
	>>

	int res_x
	int res_y
	int type

	int x
	int y
	int cx
	int cy
	*/

	int res_x;
	int res_y;

	// 한줄씩 읽기
	while (fgetws(buff, 1000, fp) != NULL)
	{
		std::wstring str = buff;
		if (std::regex_match(str, m, regex_sect))
		{
			res_x = _wtoi(m[0].str().c_str());
			res_y = _wtoi(m[1].str().c_str());
			//GetWndItem(arr, res_x, res_y, type, x, y, cx, cy);
		}
		else if (std::regex_match(str, m, regex_field))
		{
			if (res_x != 0 && res_y != 0)
			{
				if (m[0] == L"MAIN") type = 0;
				else if (m[1] == L"SETTING") type = 1;
				x = _wtoi(m[1].str().c_str());
				y = _wtoi(m[2].str().c_str());
				cx = _wtoi(m[3].str().c_str());
				cy = _wtoi(m[4].str().c_str());
			}
		}
	}
}

bool CConfig::AddWndRes()
{
	for (int i = 0; i < wndinfo.size(); i++)
	{

	}
}

bool CConfig::SaveWndConfig()
{
	FILE *fp;
	wchar_t buff[1024];
	std::wstring WndConfig;
	int nLine = 0;

	GetLoadPath(WndConfig, L"\\WndInfo.ini");

	if (_wfopen_s(&fp, WndConfig.c_str(), L"wt,ccs=UTF-8") != 0)
	{
		//MessageBox(0, L"사용자 사전을 열 수 없습니다", 0, 0);
		return false;
	}

	// 정규식 패턴
	// ^\\[([0-9]+)x([0-9]+)\\]
	// ([A-Z]+)\=([0-9]+)\|([0-9]+)\|([0-9]+)\|([0-9]+)

	std::wregex regex_sect(L"^\\[([0-9]+)x([0-9]+)\\]");
	std::wregex regex_field(L"([A-Z]+)\=([0-9]+)\|([0-9]+)\|([0-9]+)\|([0-9]+)");

	std::wsmatch m;

	/*
	<< res_x res_y type x y cx cy
	>>

	int res_x
	int res_y
	int type

	int x
	int y
	int cx
	int cy
	*/

	int res_x;
	int res_y;
	std::wstring type;

	for (int i = 0; i < wndinfo.size(); i++)
	{
		if (res_x != wndinfo[i].res_x && res_y != wndinfo[i].res_y)
		{
			res_x = wndinfo[0].res_x;
			res_y = wndinfo[0].res_y
			fwprintf(fp, L"[%dx%d]\r\n", wndinfo[i].res_x, wndinfo[i].res_y)
		}
		
		switch (wndinfo[i].type)
		{
		case 0:
			type = L"MAIN";
			break;
		case 1:
			type = L"SETTING";
			break;
		}
		fwprintf(fp, L"%s = %d|%d|%d|%d\r\n", type.c_str(), wndinfo[i].x, wndinfo[i].y, wndinfo[i].cx, wndinfo[i].cy);

	}
}