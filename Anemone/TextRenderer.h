#pragma once
#include <GdiPlus.h>
using namespace Gdiplus;
class CTextRenderer
{
public:
	CTextRenderer();
	~CTextRenderer();
	bool Init();
	bool Paint();
	int DrawText(Graphics *graphics, const wchar_t *contextText, wchar_t *fontName, int fntSize, int outlineInThick, int outlineOutThick, Color textColor, Color outlineInColor, Color outlineOutColor, Color shadowColor, bool textVisible, bool outlineInVisible, bool outlineOutVisible, bool shadowVisible, Rect *layoutRect);
	std::wstring *szNameOrg;
	std::wstring *szNameTrans;
	std::wstring *szContextOrg;
	std::wstring *szContextTrans;
private:
	ULONG_PTR m_gpToken;
	//HDC hMemDC;

};

