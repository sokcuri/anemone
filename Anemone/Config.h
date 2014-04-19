#pragma once
#define CFG_NAME 0
#define CFG_ORG 1
#define CFG_TRANS 2
#define CFG_NAME_ORG 3

#define CFG_A 0
#define CFG_B 1
#define CFG_C 2
#define CFG_D 3
#define CFG_ALL 4

class CConfig
{
private:
	bool cfg_bg_switch          = true;
	DWORD cfg_bg_color          = 0x130091FB;
	
	bool cfg_name_switch        = true;
	bool cfg_name_org_switch    = true;
	bool cfg_name_shadow        = true;
	wchar_t cfg_name_font[255];
	DWORD cfg_name_color_a      = 0xFFFFFFFF;
	DWORD cfg_name_color_b      = 0xFFFF5E00;
	DWORD cfg_name_color_c      = 0xFFFFA850;
	DWORD cfg_name_color_d      = 0x32000000;
	int cfg_name_size_a         = 25;
	int cfg_name_size_b         = 6;
	int cfg_name_size_c         = 6;

	bool cfg_org_switch         = true;
	bool cfg_org_shadow         = true;
	wchar_t cfg_org_font[255];
	DWORD cfg_org_color_a       = 0xFFFFFFFF;
	DWORD cfg_org_color_b       = 0xFFFF0000;
	DWORD cfg_org_color_c       = 0xFFFBAAAA;
	DWORD cfg_org_color_d       = 0x32000000;
	int cfg_org_size_a          = 25;
	int cfg_org_size_b          = 6;
	int cfg_org_size_c          = 6;

	bool cfg_trans_switch       = true;
	bool cfg_trans_shadow       = true;
	wchar_t cfg_trans_font[255];
	DWORD cfg_trans_color_a     = 0xFFFFFFFF;
	DWORD cfg_trans_color_b     = 0xFF4374D9;
	DWORD cfg_trans_color_c     = 0xFF5CD1E5;
	DWORD cfg_trans_color_d     = 0x32000000;
	int cfg_trans_size_a        = 25;
	int cfg_trans_size_b        = 6;
	int cfg_trans_size_c        = 6;

	bool cfg_temp_click_though = false;
	bool cfg_temp_sizable_mode = false;


public:
	CConfig();
	~CConfig();

	bool LoadConfig();
	bool SaveConfig();

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

	DWORD GetTextColor(int type, int n)
	{
		switch (type)
		{
		case CFG_NAME:
			{
				if (n == CFG_A) return cfg_name_color_a;
				else if (n == CFG_B) return cfg_name_color_b;
				else if (n == CFG_C) return cfg_name_color_c;
				else if (n == CFG_D) return cfg_name_color_d;
			}
			break;
		case CFG_ORG:
			{
				if (n == CFG_A) return cfg_org_color_a;
				else if (n == CFG_B) return cfg_org_color_b;
				else if (n == CFG_C) return cfg_org_color_c;
				else if (n == CFG_D) return cfg_org_color_d;
			}
			break;
		case CFG_TRANS:
			{
				if (n == CFG_A) return cfg_trans_color_a;
				else if (n == CFG_B) return cfg_trans_color_b;
				else if (n == CFG_C) return cfg_trans_color_c;
				else if (n == CFG_D) return cfg_trans_color_d;
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
				else if (n == CFG_D) cfg_name_color_d = c;
			}
			break;
		case CFG_ORG:
			{
				if (n == CFG_A) cfg_org_color_a = c;
				else if (n == CFG_B) cfg_org_color_b = c;
				else if (n == CFG_C) cfg_org_color_c = c;
				else if (n == CFG_D) cfg_org_color_d = c;
			}
			break;
		case CFG_TRANS:
			{
				if (n == CFG_A) cfg_trans_color_a = c;
				else if (n == CFG_B) cfg_trans_color_b = c;
				else if (n == CFG_C) cfg_trans_color_c = c;
				else if (n == CFG_D) cfg_trans_color_d = c;
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

