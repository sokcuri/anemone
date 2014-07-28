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
	ULONG_PTR GetgpToken()
	{
		return m_gpToken;
	}
	int TextDraw(Graphics *graphics, const wchar_t *contextText, wchar_t *fontName, int fontStyle, int fntSize, int outlineInThick, int outlineOutThick, Color textColor, Color outlineInColor, Color outlineOutColor, Color shadowColor, bool textVisible, bool outlineInVisible, bool outlineOutVisible, bool shadowVisible, Rect *layoutRect);
	std::wstring *szName, *szNameT, *szText, *szTextT, *szContext, *szContextT;
private:
	ULONG_PTR m_gpToken;
	HBITMAP hBitmap;
	BITMAPINFOHEADER bmih;
	int hBitmap_X = 0;
	int hBitmap_Y = 0;
	//HDC hMemDC;

};

