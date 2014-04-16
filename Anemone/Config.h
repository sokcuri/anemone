#pragma once
class CConfig
{
private:
	bool cfg_bg_switch = true;
	DWORD cfg_bg_color = 0x130091FB;
public:
	CConfig();
	~CConfig();

	bool LoadConfig();
	bool SaveConfig();
	
	bool GetBGSwitch() { return cfg_bg_switch; }
	DWORD GetBGColor() { return cfg_bg_color; }

	void SetBGSwitch(bool var) { cfg_bg_switch = var; }
	void SetBGColor(DWORD dw) { cfg_bg_color = dw; }
};

