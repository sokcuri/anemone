// Anemone.cpp : 응용 프로그램에 대한 진입점을 정의합니다.
//

#include "stdafx.h"
#include "Anemone.h"

#define MAX_LOADSTRING 100


// 전역 변수:
TCHAR szTitle[MAX_LOADSTRING];					// 제목 표시줄 텍스트입니다.
TCHAR szWindowClass[MAX_LOADSTRING];			// 기본 창 클래스 이름입니다.
std::vector<_key_map> key_map;
HINSTANCE hInst; _hWnds hWnds; _Class Cl; HANDLE AneHeap;
bool IsActive = false;

// 이 코드 모듈에 들어 있는 함수의 정방향 선언입니다.
ATOM				MyRegisterClass(HINSTANCE hInstance);
BOOL				InitInstance(HINSTANCE, int);
LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK	About(HWND, UINT, WPARAM, LPARAM);

int APIENTRY _tWinMain(
		__in HINSTANCE hInstance,
		__in_opt HINSTANCE hPrevInstance,
		__in_opt LPWSTR lpCmdLine,
		__in int nCmdShow
	)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

 	// TODO: 여기에 코드를 입력합니다.
	MSG msg;
	HACCEL hAccelTable;

	// 전역 문자열을 초기화합니다.
	LoadString(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadString(hInstance, IDC_ANEMONE, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);

	// Heap 생성 (1MB)
	AneHeap = HeapCreate(0, 1024 * 1024, 0);

	// 응용 프로그램 초기화를 수행합니다.
	if (!InitInstance(hInstance, false))
	{
		return FALSE;
	}
	hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_ANEMONE));

	std::wstring szEnginePath;

	if (!GetEZTPathFromREG(szEnginePath))
	{
		MessageBox(0, L"이지트랜스가 설치되지 않았거나 레지스트리에서 이지트랜스 경로를 찾을 수 없습니다.\r\n이지트랜스가 설치되어 있어야 아네모네 실행이 가능합니다.\r\n이지트랜스가 설치되어 있다면 INI 파일의 이지트랜스 경로를 설정해 주세요.", 0, MB_ICONERROR);
		return false;
	}

	// 번역엔진 초기화
	Cl.TransEngine = new CTransEngine();

	if (!Cl.TransEngine->Init(szEnginePath))
	{
		MessageBox(0, L"이지트랜스 엔진 초기화가 실패했습니다.", 0, MB_ICONERROR);
		return false;
	}
	/*
	char buf[1024];
	char *n_str;

	// 가끔 번역실패하는 문제가 있어서 세번 돌림
	for (int k = 0; k < 3; k++)
	{
		n_str = (char *)TransEngine.J2K_TranslateMMNT(0, "u??????xQl??b????????????v");
		if (n_str[0] != 0x00) break;
	}

	if (n_str)
	{
		strcpy_s(buf, n_str);
		TransEngine.J2K_FreeMem(n_str);
		MessageBoxA(0, buf, 0, 0);
	}
	*/
	
	// 렌더러 초기화
	Cl.TextRenderer = new CTextRenderer();
	if (!Cl.TextRenderer->Init())
	{
		MessageBox(0, L"GDI+ 엔진 초기화를 실패했습니다.\r\n시스템의 리소스가 부족할 경우 GDI+ 초기화가 실패할 수 있습니다.\r\n실행되고 있는 다른 프로그램들을 종료하고 다시 실행하세요.", 0, MB_ICONERROR);
		return false;
	}

	// 텍스트 처리 클래스
	Cl.TextProcess = new CTextProcess();

	// 단축키 클래스
	Cl.Hotkey = new CHotkey();
	
	IsActive = false;
	Cl.TextRenderer->Paint();

	// 윈도우 표시
	ShowWindow(hWnds.Main, true);

	// 기본 메시지 루프입니다.
	while (GetMessage(&msg, NULL, 0, 0))
	{
		if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	ShowWindow(hWnds.Main, false);

	delete Cl.TransEngine;
	delete Cl.TextRenderer;
	delete Cl.TextProcess;
	delete Cl.Hotkey;

	// Heap 삭제
	HeapDestroy(AneHeap);

	return (int) msg.wParam;
}



//
//  함수: MyRegisterClass()
//
//  목적: 창 클래스를 등록합니다.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEX wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style			= CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc	= WndProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= hInstance;
	wcex.hIcon			= LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ANEMONE));
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
	wcex.lpszMenuName	= 0;
	wcex.lpszClassName	= szWindowClass;
	wcex.hIconSm		= LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

	return RegisterClassEx(&wcex);
}

//
//   함수: InitInstance(HINSTANCE, int)
//
//   목적: 인스턴스 핸들을 저장하고 주 창을 만듭니다.
//
//   설명:
//
//        이 함수를 통해 인스턴스 핸들을 전역 변수에 저장하고
//        주 프로그램 창을 만든 다음 표시합니다.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   hInst = hInstance; // 인스턴스 핸들을 전역 변수에 저장합니다.

   int cx = GetSystemMetrics(SM_CXSCREEN);
   int cy = GetSystemMetrics(SM_CYSCREEN);

   int x = 300;
   int y = 200;

   hWnds.Main = CreateWindowEx(WS_EX_TOOLWINDOW | WS_EX_NOACTIVATE | WS_EX_LAYERED | WS_EX_TOPMOST, szWindowClass, szTitle, WS_POPUP,
      (cx-x)/2, (cy-y)/2, x, y, NULL, NULL, hInstance, NULL);

   if (!hWnds.Main)
   {
      return FALSE;
   }

   ShowWindow(hWnds.Main, nCmdShow);
   UpdateWindow(hWnds.Main);
   
   return TRUE;
}

//
//  함수: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  목적:  주 창의 메시지를 처리합니다.
//
//  WM_COMMAND	- 응용 프로그램 메뉴를 처리합니다.
//  WM_PAINT	- 주 창을 그립니다.
//  WM_DESTROY	- 종료 메시지를 게시하고 반환합니다.
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
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
		// 메뉴 선택을 구문 분석합니다.
		switch (wmId)
		{
		case IDM_ABOUT:
			DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
			break;
		case IDM_EXIT:
			DestroyWindow(hWnd);
			break;
		case IDM_TERMINATE_ANEMONE:
			PostQuitMessage(0);
			break;
			/*
		case IDM_WINDOW_SETTING:
			if (IsWindow(hWnds.Setting) == false)
			{
				RECT rect;
				int width = 630;
				int height = 650;

				hWnds.Setting = CreateWindowEx(WS_EX_TOOLWINDOW | WS_EX_TOPMOST | WS_EX_NOACTIVATE, szSettingWndClass, NULL, WS_BORDER,
					CW_USEDEFAULT, 0, width, height,
					hWnd, (HMENU)NULL, hInst, NULL);

				int cx = GetSystemMetrics(SM_CXSCREEN);
				int cy = GetSystemMetrics(SM_CYSCREEN);
				GetWindowRect(hWnds.Setting, &rect);

				SetWindowPos(hWnds.Setting, 0, ((cx - (rect.right - rect.left)) / 2), ((cy - (rect.bottom - rect.top)) / 2), 0, 0, SWP_NOSIZE | SWP_NOZORDER);

				int nWndStyle = GetWindowLong(hWnds.Setting, GWL_STYLE);

				nWndStyle &= ~WS_CAPTION;
				nWndStyle |= WS_BORDER;

				SetWindowLong(hWnds.Setting, GWL_STYLE, nWndStyle);

				ShowWindow(hWnds.Setting, 1);
			}
			else
			{
				DestroyWindow(hWnds.Setting);
				hWnds.Setting = NULL;
			}
			break;*/
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
		break;
	case WM_PAINT:
	{
		Cl.TextRenderer->Paint();
	}
		break;
	case WM_MOVING:
	case WM_SIZING:
	{
		RECT *prc = (RECT *)lParam;
		SetWindowPos(hWnd, NULL, prc->left, prc->top, prc->right - prc->left, prc->bottom - prc->top, 0);
		if (message == WM_SIZING) Cl.TextRenderer->Paint();
	}
		break;
	case WM_DRAWCLIPBOARD:
	{
		Cl.TextProcess->OnDrawClipboard();
	}
		break;
	case WM_LBUTTONDOWN:
		SendMessage(hWnd, WM_NCLBUTTONDOWN, HTCAPTION, 0);
		break;
	case WM_ERASEBKGND:
		return false;
	case WM_GETMINMAXINFO:
	{
		MINMAXINFO *mm = (MINMAXINFO *)lParam;

		mm->ptMinTrackSize.x = 90;
		mm->ptMinTrackSize.y = 90;
	}
		break;
	case WM_NCHITTEST:
	{
		POINT pt;
		RECT rc;
		GetClientRect(hWnd, &rc);
		pt.x = LOWORD(lParam);
		pt.y = HIWORD(lParam);
		ScreenToClient(hWnd, &pt);

		int BORDERWIDTH = 30;

		/*top-left, top and top-right*/
		if (pt.y<BORDERWIDTH)
		{
			if (pt.x<BORDERWIDTH)
			{
				return HTTOPLEFT;
			}
			else if (pt.x>(rc.right - BORDERWIDTH))
			{
				return HTTOPRIGHT;
			}    return HTTOP;
		}
		/*bottom-left, bottom and bottom-right */
		if (pt.y>(rc.bottom - BORDERWIDTH))
		{
			if (pt.x<BORDERWIDTH)
			{
				return HTBOTTOMLEFT;
			}
			else if (pt.x>(rc.right - BORDERWIDTH))
			{
				return HTBOTTOMRIGHT;
			}    return HTBOTTOM;
		}  if (pt.x<BORDERWIDTH)
		{
			return HTLEFT;
		}  if (pt.x>(rc.right - BORDERWIDTH))
		{
			return HTRIGHT;
		}  return DefWindowProc(hWnd, message, wParam, lParam);
	}
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

// 정보 대화 상자의 메시지 처리기입니다.
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);
	switch (message)
	{
	case WM_INITDIALOG:
		return (INT_PTR)TRUE;

	case WM_COMMAND:
		if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
		{
			EndDialog(hDlg, LOWORD(wParam));
			return (INT_PTR)TRUE;
		}
		break;
	}
	return (INT_PTR)FALSE;
}
