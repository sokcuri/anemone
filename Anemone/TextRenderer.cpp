#include "StdAfx.h"
#include "TextRenderer.h"


CTextRenderer::CTextRenderer()
{
	szNameOrg = new std::wstring();
	szNameTrans = new std::wstring();
	szContextOrg = new std::wstring();
	szContextTrans = new std::wstring();

	*szContextOrg = L"테스트 문자열입니다 테스트 테스트 테스트!~!@$%^&*()_1234567890";
	*szContextTrans = L"테스트 문자열입니다 테스트 테스트 테스트!~!@$%^&*()_1234567890";
}

bool CTextRenderer::Init()
{
	GdiplusStartupInput gdiplusStartupInput;
	if (GdiplusStartup(&m_gpToken, &gdiplusStartupInput, NULL) != Status(Ok)) return false;
	return true;
}

bool CTextRenderer::Paint()
{
	PAINTSTRUCT ps;
	HDC hDC, memDC;
	HBITMAP hBM;
	RECT rect;

	hDC = BeginPaint(hWnds.Main, &ps);

	GetWindowRect(hWnds.Main, &rect);
	int cx = GetSystemMetrics(SM_CXSCREEN);
	int cy = GetSystemMetrics(SM_CYSCREEN);

	int width = rect.right - rect.left;
	int height = rect.bottom - rect.top;

	memDC = CreateCompatibleDC(hDC);

	BYTE * pBits;
	BITMAPINFOHEADER bmih;

	// 해상도가 바뀌지 않았다면 비트맵을 다시 작성할 필요가 없음
	//if (cx != hBM_x || cy != hBM_y)
	{
		bmih.biSize = sizeof (BITMAPINFOHEADER);
		//bmih.biWidth = r1.right-r1.left;
		//bmih.biHeight = r1.bottom-r1.top;
		bmih.biWidth = cx;
		bmih.biHeight = cy;
		bmih.biPlanes = 1;
		bmih.biBitCount = 32; //
		bmih.biCompression = BI_RGB;
		bmih.biSizeImage = 0;
		bmih.biXPelsPerMeter = 0;
		bmih.biYPelsPerMeter = 0;
		bmih.biClrUsed = 0;
		bmih.biClrImportant = 0;

		hBM = CreateDIBSection(NULL, (BITMAPINFO *)&bmih, 0, (VOID**)&pBits, NULL, 0);

		//hBM_x = cx;
		//hBM_y = cy;
	}

	SelectObject(memDC, hBM);

	using namespace Gdiplus;
	Graphics graphics(memDC);
	graphics.SetSmoothingMode(SmoothingModeAntiAlias);
	graphics.SetInterpolationMode(InterpolationModeHighQualityBicubic);

	FontFamily fontFamily(L"청소년서체");
	StringFormat strformat = StringFormat::GenericTypographic();
	strformat.SetFormatFlags(StringFormatFlagsMeasureTrailingSpaces);
	//wchar_t pszbuf[] = L"테스트 문자열입니다 테스트 테스트 테스트!~!@$%^&*()_1234567890";
	
	{
		GraphicsPath path;
		path.AddString((*szContextOrg).c_str(), (*szContextOrg).length(),
			&fontFamily, FontStyleRegular, 36, Gdiplus::Rect(10, 10, width - 20, height - 20), &strformat);
		for (int i = 1; i < 12; ++i)
		{
			Pen pen(Color(32, 0, 128, 192), (Gdiplus::REAL)i);
			pen.SetLineJoin(LineJoinRound);
			graphics.DrawPath(&pen, &path);
		}

		SolidBrush brush(Color(255, 255, 255));
		graphics.FillPath(&brush, &path);
	}
	{
		GraphicsPath path;
		path.AddString((*szContextTrans).c_str(), (*szContextTrans).length(),
			&fontFamily, FontStyleRegular, 36, Gdiplus::Rect(10, 100, width - 20, height - 20), &strformat);
		Pen penOut(Color(32, 117, 81), 12);
		penOut.SetLineJoin(LineJoinRound);
		graphics.DrawPath(&penOut, &path);

		Pen pen(Color(234, 137, 6), 6);
		pen.SetLineJoin(LineJoinRound);
		graphics.DrawPath(&pen, &path);
		SolidBrush brush(Color(128, 0, 255));
		graphics.FillPath(&brush, &path);
		//SolidBrush brush(Color(255, 255, 255));
		//graphics.FillPath(&brush, &path);
	}

	int nBorderWidth = 5;
	Pen nBorderPen(Color(30, 0, 0, 0), (Gdiplus::REAL)nBorderWidth);

	graphics.DrawRectangle(&nBorderPen, Rect(1, 1, rect.right - rect.left - nBorderWidth - 1, rect.bottom - rect.top - nBorderWidth - 1));

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
	DeleteObject(hBM);
	return true;
}


CTextRenderer::~CTextRenderer()
{
	GdiplusShutdown(m_gpToken);

	delete szNameOrg;
	delete szNameTrans;
	delete szContextOrg;
	delete szContextTrans;
}
