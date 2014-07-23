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

	// 텍스트 정렬
	if (Cl.Config->GetTextAlign() == 0) strformat.SetAlignment(StringAlignmentNear);
	else if (Cl.Config->GetTextAlign() == 1) strformat.SetAlignment(StringAlignmentCenter);
	else if (Cl.Config->GetTextAlign() == 2) strformat.SetAlignment(StringAlignmentFar);

	//strformat.SetFormatFlags(StringFormatFlagsMeasureTrailingSpaces);
	strformat.SetTrimming(StringTrimmingCharacter);
	
	Gdiplus::RectF boundRect;

	int outlineTotalThick = 0;
	if (outlineInVisible) outlineTotalThick += outlineInThick;
	if (outlineOutVisible) outlineTotalThick += outlineOutThick;


	Font font(&fontFamily, (Gdiplus::REAL)fntSize, FontStyleRegular);
	graphics->MeasureString(contextText, wcslen(contextText), &font, Gdiplus::RectF((Gdiplus::REAL)layoutRect->X, (Gdiplus::REAL)layoutRect->Y, (Gdiplus::REAL)layoutRect->Width, (Gdiplus::REAL)layoutRect->Height), &strformat, &boundRect);
	//SolidBrush tempBrush(outlineOutColor);
	//graphics->DrawString(contextText, wcslen(contextText), &font, Gdiplus::RectF((Gdiplus::REAL)layoutRect->X, (Gdiplus::REAL)layoutRect->Y, (Gdiplus::REAL)layoutRect->Width, (Gdiplus::REAL)layoutRect->Height), &strformat, &tempBrush);

	Gdiplus::REAL emSize = graphics->GetDpiY() * fntSize / 72;

	int shadowX = Cl.Config->GetShadowX()-10+4;
	int shadowY = Cl.Config->GetShadowY()-10+4;

	if (shadowVisible)
	{
		path_shadow.AddString(contextText, wcslen(contextText),
			&fontFamily, FontStyleRegular, emSize, Gdiplus::Rect(layoutRect->X + shadowX, layoutRect->Y + shadowY, layoutRect->Width, layoutRect->Height), &strformat);

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
	if (m_gpToken == NULL) return false;

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

	//130091FB
	bool bBGSwitch = Cl.Config->GetBGSwitch();
	DWORD BGColor = Cl.Config->GetBGColor();

	if (!Cl.Config->GetSizableMode())
	{
		if (!IsActive && !bBGSwitch) graphics.Clear(Color(1, 0, 0, 0));
		else if (bBGSwitch) graphics.Clear(Color((BGColor >> 24) & 0xFF, (BGColor >> 16) & 0xFF, (BGColor >> 8) & 0xFF, (BGColor)& 0xFF));
		else graphics.Clear(Color(0, 0, 0, 0));
	}
	// 크기 조절 모드
	else
	{
		if (!IsActive && !bBGSwitch) graphics.Clear(Color(1, 0, 0, 0));
		else if (bBGSwitch) graphics.Clear(Color((BGColor >> 24) & 0xFF, (BGColor >> 16) & 0xFF, (BGColor >> 8) & 0xFF, (BGColor)& 0xFF));
		else graphics.Clear(Color(1, 0, 0, 0));

		int nBorderWidth = 8;
		Pen nBorderPen(Color(0x80, 0, 0, 0), (Gdiplus::REAL)nBorderWidth);

		graphics.DrawRectangle(&nBorderPen, Rect((nBorderWidth / 2) - 1, (nBorderWidth / 2) - 1, rect.right - rect.left - nBorderWidth + 1, rect.bottom - rect.top - nBorderWidth + 1));
	}


	StringFormat strformat = StringFormat::GenericTypographic();
	strformat.SetFormatFlags(StringFormatFlagsMeasureTrailingSpaces);

	int pad_y = 20;

	bool bNameSwitch = Cl.Config->GetTextSwitch(CFG_NAME);
	bool bNameOrgSwitch = Cl.Config->GetTextSwitch(CFG_NAME_ORG);
	bool bNameShadow = Cl.Config->GetTextShadow(CFG_NAME);

	wchar_t *fnName = Cl.Config->GetTextFont(CFG_NAME);

	//DWORD dwShadow = Cl.Config->GetShadowColor();

	int nNameA = Cl.Config->GetTextSize(CFG_NAME, CFG_A);
	int nNameB = Cl.Config->GetTextSize(CFG_NAME, CFG_B);
	int nNameC = Cl.Config->GetTextSize(CFG_NAME, CFG_C);

	DWORD dwNameA = Cl.Config->GetTextColor(CFG_NAME, CFG_A);
	DWORD dwNameB = Cl.Config->GetTextColor(CFG_NAME, CFG_B);
	DWORD dwNameC = Cl.Config->GetTextColor(CFG_NAME, CFG_C);
	DWORD dwNameS = Cl.Config->GetTextColor(CFG_NAME, CFG_S);

	bool bOrgSwitch = Cl.Config->GetTextSwitch(CFG_ORG);
	bool bOrgShadow = Cl.Config->GetTextShadow(CFG_ORG);

	wchar_t *fnOrg = Cl.Config->GetTextFont(CFG_ORG);

	int nOrgA = Cl.Config->GetTextSize(CFG_ORG, CFG_A);
	int nOrgB = Cl.Config->GetTextSize(CFG_ORG, CFG_B);
	int nOrgC = Cl.Config->GetTextSize(CFG_ORG, CFG_C);

	DWORD dwOrgA = Cl.Config->GetTextColor(CFG_ORG, CFG_A);
	DWORD dwOrgB = Cl.Config->GetTextColor(CFG_ORG, CFG_B);
	DWORD dwOrgC = Cl.Config->GetTextColor(CFG_ORG, CFG_C);
	DWORD dwOrgS = Cl.Config->GetTextColor(CFG_ORG, CFG_S);

	bool bTransSwitch = Cl.Config->GetTextSwitch(CFG_TRANS);
	bool bTransShadow = Cl.Config->GetTextShadow(CFG_TRANS);

	wchar_t *fnTrans = Cl.Config->GetTextFont(CFG_TRANS);

	int nTransA = Cl.Config->GetTextSize(CFG_TRANS, CFG_A);
	int nTransB = Cl.Config->GetTextSize(CFG_TRANS, CFG_B);
	int nTransC = Cl.Config->GetTextSize(CFG_TRANS, CFG_C);

	DWORD dwTransA = Cl.Config->GetTextColor(CFG_TRANS, CFG_A);
	DWORD dwTransB = Cl.Config->GetTextColor(CFG_TRANS, CFG_B);
	DWORD dwTransC = Cl.Config->GetTextColor(CFG_TRANS, CFG_C);
	DWORD dwTransS = Cl.Config->GetTextColor(CFG_TRANS, CFG_S);

	if (Cl.Config->GetTextSize(CFG_NAME, CFG_B) == 0)
		dwNameB = dwNameB & 0xFFFFFF;
	if (Cl.Config->GetTextSize(CFG_NAME, CFG_C) == 0)
		dwNameC = dwNameC & 0xFFFFFF;

	if (Cl.Config->GetTextSize(CFG_ORG, CFG_B) == 0)
		dwOrgB = dwOrgB & 0xFFFFFF;
	if (Cl.Config->GetTextSize(CFG_ORG, CFG_C) == 0)
		dwOrgC = dwOrgC & 0xFFFFFF;

	if (Cl.Config->GetTextSize(CFG_TRANS, CFG_B) == 0)
		dwTransB = dwTransB & 0xFFFFFF;
	if (Cl.Config->GetTextSize(CFG_TRANS, CFG_C) == 0)
		dwTransC = dwTransC & 0xFFFFFF;


	// 원문 이름 괄호 옆에 붙이기
	std::wstring szNameConv = (*szNameT);
	szNameConv += L" ";

	if (bNameSwitch)
	{
		if (bNameOrgSwitch)
		{
			szNameConv += L"(";
			szNameConv += (*szName);
			szNameConv += L")";

			szNameConv = replaceAll(szNameConv, L"()", L"");
		}
	}

	int mar_x = Cl.Config->GetTextMarginX();
	int mar_y = Cl.Config->GetTextMarginY();
	int mar_name = Cl.Config->GetTextMarginName();

	if (!IsActive)
	{
		int nBorderWidth = 5;
		Pen nBorderPen(Color(30, 0, 0, 0), (Gdiplus::REAL)nBorderWidth);

		SolidBrush brush(Color(32, 0, 0, 0));
		Pen pen(Color(16, 255, 255, 255), 10);

		//for (int i = 1; i <= 10; i++)
		//	graphics.DrawLine(&pen, width / 10 * i * 2, 0, 0, height / 10 * i * 2);

		DrawText(&graphics, L"~아네모네 V1.00 알파 버전~\r\nby 소쿠릿", fnTrans, nTransA, nTransB, nTransC, Color((dwTransA >> 24) & 0xFF, (dwTransA >> 16) & 0xFF, (dwTransA >> 8) & 0xFF, (dwTransA)& 0xFF), Color((dwTransB >> 24) & 0xFF, (dwTransB >> 16) & 0xFF, (dwTransB >> 8) & 0xFF, (dwTransB)& 0xFF), Color((dwTransC >> 24) & 0xFF, (dwTransC >> 16) & 0xFF, (dwTransC >> 8) & 0xFF, (dwTransC)& 0xFF), Color((dwTransS >> 24) & 0xFF, (dwTransS >> 16) & 0xFF, (dwTransS >> 8) & 0xFF, (dwTransS)& 0xFF), true, true, true, bTransShadow, &Gdiplus::Rect(20 + mar_x, pad_y + mar_y, width - 40 - mar_x, height + 300 - mar_y));

		if (!Cl.Config->GetSizableMode()) graphics.DrawRectangle(&nBorderPen, Rect((nBorderWidth / 2), (nBorderWidth / 2), rect.right - rect.left - nBorderWidth, rect.bottom - rect.top - nBorderWidth));
	}
	else
	{
		pad_y += DrawText(&graphics, (bNameSwitch ? (szNameConv).c_str() : L" "), fnName, nNameA, nNameB, nNameC, Color((dwNameA >> 24) & 0xFF, (dwNameA >> 16) & 0xFF, (dwNameA >> 8) & 0xFF, (dwNameA)& 0xFF), Color((dwNameB >> 24) & 0xFF, (dwNameB >> 16) & 0xFF, (dwNameB >> 8) & 0xFF, (dwNameB)& 0xFF), Color((dwNameC >> 24) & 0xFF, (dwNameC >> 16) & 0xFF, (dwNameC >> 8) & 0xFF, (dwNameC)& 0xFF), Color((dwNameS >> 24) & 0xFF, (dwNameS >> 16) & 0xFF, (dwNameS >> 8) & 0xFF, (dwNameS)& 0xFF), true, true, true, bNameShadow, &Gdiplus::Rect(20 + mar_name, pad_y, width - 40 - mar_name, height + 300));
		if (bOrgSwitch)   pad_y += DrawText(&graphics, (bNameSwitch ? (*szText).c_str() : (*szContext).c_str()), fnOrg, nOrgA, nOrgB, nOrgC, Color((dwOrgA >> 24) & 0xFF, (dwOrgA >> 16) & 0xFF, (dwOrgA >> 8) & 0xFF, (dwOrgA)& 0xFF), Color((dwOrgB >> 24) & 0xFF, (dwOrgB >> 16) & 0xFF, (dwOrgB >> 8) & 0xFF, (dwOrgB)& 0xFF), Color((dwOrgC >> 24) & 0xFF, (dwOrgC >> 16) & 0xFF, (dwOrgC >> 8) & 0xFF, (dwOrgC)& 0xFF), Color((dwOrgS >> 24) & 0xFF, (dwOrgS >> 16) & 0xFF, (dwOrgS >> 8) & 0xFF, (dwOrgS)& 0xFF), true, true, true, bOrgShadow, &Gdiplus::Rect(20 + mar_x, pad_y + mar_y, width - 40 - mar_x, height + 300 - mar_y));
		if (bTransSwitch) pad_y += DrawText(&graphics, (bNameSwitch ? (*szTextT).c_str() : (*szContextT).c_str()), fnTrans, nTransA, nTransB, nTransC, Color((dwTransA >> 24) & 0xFF, (dwTransA >> 16) & 0xFF, (dwTransA >> 8) & 0xFF, (dwTransA)& 0xFF), Color((dwTransB >> 24) & 0xFF, (dwTransB >> 16) & 0xFF, (dwTransB >> 8) & 0xFF, (dwTransB)& 0xFF), Color((dwTransC >> 24) & 0xFF, (dwTransC >> 16) & 0xFF, (dwTransC >> 8) & 0xFF, (dwTransC)& 0xFF), Color((dwTransS >> 24) & 0xFF, (dwTransS >> 16) & 0xFF, (dwTransS >> 8) & 0xFF, (dwTransS) & 0xFF), true, true, true, bTransShadow, &Gdiplus::Rect(20 + mar_x, pad_y + mar_y, width - 40 - mar_x, height + 300 - mar_y));
	}
	int nBorderWidth = 5;
	//Pen nBorderPen(Color(30, 0, 0, 0), (Gdiplus::REAL)nBorderWidth);
	//graphics.DrawRectangle(&nBorderPen, Rect((nBorderWidth / 2), (nBorderWidth / 2), rect.right - rect.left - nBorderWidth, rect.bottom - rect.top - nBorderWidth));

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
{
	if (hBitmap_X != 0 && hBitmap_Y != 0) DeleteObject(hBitmap);

	GdiplusShutdown(m_gpToken);
	m_gpToken = NULL;
}
