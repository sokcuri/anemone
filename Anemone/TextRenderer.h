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

	void SetName(const wchar_t *str)
	{
		int nLen = wcslen(str);
		if (nLen > 1000) nLen = 1000;
		wcsncpy_s(szName, str, nLen);
	}
	wchar_t *GetName()
	{
		return szName;
	}

	void SetNameT(const wchar_t *str)
	{
		int nLen = wcslen(str);
		if (nLen > 1000) nLen = 1000;
		wcsncpy_s(szNameT, str, nLen);
	}
	wchar_t *GetNameT()
	{
		return szNameT;
	}

	void SetText(const wchar_t *str)
	{
		int nLen = wcslen(str);
		if (nLen > 1000) nLen = 1000;
		wcsncpy_s(szText, str, nLen);
	}
	wchar_t *GetText()
	{
		return szText;
	}

	void SetTextT(const wchar_t *str)
	{
		int nLen = wcslen(str);
		if (nLen > 1000) nLen = 1000;
		wcsncpy_s(szTextT, str, nLen);
	}
	wchar_t *GetTextT()
	{
		return szTextT;
	}

	void SetContext()
	{
		int nNameLen, nTextLen;
		nNameLen = wcslen(szName);
		if (nNameLen > 1000) nNameLen = 1000;
		wcsncpy_s(szContext, szName, nNameLen);

		nTextLen = wcslen(szText);
		if (nTextLen > 1000) nTextLen = 1000;
		wcsncpy_s(szContext + nNameLen, sizeof(szContext), szText, nTextLen);
	}
	wchar_t *GetContext()
	{
		return szContext;
	}

	void SetContextT()
	{
		int nNameLen, nTextLen;
		nNameLen = wcslen(szNameT);
		if (nNameLen > 1000) nNameLen = 1000;
		wcsncpy_s(szContextT, szNameT, nNameLen);

		nTextLen = wcslen(szTextT);
		if (nTextLen > 1000) nTextLen = 1000;
		wcsncpy_s(szContextT + nNameLen, sizeof(szContextT), szTextT, nTextLen);
	}
	wchar_t *GetContextT()
	{
		return szContext;
	}
	
	void SetTextSet(const wchar_t *Name, const wchar_t *NameT, const wchar_t *Text, const wchar_t *TextT)
	{
		SetName(Name);
		SetNameT(NameT);
		SetText(Text);
		SetTextT(TextT);
		SetContext();
		SetContextT();
	}

	//std::wstring *szName, *szNameT, *szText, *szTextT, *szContext, *szContextT;
private:
	ULONG_PTR m_gpToken;
	HBITMAP hBitmap;
	BITMAPINFOHEADER bmih;
	int hBitmap_X = 0;
	int hBitmap_Y = 0;
	//HDC hMemDC;

	wchar_t szName[1024];
	wchar_t szNameT[1024];
	wchar_t szText[1024];
	wchar_t szTextT[1024];
	wchar_t szContext[2048];
	wchar_t szContextT[2048];

};

