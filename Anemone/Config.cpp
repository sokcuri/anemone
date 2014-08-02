#include "stdafx.h"
#include "Config.h"


CConfig::CConfig()
{
	wcscpy_s(cfg_name_font, L"맑은 고딕");
	wcscpy_s(cfg_org_font, L"맑은 고딕");
	wcscpy_s(cfg_trans_font, L"맑은 고딕");
	cfg_eztrans_path[0] = 0x00;
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
	ReadINI_Str(L"HOOKER_TEXT_SIGNCUT", L"CONFIG", buf, (wchar_t*)INIPath.c_str());
	if (buf[0] != NULL) (_wcsicmp(buf, L"OFF") != 0) ? SetHookTextSignCut(true) : SetHookTextSignCut(false);

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
	ReadINI_Str(L"EZTRANS_PATH", L"CONFIG", buf, (wchar_t*)INIPath.c_str());
	if (buf[0] != NULL) SetEzTransPath(buf);
	return true;
}

bool CConfig::SaveConfig()
{
	Cl.FileWatch->TurnOff();

	std::wstring INIPath;
	GetLoadPath(INIPath, L"\\anemone.ini");

	// 설정파일이 존재하지 않는 경우 UNICODE로 파일 생성
	FILE *fp;
	if (_wfopen_s(&fp, INIPath.c_str(), L"rt,ccs=UNICODE") != 0)
	{
		if (_wfopen_s(&fp, INIPath.c_str(), L"wt,ccs=UNICODE") == 0)
		{
			fwrite(L"[CONFIG]", sizeof(wchar_t), 8, fp);
			fclose(fp);
		}
	}
	fclose(fp);

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
	wcscpy(buf, (GetHookTextSignCut() ? L"ON" : L"OFF"));
	WriteINI_Str(L"HOOKER_TEXT_SIGNCUT", L"CONFIG", buf, (wchar_t*)INIPath.c_str());

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
	wsprintf(buf, L"%s", GetEzTransPath());
	WriteINI_Str(L"EZTRANS_PATH", L"CONFIG", buf, (wchar_t*)INIPath.c_str());
	
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

	if (_wfopen_s(&fp, WndConfig.c_str(), L"rt,ccs=UNICODE") != 0)
	{
		//MessageBox(0, L"사용자 사전을 열 수 없습니다", 0, 0);
		return false;
	}

	// 정규식 패턴
	// ^\\[([0-9]+)x([0-9]+)\\]
	// ([A-Z]+)\=([0-9]+)\|([0-9]+)\|([0-9]+)\|([0-9]+)

	//std::wregex regex_sect(L"^\\[([0-9]+)x([0-9]+)\\]");
	
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
	std::wregex regex_field(L"^(\\d+)x(\\d+)=(\\d+)[|](\\d+)[|](\\d+)[|](\\d+)(\\s+)");

	std::wsmatch m;
	std::wstring str;
	// 한줄씩 읽기
	while (fgetws(buff, 1000, fp) != NULL)
	{
		str = buff;
		
		if (std::regex_match(str, m, regex_field))
		{
			_wndinfo wi;
			wi.res_x = _wtoi(m[1].str().c_str());
			wi.res_y = _wtoi(m[2].str().c_str());
			wi.x = _wtoi(m[3].str().c_str());
			wi.y = _wtoi(m[4].str().c_str());
			wi.cx = _wtoi(m[5].str().c_str());
			wi.cy = _wtoi(m[6].str().c_str());

			WndInfo.push_back(wi);
		}
	}

	fclose(fp);
	return true;
}

bool CConfig::SetWndRes(_wndinfo &wi)
{
	for (DWORD i = 0; i < WndInfo.size(); i++)
	{
		if (WndInfo[i].res_x == wi.res_x && WndInfo[i].res_y == wi.res_y)
		{
			WndInfo[i].x = wi.x;
			WndInfo[i].y = wi.y;
			WndInfo[i].cx = wi.cx;
			WndInfo[i].cy = wi.cy;
			return true;
		}
	}

	WndInfo.push_back(wi);
	return true;
}

bool CConfig::GetWndRes(_wndinfo &wi)
{
	for (DWORD i = 0; i < WndInfo.size(); i++)
	{
		if (WndInfo[i].res_x == wi.res_x && WndInfo[i].res_y == wi.res_y)
		{
			wi.x = WndInfo[i].x;
			wi.y = WndInfo[i].y;
			wi.cx = WndInfo[i].cx;
			wi.cy = WndInfo[i].cy;
			
			return true;
		}
	}
	return false;
}

bool CConfig::SaveWndConfig()
{
	FILE *fp;
	std::wstring WndConfig;
	int nLine = 0;

	GetLoadPath(WndConfig, L"\\WndInfo.ini");

	if (_wfopen_s(&fp, WndConfig.c_str(), L"wt,ccs=UNICODE") != 0)
	{
		MessageBox(0, L"열 수 없음", 0, 0);
		//MessageBox(0, L"사용자 사전을 열 수 없습니다", 0, 0);
		return false;
	}

	//fwprintf(fp, L"");

	for (DWORD i = 0; i < WndInfo.size(); i++)
	{
		fwprintf(fp, L"%dx%d=%d|%d|%d|%d\r\n", WndInfo[i].res_x, WndInfo[i].res_y, WndInfo[i].x, WndInfo[i].y, WndInfo[i].cx, WndInfo[i].cy);
	}

	fclose(fp);
	return true;
}