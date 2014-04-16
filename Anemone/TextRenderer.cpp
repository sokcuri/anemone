#include "StdAfx.h"
#include "TextRenderer.h"

CTextRenderer::CTextRenderer()
{
	szName = new std::wstring();
	szNameT = new std::wstring();
	szText = new std::wstring();
	szTextT = new std::wstring();
	szContext = new std::wstring();
	szContextT = new std::wstring();
}

bool CTextRenderer::Init()
{
	GdiplusStartupInput gdiplusStartupInput;
	if (GdiplusStartup(&m_gpToken, &gdiplusStartupInput, NULL) != Status(Ok)) return false;
	return true;
}

int CTextRenderer::DrawText(Graphics *graphics, const wchar_t *contextText, wchar_t *fontName, int fntSize, int outlineInThick, int outlineOutThick, Color textColor, Color outlineInColor, Color outlineOutColor, Color shadowColor, bool textVisible, bool outlineInVisible, bool outlineOutVisible, bool shadowVisible, Rect *layoutRect)
{
	// 텍스트를 표시하지 않으면 리턴시킨다
	if (textVisible == false) return true;

	GraphicsPath path;
	GraphicsPath path_shadow;

	// 폰트 설정
	FontFamily fontFamily(fontName);
	if (!fontFamily.IsAvailable())
	{
		MessageBox(0, L"폰트 사용 불가능", 0, 0);
		return false;
	}

	StringFormat strformat = StringFormat::GenericTypographic();
	//strformat.SetFormatFlags(StringFormatFlagsMeasureTrailingSpaces);
	strformat.SetTrimming(StringTrimmingCharacter);
	
	Gdiplus::RectF boundRect;

	int outlineTotalThick = 0;
	if (outlineInVisible) outlineTotalThick += outlineInThick;
	if (outlineOutVisible) outlineTotalThick += outlineOutThick;

	Font font(&fontFamily, (Gdiplus::REAL)fntSize, FontStyleRegular);
	graphics->MeasureString(contextText, wcslen(contextText), &font, Gdiplus::RectF((Gdiplus::REAL)layoutRect->X, (Gdiplus::REAL)layoutRect->Y, (Gdiplus::REAL)layoutRect->Width, (Gdiplus::REAL)layoutRect->Height), &strformat, &boundRect);
	SolidBrush tempBrush(outlineOutColor);
	graphics->DrawString(contextText, wcslen(contextText), &font, Gdiplus::RectF((Gdiplus::REAL)layoutRect->X, (Gdiplus::REAL)layoutRect->Y, (Gdiplus::REAL)layoutRect->Width, (Gdiplus::REAL)layoutRect->Height), &strformat, &tempBrush);

	Gdiplus::REAL emSize = graphics->GetDpiY() * fntSize / 72;

	if (shadowVisible)
	{
		path_shadow.AddString(contextText, wcslen(contextText),
			&fontFamily, FontStyleRegular, emSize, Gdiplus::Rect(layoutRect->X + 4, layoutRect->Y + 4, layoutRect->Width, layoutRect->Height), &strformat);

		Pen pen_shadow(shadowColor, (Gdiplus::REAL)outlineTotalThick);
		pen_shadow.SetLineJoin(LineJoinRound);
		graphics->DrawPath(&pen_shadow, &path_shadow);
	}

	path.AddString(contextText, wcslen(contextText),
		&fontFamily, FontStyleRegular, emSize, Gdiplus::Rect(layoutRect->X, layoutRect->Y, layoutRect->Width, layoutRect->Height), &strformat);

	if (outlineOutVisible)
	{
		Pen penOut(outlineOutColor, (Gdiplus::REAL)outlineTotalThick);
		penOut.SetLineJoin(LineJoinRound);
		graphics->DrawPath(&penOut, &path);
	}

	if (outlineInVisible)
	{
		Pen pen(outlineInColor, (Gdiplus::REAL)outlineInThick);
		pen.SetLineJoin(LineJoinRound);
		graphics->DrawPath(&pen, &path);
	}

	SolidBrush brush(textColor);
	graphics->FillPath(&brush, &path);

	return (int)(boundRect.Height);
}

bool CTextRenderer::Paint()
{
	PAINTSTRUCT ps;
	HDC hDC, memDC;
	RECT rect;

	hDC = BeginPaint(hWnds.Main, &ps);

	GetWindowRect(hWnds.Main, &rect);
	int cx = GetSystemMetrics(SM_CXSCREEN);
	int cy = GetSystemMetrics(SM_CYSCREEN);

	int width = rect.right - rect.left;
	int height = rect.bottom - rect.top;

	memDC = CreateCompatibleDC(hDC);

	BYTE * pBits;

	// 해상도가 바뀌지 않았다면 비트맵을 다시 작성할 필요가 없음
	if (cx != hBitmap_X || cy != hBitmap_Y)
	{
		// hBitmap_X, hBitmap_Y가 0이 아니면 hBitmap 반환
		if (hBitmap_X != 0 && hBitmap_Y != 0) DeleteObject(hBitmap);

		bmih.biSize = sizeof (BITMAPINFOHEADER);
		bmih.biWidth = cx;
		bmih.biHeight = cy;
		bmih.biPlanes = 1;
		bmih.biBitCount = 32; 
		bmih.biCompression = BI_RGB;
		bmih.biSizeImage = 0;
		bmih.biXPelsPerMeter = 0;
		bmih.biYPelsPerMeter = 0;
		bmih.biClrUsed = 0;
		bmih.biClrImportant = 0;

		hBitmap = CreateDIBSection(NULL, (BITMAPINFO *)&bmih, 0, (VOID**)&pBits, NULL, 0);

		hBitmap_X = cx;
		hBitmap_Y = cy;
	}

	SelectObject(memDC, hBitmap);

	using namespace Gdiplus;
	Graphics graphics(memDC);
	graphics.SetSmoothingMode(SmoothingModeAntiAlias);
	graphics.SetTextRenderingHint(TextRenderingHintAntiAliasGridFit);
	graphics.SetInterpolationMode(InterpolationModeHighQualityBicubic);

	if (IsActive == false)
	{
		graphics.Clear(Color(75, 0, 216, 255));

		int nBorderWidth = 5;
		Pen nBorderPen(Color(30, 0, 0, 0), (Gdiplus::REAL)nBorderWidth);

		SolidBrush brush(Color(32, 0, 0, 0));
		Pen pen(Color(16, 255, 255, 255), 10);
		
		for (int i = 1; i <= 10; i++)
			graphics.DrawLine(&pen, width / 10 * i * 2, 0, 0, height / 10 * i * 2);
		
		DrawText(&graphics, L"~아네모네 V1.00 알파 버전~ by eroha", L"맑은 고딕", 25, 6, 6, Color(255, 255, 255, 255), Color(255, 67, 116, 217), Color(255, 139, 189, 255), Color(32, 0, 0, 0), true, true, true, true, &Gdiplus::Rect(20, 20, width - 40, height + 300));

		graphics.DrawRectangle(&nBorderPen, Rect(2, 2, rect.right - rect.left - nBorderWidth - 1, rect.bottom - rect.top - nBorderWidth - 1));
	}
	else
	{
		//130091FB
		DWORD BGColor = Cl.Config->GetBGColor();
		graphics.Clear(Color((BGColor >> 24) & 0xFF, (BGColor >> 16) & 0xFF, (BGColor >> 8) & 0xFF, (BGColor) & 0xFF));

		StringFormat strformat = StringFormat::GenericTypographic();
		strformat.SetFormatFlags(StringFormatFlagsMeasureTrailingSpaces);
		//wchar_t pszbuf[] = L"테스트 문자열입니다 테스트 테스트 테스트!~!@$%^&*()_1234567890";

		int i_x = 10;
		int i_y = 10;

		int i_shadow_width = 3;
		int i_outline_width = 12;
		// 5 -> 3
		// 10 -> 5

		int pad_y = 20;

		std::wstring szNameConv = (*szName);
		szNameConv += L" ";
		szNameConv += L"(";
		szNameConv += (*szNameT);
		szNameConv += L")";

		szNameConv = replaceAll(szNameConv, L"()", L"");

		pad_y += DrawText(&graphics, (szNameConv).c_str(), L"맑은 고딕", 25, 6, 6, Color(255, 255, 255, 255), Color(255, 255, 94, 0), Color(255, 255, 166, 80), Color(32, 0, 0, 0), true, true, true, true, &Gdiplus::Rect(20, pad_y, width - 40, height + 300));
		pad_y += DrawText(&graphics, (*szText).c_str(), L"맑은 고딕", 25, 6, 6, Color(255, 255, 255, 255), Color(255, 255, 0, 0), Color(255, 251, 170, 170), Color(32, 0, 0, 0), true, true, true, true, &Gdiplus::Rect(20, pad_y, width - 40, height + 300));
		pad_y += DrawText(&graphics, (*szTextT).c_str(), L"맑은 고딕", 25, 6, 6, Color(255, 255, 255, 255), Color(255, 67, 116, 217), Color(255, 92, 209, 229), Color(32, 0, 0, 0), true, true, true, true, &Gdiplus::Rect(20, pad_y, width - 40, height + 300));
		
		int nBorderWidth = 5;
		Pen nBorderPen(Color(30, 0, 0, 0), (Gdiplus::REAL)nBorderWidth);

		graphics.DrawRectangle(&nBorderPen, Rect(1, 1, rect.right - rect.left - nBorderWidth + 1, rect.bottom - rect.top - nBorderWidth + 1));
	}
	POINT dcOffset = { 0, 0 };
	SIZE size = { rect.right - rect.left, rect.bottom - rect.top };

	BLENDFUNCTION bf;
	bf.BlendOp = AC_SRC_OVER;
	bf.BlendFlags = 0;
	bf.SourceConstantAlpha = 255;
	bf.AlphaFormat = AC_SRC_ALPHA;

	UpdateLayeredWindow(hWnds.Main, hDC, 0, &size, memDC, &dcOffset, 0, &bf, ULW_ALPHA);

	EndPaint(hWnds.Main, &ps);

	ReleaseDC(NULL, hDC);
	ReleaseDC(NULL, memDC);

	DeleteObject(memDC);
	DeleteObject(hDC); 
	return true;
}


CTextRenderer::~CTextRenderer()
{	if (hBitmap_X != 0 && hBitmap_Y != 0) DeleteObject(hBitmap);

	GdiplusShutdown(m_gpToken);

	delete szName;
	delete szNameT;
	delete szText;
	delete szTextT;
	delete szContext;
	delete szContextT;
}
