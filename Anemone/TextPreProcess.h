#pragma once

#include <string>

class CTextPreProcess
{
public:
	CTextPreProcess();
	~CTextPreProcess();

	std::wstring PreProcessText(const std::wstring &input, bool bForceOff = false);
};