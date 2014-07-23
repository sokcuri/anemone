#pragma once
#define CFG_NAME 0
#define CFG_ORG 1
#define CFG_TRANS 2
#define CFG_NAME_ORG 3

#define CFG_A 0
#define CFG_B 1
#define CFG_C 2
#define CFG_S 3
#define CFG_ALL 4

class CConfig
{
private:
	bool cfg_bg_switch          = true;
	DWORD cfg_bg_color          = 0x130091FB;

	int cfg_shadow_x            = 10;
	int cfg_shadow_y            = 10;

	bool cfg_name_switch        = true;
	bool cfg_name_revise        = true;
	bool cfg_name_org_switch    = true;
	bool cfg_name_shadow        = true;
	wchar_t cfg_name_font[255];
	DWORD cfg_name_color_a      = 0xFFFFFFFF;
	DWORD cfg_name_color_b      = 0xFFFF5E00;
	DWORD cfg_name_color_c      = 0xFFFFA850;
	DWORD cfg_name_color_s      = 0x32000000;
	int cfg_name_size_a         = 25;
	int cfg_name_size_b         = 3;
	int cfg_name_size_c         = 3;

	bool cfg_org_switch         = true;
	bool cfg_org_shadow         = true;
	wchar_t cfg_org_font[255];
	DWORD cfg_org_color_a       = 0xFFFFFFFF;
	DWORD cfg_org_color_b       = 0xFFFF0000;
	DWORD cfg_org_color_c       = 0xFFFBAAAA;
	DWORD cfg_org_color_s       = 0x32000000;
	int cfg_org_size_a          = 25;
	int cfg_org_size_b          = 3;
	int cfg_org_size_c          = 3;

	bool cfg_trans_switch       = true;
	bool cfg_trans_shadow       = true;
	wchar_t cfg_trans_font[255];
	DWORD cfg_trans_color_a     = 0xFFFFFFFF;
	DWORD cfg_trans_color_b     = 0xFF4374D9;
	DWORD cfg_trans_color_c     = 0xFF5CD1E5;
	DWORD cfg_trans_color_s     = 0x32000000;
	int cfg_trans_size_a        = 25;
	int cfg_trans_size_b        = 3;
	int cfg_trans_size_c        = 3;

	bool cfg_temp_click_though = false;
	bool cfg_temp_sizable_mode = false;
	bool cfg_clipboard_watch = true;
	int cfg_window_hide = 0;
	bool cfg_window_topmost = true;
	bool cfg_window_magnetic_topmost = false;
	bool cfg_hidewin_unwatch_clipboard = true;
	bool cfg_hidewin_unlock_hotkey = true;
	bool cfg_magnetic_mode = false;
	int cfg_repeat_text = 1;
	bool cfg_force_anedic = false;
	bool cfg_ane_remocon = false;
	bool cfg_extern_hotkey = true;
	int cfg_text_align = 0;
	int cfg_text_margin_x = 0;
	int cfg_text_margin_y = 0;
	int cfg_text_margin_name = 0;
	bool cfg_trans_onego = true;

public:
	CConfig();
	~CConfig();

	bool LoadConfig();
	bool SaveConfig();

	int GetReviseName() { return cfg_name_revise; }
	void SetReviseName(bool b) { cfg_name_revise = b; }

	int GetTransOneGo() { return cfg_trans_onego; }
	void SetTransOneGo(bool b) { cfg_trans_onego = b; }

	int GetTextMarginX() { return cfg_text_margin_x; }
	void SetTextMarginX(int i) { cfg_text_margin_x = i; }

	int GetTextMarginY() { return cfg_text_margin_y; }
	void SetTextMarginY(int i) { cfg_text_margin_y = i; }

	int GetTextMarginName() { return cfg_text_margin_name; }
	void SetTextMarginName(int i) { cfg_text_margin_name = i; }

	int GetTextAlign() { return cfg_text_align; }
	void SetTextAlign(int i) { cfg_text_align = i; }

	bool GetExternHotkey() { return cfg_extern_hotkey; }
	void SetExternHotkey(bool b) { cfg_extern_hotkey = b; }

	bool GetRemoconMode() { return cfg_ane_remocon; }
	void SetRemoconMode(bool b) { cfg_ane_remocon = b; }

	bool GetForceAneDic() { return cfg_force_anedic; }
	void SetForceAneDic(bool b) { cfg_force_anedic = b; }

	int GetRepeatTextProc() { return cfg_repeat_text; }
	void SetRepeatTextProc(int i) { cfg_repeat_text = i; }

	bool GetMagneticMode() { return cfg_magnetic_mode; }
	void SetMagneticMode(bool b) { cfg_magnetic_mode = b; }

	bool GetHideWinUnlockHotkey() { return cfg_hidewin_unlock_hotkey; }
	void SetHideWinUnlockHotkey(bool b) { cfg_hidewin_unlock_hotkey = b; }

	bool GetHideWinUnWatchClip() { return cfg_hidewin_unwatch_clipboard; }
	void SetHideWinUnWatchClip(bool b) { cfg_hidewin_unwatch_clipboard = b; }

	bool GetMagneticTopMost() { return cfg_window_magnetic_topmost; }
	void SetMagneticTopMost(bool b) { cfg_window_magnetic_topmost = b; }

	bool GetWindowTopMost() { return cfg_window_topmost; }
	void SetWindowTopMost(bool b) { cfg_window_topmost = b; }

	bool GetWindowVisible() { return (cfg_window_hide == 0 ? true : false); }
	void SetWindowVisible(bool b) { (b ? cfg_window_hide = 0 : cfg_window_hide = 1); }

	bool GetTempWinHide() { return (cfg_window_hide == 2 ? true : false); }
	void SetTempWinHide(bool b) { (b ? cfg_window_hide = 2 : cfg_window_hide = 0); }

	bool GetClipboardWatch() { return cfg_clipboard_watch; }
	void SetClipboardWatch(bool b) { cfg_clipboard_watch = b; }

	bool GetClickThough() { return cfg_temp_click_though; }
	void SetClickThough(bool b) { cfg_temp_click_though = b; }

	bool GetSizableMode() { return cfg_temp_sizable_mode; }
	void SetSizableMode(bool b) { cfg_temp_sizable_mode = b; }

	bool GetBGSwitch() { return cfg_bg_switch; }
	DWORD GetBGColor() { return cfg_bg_color; }

	void SetBGSwitch(bool var) { cfg_bg_switch = var; }
	void SetBGColor(DWORD dw) { cfg_bg_color = dw; }

	bool GetTextSwitch(int type)
	{
		switch (type)
		{
		case CFG_NAME:
			return cfg_name_switch;
			break;
		case CFG_ORG:
			return cfg_org_switch;
			break;
		case CFG_TRANS:
			return cfg_trans_switch;
			break;
		case CFG_NAME_ORG:
			return cfg_name_org_switch;
			break;
		}
		return 0;
	}

	void SetTextSwitch(int type, bool b)
	{
		switch (type)
		{
		case CFG_NAME:
			cfg_name_switch = b;
			break;
		case CFG_ORG:
			cfg_org_switch = b;
			break;
		case CFG_TRANS:
			cfg_trans_switch = b;
			break;
		case CFG_NAME_ORG:
			cfg_name_org_switch = b;
			break;
		}
	}

	wchar_t *GetTextFont(int type)
	{
		switch (type)
		{
		case CFG_NAME:
			return cfg_name_font;
			break;
		case CFG_ORG:
			return cfg_org_font;
			break;
		case CFG_TRANS:
			return cfg_trans_font;
			break;
		}
		return 0;
	}

	void SetTextFont(int type, wchar_t *w)
	{
		switch (type)
		{
		case CFG_NAME:
			wcscpy_s(cfg_name_font, w);
			break;
		case CFG_ORG:
			wcscpy_s(cfg_org_font, w);
			break;
		case CFG_TRANS:
			wcscpy_s(cfg_trans_font, w);
			break;
		}
	}

	DWORD GetShadowX() { return cfg_shadow_x; }
	void SetShadowX(int c) { cfg_shadow_x = c; }

	DWORD GetShadowY() { return cfg_shadow_y; }
	void SetShadowY(int c) { cfg_shadow_y = c; }

	DWORD GetTextColor(int type, int n)
	{
		switch (type)
		{
		case CFG_NAME:
			{
				if (n == CFG_A) return cfg_name_color_a;
				else if (n == CFG_B) return cfg_name_color_b;
				else if (n == CFG_C) return cfg_name_color_c;
				else if (n == CFG_S) return cfg_name_color_s;
			}
			break;
		case CFG_ORG:
			{
				if (n == CFG_A) return cfg_org_color_a;
				else if (n == CFG_B) return cfg_org_color_b;
				else if (n == CFG_C) return cfg_org_color_c;
				else if (n == CFG_S) return cfg_org_color_s;
			}
			break;
		case CFG_TRANS:
			{
				if (n == CFG_A) return cfg_trans_color_a;
				else if (n == CFG_B) return cfg_trans_color_b;
				else if (n == CFG_C) return cfg_trans_color_c;
				else if (n == CFG_S) return cfg_trans_color_s;
			}
			break;
		}
		return 0;
	}
	
	void SetTextColor(int type, int n, int c)
	{
		switch (type)
		{
		case CFG_NAME:
			{
				if (n == CFG_A) cfg_name_color_a = c;
				else if (n == CFG_B) cfg_name_color_b = c;
				else if (n == CFG_C) cfg_name_color_c = c;
				else if (n == CFG_S) cfg_name_color_s = c;
			}
			break;
		case CFG_ORG:
			{
				if (n == CFG_A) cfg_org_color_a = c;
				else if (n == CFG_B) cfg_org_color_b = c;
				else if (n == CFG_C) cfg_org_color_c = c;
				else if (n == CFG_S) cfg_org_color_s = c;
			}
			break;
		case CFG_TRANS:
			{
				if (n == CFG_A) cfg_trans_color_a = c;
				else if (n == CFG_B) cfg_trans_color_b = c;
				else if (n == CFG_C) cfg_trans_color_c = c;
				else if (n == CFG_S) cfg_trans_color_s = c;
			}
			break;
		}
	}
	int GetTextSize(int type, int n)
	{
		switch (type)
		{
		case CFG_NAME:
			{
				if (n == CFG_A) return cfg_name_size_a;
				else if (n == CFG_B) return cfg_name_size_b;
				else if (n == CFG_C) return cfg_name_size_c;
			}
			break;
		case CFG_ORG:
			{
				if (n == CFG_A) return cfg_org_size_a;
				else if (n == CFG_B) return cfg_org_size_b;
				else if (n == CFG_C) return cfg_org_size_c;
			}
			break;
		case CFG_TRANS:
			{
				if (n == CFG_A) return cfg_trans_size_a;
				else if (n == CFG_B) return cfg_trans_size_b;
				else if (n == CFG_C) return cfg_trans_size_c;
			}
			break;
		}
		return 0;
	}

	void SetTextSize(int type, int n, int c)
	{
		switch (type)
		{
		case CFG_NAME:
			{
				if (n == CFG_A) cfg_name_size_a = c;
				else if (n == CFG_B) cfg_name_size_b = c;
				else if (n == CFG_C) cfg_name_size_c = c;
				else if (n == CFG_ALL)
				{
					cfg_name_size_a = c;
					cfg_name_size_b = c;
					cfg_name_size_c = c;
				}
			}
			break;
		case CFG_ORG:
			{
				if (n == CFG_A) cfg_org_size_a = c;
				else if (n == CFG_B) cfg_org_size_b = c;
				else if (n == CFG_C) cfg_org_size_c = c;
				else if (n == CFG_ALL)
				{
					cfg_org_size_a = c;
					cfg_org_size_b = c;
					cfg_org_size_c = c;
				}
			}
			break;
		case CFG_TRANS:
			{
				if (n == CFG_A) cfg_trans_size_a = c;
				else if (n == CFG_B) cfg_trans_size_b = c;
				else if (n == CFG_C) cfg_trans_size_c = c;
				else if (n == CFG_ALL)
				{
					cfg_trans_size_a = c;
					cfg_trans_size_b = c;
					cfg_trans_size_c = c;
				}
			}
			break;
		}
	}

	bool GetTextShadow(int type)
	{
		switch (type)
		{
		case CFG_NAME:
			return cfg_name_shadow;
			break;
		case CFG_ORG:
			return cfg_org_shadow;
			break;
		case CFG_TRANS:
			return cfg_trans_shadow;
			break;
		}
		return 0;
	}

	void SetTextShadow(int type, bool b)
	{
		switch (type)
		{
		case CFG_NAME:
			cfg_name_shadow = b;
			break;
		case CFG_ORG:
			cfg_org_shadow = b;
			break;
		case CFG_TRANS:
			cfg_trans_shadow = b;
			break;
		}
	}

};

