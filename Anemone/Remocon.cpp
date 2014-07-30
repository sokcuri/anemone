#include "stdafx.h"
#include "Remocon.h"

CRemocon *CRemocon::m_pThis = NULL;
DWORD CRemocon::bgColor = 0x4BFFFFFF;
bool CRemocon::m_bTracking = false;

int CRemocon::m_nBtnStatus = 0;

CRemocon::CRemocon()
{
}

CRemocon::~CRemocon()
{
	DestroyWindow(hWnds.Remocon);
}

bool CRemocon::CreateInstance()
{
	WNDCLASSEX wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInst;
	wcex.hIcon = LoadIcon(hInst, MAKEINTRESOURCE(IDI_ANEMONE));
	wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wcex.lpszMenuName = 0;
	wcex.lpszClassName = L"AnemoneRemoconClass";
	wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_ANEMONE));

	RegisterClassEx(&wcex);

	int cx = GetSystemMetrics(SM_CXSCREEN);
	int cy = GetSystemMetrics(SM_CYSCREEN);

	int x = 273+40;
	int y = 91+40;

	hWnds.Remocon = CreateWindowEx(WS_EX_TOOLWINDOW | WS_EX_NOACTIVATE | WS_EX_LAYERED | WS_EX_TOPMOST, L"AnemoneRemoconClass", L"아네모네 리모콘", WS_POPUP,
		(cx - x) / 2, (cy - y) / 2, x, y, NULL, NULL, hInst, NULL);

	if (!hWnds.Remocon)
	{
		MessageBox(0, L"리모콘 윈도우 생성 실패!", 0, 0);
		return false;
	}

	UpdateWindow(hWnds.Remocon);

	Paint();

	return true;
}

bool CRemocon::Paint()
{
	if (Cl.TextRenderer->GetgpToken() == NULL) return false;

	PAINTSTRUCT ps;
	HDC hDC, memDC;
	RECT rect;

	hDC = BeginPaint(hWnds.Remocon, &ps);

	GetWindowRect(hWnds.Remocon, &rect);
	int cx = GetSystemMetrics(SM_CXSCREEN);
	int cy = GetSystemMetrics(SM_CYSCREEN);

	int width = rect.right - rect.left;
	int height = rect.bottom - rect.top;

	memDC = CreateCompatibleDC(hDC);

	BYTE * pBits;

	HBITMAP hBitmap;
	BITMAPINFOHEADER bmih;

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
	SelectObject(memDC, hBitmap);

	using namespace Gdiplus;
	Graphics graphics(memDC);
	graphics.SetSmoothingMode(SmoothingModeAntiAlias);
	graphics.SetTextRenderingHint(TextRenderingHintAntiAliasGridFit);
	graphics.SetInterpolationMode(InterpolationModeHighQualityBicubic);

	graphics.Clear(Color((bgColor >> 24) & 0xFF, (bgColor >> 16) & 0xFF, (bgColor >> 8) & 0xFF, bgColor & 0xFF));

	int nBorderWidth = 5;
	Pen nBorderPen(Color(30, 0, 0, 0), (Gdiplus::REAL)nBorderWidth);

	SolidBrush brush(Color(32, 0, 0, 0));
	Pen pen(Color(16, 255, 255, 255), 10);

	std::wstring imgPath;
	GetLoadPath(imgPath, L"\\remocon.png");

	Image image(imgPath.c_str());
	graphics.DrawImage(&image, 20, 20, image.GetWidth(), image.GetHeight());

	SolidBrush fade(Color(24, 0, 0, 0));
	if (m_nBtnStatus != 0)
	graphics.FillRectangle(&fade, Rect(20 + (90 * ((m_nBtnStatus % 4) - 1)) + ((m_nBtnStatus % 4) - 1), 20 + (90 * (m_nBtnStatus / 4) + (m_nBtnStatus / 4)), 90, 90));

	POINT dcOffset = { 0, 0 };
	SIZE size = { rect.right - rect.left, rect.bottom - rect.top };

	BLENDFUNCTION bf;
	bf.BlendOp = AC_SRC_OVER;
	bf.BlendFlags = 0;
	bf.SourceConstantAlpha = 255;
	bf.AlphaFormat = AC_SRC_ALPHA;

	UpdateLayeredWindow(hWnds.Remocon, hDC, 0, &size, memDC, &dcOffset, 0, &bf, ULW_ALPHA);

	EndPaint(hWnds.Remocon, &ps);

	ReleaseDC(NULL, hDC);
	ReleaseDC(NULL, memDC);

	DeleteObject(memDC);
	DeleteObject(hDC);

	DeleteObject(hBitmap);
	return true;
}

LRESULT CRemocon::_WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	int wmId, wmEvent;

	switch (message)
	{
	case WM_CREATE:
	{
	}
		break;
	case WM_COMMAND:
		wmId = LOWORD(wParam);
		wmEvent = HIWORD(wParam);

		switch (wmId)
		{
		case ID_EXIT:
			DestroyWindow(hWnd);
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
		break;
	case WM_PAINT:
	{
		Paint();
	}
		break;
	case WM_MOVING:
	case WM_SIZING:
	{
		RECT *prc = (RECT *)lParam;
		SetWindowPos(hWnd, NULL, prc->left, prc->top, prc->right - prc->left, prc->bottom - prc->top, 0);
		if (message == WM_SIZING) Paint();
	}
		break;
	case WM_MOUSEMOVE:
	{
		if (!m_bTracking)
		{
			TRACKMOUSEEVENT tme;
			tme.cbSize = sizeof(tme);
			tme.hwndTrack = hWnd;
			tme.dwFlags = TME_LEAVE | TME_HOVER;
			tme.dwHoverTime = 1;
			m_bTracking = TrackMouseEvent(&tme) ? true : false;
		}
	}
		break;
	case WM_MOUSEHOVER:
	{
		POINT pt;
		pt.x = LOWORD(lParam);
		pt.y = HIWORD(lParam);

		// PREV
		if (pt.x >= 20 && pt.x <= 110 && pt.y >= 20 && pt.y <= 110)
		{
			m_nBtnStatus = 1;
		}

		// NEXT
		else if (pt.x >= 112 && pt.x <= 202 && pt.y >= 20 && pt.y <= 110)
		{
			m_nBtnStatus = 2;
		}

		// CONFIG
		else if (pt.x >= 204 && pt.x <= 294 && pt.y >= 20 && pt.y <= 110)
		{
			m_nBtnStatus = 3;
		}

		else m_nBtnStatus = 0;

		Paint();
		m_bTracking = false;
		break;
	}
	case WM_MOUSELEAVE:
		m_nBtnStatus = 0;
		Paint();
		m_bTracking = false;
		break;
	case WM_LBUTTONDOWN:
		break;
	case WM_LBUTTONUP:
		POINT pt;
		pt.x = LOWORD(lParam);
		pt.y = HIWORD(lParam);

		// PREV
		if (pt.x >= 20 && pt.x <= 110 && pt.y >= 20 && pt.y <= 110)
		{
			if (message == WM_LBUTTONUP) MessageBox(hWnd, L"PREV CLICK", 0, 0);
		}

		// NEXT
		else if (pt.x >= 112 && pt.x <= 202 && pt.y >= 20 && pt.y <= 110)
		{
			if (message == WM_LBUTTONUP) MessageBox(hWnd, L"NEXT CLICK", 0, 0);
		}

		// CONFIG
		else if (pt.x >= 204 && pt.x <= 294 && pt.y >= 20 && pt.y <= 110)
		{
			if (message == WM_LBUTTONUP) PostMessage(hWnds.Main, WM_COMMAND, MAKELONG(ID_WINDOW_SETTING, 0), 0);
		}

		else if (message == WM_LBUTTONDOWN) SendMessage(hWnd, WM_NCLBUTTONDOWN, HTCAPTION, 0);
		break;
	case WM_ERASEBKGND:
		return false;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}