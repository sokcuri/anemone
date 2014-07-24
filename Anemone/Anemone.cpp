// Anemone.cpp : 응용 프로그램에 대한 진입점을 정의합니다.
//

#include "stdafx.h"
#include "Anemone.h"

#define MAX_LOADSTRING 100


// 전역 변수:
TCHAR szTitle[MAX_LOADSTRING];					// 제목 표시줄 텍스트입니다.
TCHAR szWindowClass[MAX_LOADSTRING];			// 기본 창 클래스 이름입니다.
TCHAR szSettingClass[MAX_LOADSTRING];			// 기본 창 클래스 이름입니다.
TCHAR szParentClass[MAX_LOADSTRING];			// 기본 창 클래스 이름입니다.
std::vector<_key_map> key_map;
HINSTANCE hInst; _hWnds hWnds; _MagnetWnd MagnetWnd; _Class Cl; HANDLE AneHeap;
int IsActive = 0;
int Elapsed_Prepare = 0;
int Elapsed_Translate = 0;
//bool IsActive = FALSE;

// 이 코드 모듈에 들어 있는 함수의 정방향 선언입니다.
ATOM				MainWndClassRegister(HINSTANCE hInstance);
ATOM				SettingClassRegister(HINSTANCE hInstance);
ATOM				ParentClassRegister(HINSTANCE hInstance);
BOOL				InitInstance(HINSTANCE, int);
LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK	ParentWndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK	SettingProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK	About(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK	TransWinProc(HWND, UINT, WPARAM, LPARAM);

unsigned int WINAPI MagneticThread(void *arg);

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
	wcscpy(szSettingClass, L"Anemone_Setting_Class");
	wcscpy(szParentClass, L"AneParentClass");

	MainWndClassRegister(hInstance);
	ParentClassRegister(hInstance);
	SettingClassRegister(hInstance);

	// Heap 생성 (1MB)
	AneHeap = HeapCreate(0, 1024 * 1024, 0);

	// 환경설정 클래스
	Cl.Config = new CConfig();
	Cl.Config->LoadConfig();

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

	// 파일 감시 쓰레드
	Cl.FileWatch = new CFileWatch();

	// 리모콘 클래스
	Cl.Remocon = new CRemocon();
	if (!Cl.Remocon->CreateInstance())
	{
		MessageBox(0, L"아네모네 리모콘 초기화가 실패했습니다.", 0, MB_ICONERROR);
	}
		
	//ShowWindow(hWnds.Remocon, true);

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

	return (int) msg.wParam;
}

void CleanUp()
{
	Cl.Config->SaveConfig();
	ShowWindow(hWnds.Main, false);

	DestroyWindow(hWnds.Parent);

	// 설정 저장
	Cl.Config->SaveConfig();

	delete Cl.Config;
	delete Cl.TransEngine;
	delete Cl.TextRenderer;
	delete Cl.TextProcess;
	delete Cl.Hotkey;
	delete Cl.FileWatch;
	delete Cl.Remocon;

	// Heap 삭제
	HeapDestroy(AneHeap);

	ExitProcess(0);
}

//
// 자석모드 쓰레드
//

unsigned int WINAPI MagneticThread(void *arg)
{
	bool IsForegroundCheck = false;
	bool IsMinimizeOnce = false;
	RECT rect;
	//int nExStyle_Main, nExStyle_Target;

	while (1)
	{
		//if (GetForegroundWindow() != GetActiveWindow())
		//	SetWindowPos(hWnds.Parent, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE | SWP_NOACTIVATE);
		if (!IsWindow(hWnds.Parent))
		{
			MessageBox(0, L"Parent 죽음", 0, 0);
		}


		if (IsWindow(MagnetWnd.hWnd) && MagnetWnd.IsMagnet)
		{
			if (Cl.Config->GetMagneticMode())
			{
				GetWindowRect(MagnetWnd.hWnd, &rect);

				// 자석 타겟창에 맞춰서 항상위 변경
				int nExStyle_Main = GetWindowLong(hWnds.Parent, GWL_EXSTYLE);
				int nExStyle_Target = GetWindowLong(MagnetWnd.hWnd, GWL_EXSTYLE);

				if ((nExStyle_Target & WS_EX_TOPMOST) != (nExStyle_Main & WS_EX_TOPMOST))
				{
					SetWindowPos(hWnds.Parent, (nExStyle_Target & WS_EX_TOPMOST ? HWND_TOPMOST : HWND_NOTOPMOST), 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE);
					SetWindowPos(hWnds.Main, (nExStyle_Target & WS_EX_TOPMOST ? HWND_TOPMOST : HWND_NOTOPMOST), 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE);
				}

				if (GetWindowLong(MagnetWnd.hWnd, GWL_STYLE) & WS_MINIMIZE)
				{
					if (IsMinimizeOnce == false)
					{
						IsMinimizeOnce = true;

						if (Cl.Config->GetMagneticMinimize())
						{
							MagnetWnd.IsMinimize = true;
							Cl.Config->SetWindowVisible(false);
							ShowWindow(hWnds.Main, false);
							SendMessage(hWnds.Main, WM_COMMAND, IDM_SETTING_CHECK, 0);
						}
					}
				}
				else 
				{
					if (IsMinimizeOnce)
					{
						if (MagnetWnd.IsMinimize == true)
						{
							Cl.Config->SetWindowVisible(true);
							ShowWindow(hWnds.Main, true);
						}
						MagnetWnd.IsMinimize = false;
						IsMinimizeOnce = false;
						SendMessage(hWnds.Main, WM_COMMAND, IDM_SETTING_CHECK, 0);
					}
				}

				if (MagnetWnd.rect_x != rect.left || MagnetWnd.rect_y != rect.top)
				{
					if (!(GetWindowLong(MagnetWnd.hWnd, GWL_STYLE) & WS_MINIMIZE))
					{
						DEVMODE dmCurrent, dmRegistry;

						EnumDisplaySettings(NULL, ENUM_CURRENT_SETTINGS, &dmCurrent);
						EnumDisplaySettings(NULL, ENUM_REGISTRY_SETTINGS, &dmRegistry);

						if (dmCurrent.dmPelsWidth == dmRegistry.dmPelsWidth &&
							dmCurrent.dmPelsHeight == dmRegistry.dmPelsHeight)
						{
							SetWindowPos(hWnds.Main, NULL, rect.left + MagnetWnd.diff_x, rect.top + MagnetWnd.diff_y, 0, 0, SWP_NOSIZE | SWP_NOOWNERZORDER);
						}

						MagnetWnd.rect_x = rect.left;
						MagnetWnd.rect_y = rect.top;
					}
				}

				// 자석모드 윈도우 활성화시 아네모네도 최상위로 띄운다
				if (GetForegroundWindow() == MagnetWnd.hWnd)
				{
					if (!IsForegroundCheck)
					{
						IsForegroundCheck = true;
						Sleep(20);
						SetWindowPos(hWnds.Parent, HWND_TOP, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE | SWP_NOACTIVATE);
						SetWindowPos(hWnds.Main, HWND_TOP, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE | SWP_NOACTIVATE);

						RECT rectx;

						GetWindowRect(MagnetWnd.hWnd, &rectx);

						SetWindowPos(MagnetWnd.hWnd, HWND_TOP, rectx.left, rectx.top, rectx.right-rectx.left, rectx.bottom-rectx.top, SWP_SHOWWINDOW);
					}
				}
				else IsForegroundCheck = false;
			}
		}
		else if (MagnetWnd.IsMagnet)
		{
			if (MagnetWnd.IsMinimize)
			{
				Cl.Config->SetWindowVisible(true);
				ShowWindow(hWnds.Main, true);
				MagnetWnd.IsMinimize = false;
			}
			MagnetWnd.hWnd = NULL;
			SetParent(hWnds.Parent, NULL);
			Cl.Config->SetMagneticMode(false);

			MagnetWnd.IsMagnet = false;

			if (Cl.Config->GetWindowTopMost())
			{
				SetWindowPos(hWnds.Main, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE);
				SetWindowPos(hWnds.Parent, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE);
			}
			else
			{
				SetWindowPos(hWnds.Main, HWND_NOTOPMOST, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE);
				SetWindowPos(hWnds.Parent, HWND_NOTOPMOST, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE);
			}

			Sleep(20);

			SetWindowPos(hWnds.Parent, HWND_TOP, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE);

			SendMessage(hWnds.Main, WM_COMMAND, IDM_SETTING_CHECK, 0);
		}
		Sleep(10);
	}
	return 0;
}

//
// 우클릭 메뉴
//

VOID APIENTRY DisplayContextMenu(HWND hwnd, POINT pt)
{
	HMENU hmenu;            // top-level menu 
	HMENU hmenuTrackPopup;  // shortcut menu 

	// Load the menu resource. 

	if ((hmenu = LoadMenu(hInst, MAKEINTRESOURCE(IDC_ANEMONE))) == NULL)
		return;

	// TrackPopupMenu cannot display the menu bar so get 
	// a handle to the first shortcut menu. 

	hmenuTrackPopup = GetSubMenu(hmenu, 0);

	// Display the shortcut menu. Track the right mouse 
	// button. 

	CheckMenuItem(hmenuTrackPopup, IDM_CLIPBOARD_SWITCH, (Cl.Config->GetClipSwitch() ? MF_CHECKED : MF_UNCHECKED));
	CheckMenuItem(hmenuTrackPopup, IDM_WND_BORDER_MODE, (Cl.Config->GetWndBorderMode() ? MF_CHECKED : MF_UNCHECKED));
	CheckMenuItem(hmenuTrackPopup, IDM_WINDOW_THROUGH_CLICK, (Cl.Config->GetClickThough() ? MF_CHECKED : MF_UNCHECKED));

	TrackPopupMenu(hmenuTrackPopup,
		TPM_LEFTALIGN | TPM_RIGHTBUTTON,
		pt.x, pt.y, 0, hwnd, NULL);


	// Destroy the menu. 

	DestroyMenu(hmenu);
}

BOOL WINAPI OnContextMenu(HWND hwnd, int x, int y)
{
	RECT rc;                    // client area of window 
	POINT pt = { x, y };        // location of mouse click 

	// Get the bounding rectangle of the client area. 

	GetClientRect(hwnd, &rc);

	// Convert the mouse position to client coordinates. 

	ScreenToClient(hwnd, &pt);

	// If the position is in the client area, display a  
	// shortcut menu. 

	if (PtInRect(&rc, pt))
	{
		ClientToScreen(hwnd, &pt);
		DisplayContextMenu(hwnd, pt);
		return TRUE;
	}

	// Return FALSE if no menu is displayed. 

	return FALSE;
}
//
// 윈도우 클래스 등록
//
ATOM MainWndClassRegister(HINSTANCE hInstance)
{
	WNDCLASSEX wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;
	wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ANEMONE));
	wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wcex.lpszMenuName = 0;
	wcex.lpszClassName = szWindowClass;
	wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

	return RegisterClassEx(&wcex);
}

ATOM ParentClassRegister(HINSTANCE hInstance)
{
	WNDCLASSEX wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = ParentWndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;
	wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ANEMONE));
	wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wcex.lpszMenuName = 0;
	wcex.lpszClassName = szParentClass;
	wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

	return RegisterClassEx(&wcex);
}

ATOM SettingClassRegister(HINSTANCE hInstance)
{
	WNDCLASSEX wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style = CS_HREDRAW | CS_VREDRAW;
	//wcex.lpfnWndProc = SettingProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;
	wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ANEMONE));
	wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wcex.lpszMenuName = 0;
	wcex.lpszClassName = szSettingClass;
	wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

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

   int x = 500;
   int y = 200;

   hWnds.Parent = CreateWindowEx(WS_EX_TOOLWINDOW | WS_EX_TOPMOST, szParentClass, L"아네모네", WS_POPUP,
	   0, 0, 0, 0, NULL, NULL, hInstance, NULL);

   hWnds.Main = CreateWindowEx(WS_EX_TOOLWINDOW | WS_EX_NOACTIVATE | WS_EX_LAYERED | WS_EX_TOPMOST, szWindowClass, szTitle, WS_POPUP,
      (cx-x)/2, (cy-y)/2, x, y, hWnds.Parent, NULL, hInstance, NULL);

   HANDLE hThread = NULL;
   DWORD dwThreadID = NULL;
   MagnetWnd.hThread = (HANDLE)_beginthreadex(NULL, 0, MagneticThread, NULL, 0, (unsigned*)&dwThreadID);
   if (MagnetWnd.hThread == 0)
   {
	   MessageBox(0, L"아네모네 쓰레드 생성 실패", 0, 0);
	   return FALSE;
   }

   if (!hWnds.Parent || !hWnds.Main)
   {
	  MessageBox(0, L"아네모네 초기화 실패", 0, 0);
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
		case IDM_TERMINATE_ANEMONE:
		case WM_DESTROY:
			CleanUp();
			break;
		case IDM_TEMP_CLICK_THOUGH:
			(Cl.Config->GetClickThough() ? Cl.Config->SetClickThough(false) : Cl.Config->SetClickThough(true));

			if (Cl.Config->GetClickThough())
			{
				int nExStyle = GetWindowLong(hWnd, GWL_EXSTYLE);
				nExStyle |= WS_EX_TRANSPARENT;
				SetWindowLong(hWnd, GWL_EXSTYLE, nExStyle);
			}
			else
			{
				int nExStyle = GetWindowLong(hWnd, GWL_EXSTYLE);
				nExStyle &= ~WS_EX_TRANSPARENT;
				SetWindowLong(hWnd, GWL_EXSTYLE, nExStyle);
			}
			SendMessage(hWnds.Main, WM_COMMAND, IDM_SETTING_CHECK, 0);
			break;
		case IDM_TEMP_WINDOW_HIDE:
		{
			(Cl.Config->GetTempWinHide() ? Cl.Config->SetTempWinHide(false) : (Cl.Config->GetWindowVisible() ? Cl.Config->SetTempWinHide(true) : Cl.Config->SetTempWinHide(false)));
			if (Cl.Config->GetTempWinHide())
			{
				ShowWindow(hWnd, false);
			}
			else
			{
				ShowWindow(hWnd, true);
			}
		}
			break;
		case IDM_WINDOW_VISIBLE:
		{
			(Cl.Config->GetWindowVisible() ? Cl.Config->SetWindowVisible(false) : Cl.Config->SetWindowVisible(true));
			if (!Cl.Config->GetWindowVisible())
			{
				ShowWindow(hWnd, false);
			}
			else
			{
				ShowWindow(hWnd, true);
			}
		}
			break;
		case IDM_HIDEWIN_UNLOCK_HOTKEY:
		{
			(Cl.Config->GetHideWinUnlockHotkey() ? Cl.Config->SetHideWinUnlockHotkey(false) : Cl.Config->SetHideWinUnlockHotkey(true));
		}
			break;
		case IDM_HIDEWIN_UNWATCH_CLIPBOARD:
		{
			(Cl.Config->GetHideWinUnWatchClip() ? Cl.Config->SetHideWinUnWatchClip(false) : Cl.Config->SetHideWinUnWatchClip(true));
		}
			break;
		case IDM_MAGNETIC_MODE:
		{
			// 아네모네 윈도우는 자석 모드가 걸리지 않음, 창 숨김 모드일때는 푸는것만 가능함
			if (GetForegroundWindow() == GetActiveWindow() || !Cl.Config->GetWindowVisible())
			{
				Cl.Config->SetMagneticMode(false);
				MagnetWnd.hWnd = NULL;
				break;
			}

			RECT rect_main, rect_target;
			(Cl.Config->GetMagneticMode() ? Cl.Config->SetMagneticMode(false) : Cl.Config->SetMagneticMode(true));

			if (Cl.Config->GetMagneticMode())
			{
				DEVMODE dmCurrent;
				EnumDisplaySettings(NULL, ENUM_CURRENT_SETTINGS, &dmCurrent);

				MagnetWnd.hWnd = GetForegroundWindow();

				MagnetWnd.res_x = dmCurrent.dmPelsWidth;
				MagnetWnd.res_y = dmCurrent.dmPelsHeight;
				MagnetWnd.res_c = dmCurrent.dmBitsPerPel;

				GetWindowRect(hWnds.Main, &rect_main);
				GetWindowRect(MagnetWnd.hWnd, &rect_target);

				MagnetWnd.diff_x = rect_main.left - rect_target.left;
				MagnetWnd.diff_y = rect_main.top - rect_target.top;

				MagnetWnd.rect_x = rect_target.left;
				MagnetWnd.rect_y = rect_target.top;

				// 창이 뒤에 있을때 가려지므로 이 문제를 보완
				SetWindowPos(hWnds.Parent, HWND_TOP, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE);
				SetWindowPos(hWnds.Main, HWND_TOP, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE);

				SetParent(hWnds.Parent, MagnetWnd.hWnd);
				MagnetWnd.IsMagnet = true;
			}
			else
			{
				MagnetWnd.hWnd = NULL;
			}
		}
			break;
		case IDM_WND_BORDER_MODE:
			(Cl.Config->GetWndBorderMode() ? Cl.Config->SetWndBorderMode(false) : Cl.Config->SetWndBorderMode(true));
			SendMessage(hWnds.Main, WM_COMMAND, IDM_SETTING_CHECK, 0);
			PostMessage(hWnds.Main, WM_PAINT, 0, 0);
			break;
		case IDM_CLIPBOARD_SWITCH:
		{
			(Cl.Config->GetClipSwitch() ? Cl.Config->SetClipSwitch(false) : Cl.Config->SetClipSwitch(true));

			if (Cl.Config->GetClipSwitch())
			{
				Cl.TextProcess->StartWatchClip();
			}
			else
			{
				Cl.TextProcess->EndWatchClip();
			}

			SendMessage(hWnds.Main, WM_COMMAND, IDM_SETTING_CHECK, 0);
		}
			break;
		case IDM_TEXT_PREV:
			break;
		case IDM_TEXT_NEXT:
			break;
		case IDM_OPENDIC:
			ExecuteFile(L".\\AneDic.txt");
			break;
		case IDM_OPENINI:
			ExecuteFile(L".\\Anemone.ini");
			break;
		case IDM_TOPMOST:
			(Cl.Config->GetWindowTopMost() ? Cl.Config->SetWindowTopMost(false) : Cl.Config->SetWindowTopMost(true));

			// 자석 모드일때는 항상 위 표시 동작을 유보한다
			if (Cl.Config->GetMagneticMode()) break;

			if (Cl.Config->GetWindowTopMost())
			{
				SetWindowPos(hWnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE);
			}
			else
			{
				SetWindowPos(hWnd, HWND_NOTOPMOST, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE);
			}
			break;
		case IDM_WINDOW_SETTING:
		{
			if (IsWindow(hWnds.Setting) == false)
			{
				RECT rect;
				int cx = GetSystemMetrics(SM_CXSCREEN);
				int cy = GetSystemMetrics(SM_CYSCREEN);

				hWnds.Setting = CreateDialog(hInst, MAKEINTRESOURCE(IDD_SETTING), hWnd, SettingProc);
			
				GetWindowRect(hWnds.Setting, &rect);

				SetWindowPos(hWnds.Setting, 0, (cx - rect.right + rect.left) / 2, (cy - rect.bottom + rect.top) / 2, 0, 0, SWP_NOSIZE);
				ShowWindow(hWnds.Setting, 1);
			}
			else
			{
				DestroyWindow(hWnds.Setting);
				hWnds.Setting = NULL;
				break;
			}
		}
		case IDM_SETTING_CHECK:
		{
			if (IsWindow(hWnds.Setting))
			{
				// 체크박스
				CheckDlgButton(hWnds.Setting, IDC_SETTING_TOPMOST, Cl.Config->GetWindowTopMost());
				CheckDlgButton(hWnds.Setting, IDC_SETTING_MAGNETIC_MINIMIZE, Cl.Config->GetMagneticMinimize());
				CheckDlgButton(hWnds.Setting, IDC_SETTING_HIDEWIN, !Cl.Config->GetWindowVisible());
				CheckDlgButton(hWnds.Setting, IDC_SETTING_HIDEWIN_UNWATCH_CLIPBOARD, Cl.Config->GetHideWinUnWatchClip());
				CheckDlgButton(hWnds.Setting, IDC_SETTING_HIDEWIN_UNLOCK_HOTKEY, Cl.Config->GetHideWinUnlockHotkey());
				CheckDlgButton(hWnds.Setting, IDC_SETTING_CLIPBOARD_WATCH, Cl.Config->GetClipSwitch());
				CheckDlgButton(hWnds.Setting, IDC_SETTING_WNDCLICK_THOUGH, Cl.Config->GetClickThough());
				CheckDlgButton(hWnds.Setting, IDC_SETTING_USE_MAGNETIC, Cl.Config->GetMagneticMode());
				CheckDlgButton(hWnds.Setting, IDC_SETTING_WND_BORDER_MODE, Cl.Config->GetWndBorderMode());
				CheckDlgButton(hWnds.Setting, IDC_SETTING_PRINT_ORGTEXT, Cl.Config->GetTextSwitch(CFG_ORG));
				CheckDlgButton(hWnds.Setting, IDC_SETTING_PRINT_ORGNAME, Cl.Config->GetTextSwitch(CFG_NAME_ORG));
				CheckDlgButton(hWnds.Setting, IDC_SETTING_SEPERATE_NAME, Cl.Config->GetTextSwitch(CFG_NAME));

				{
					(Cl.Config->GetRepeatTextProc() == 0) ? CheckDlgButton(hWnds.Setting, IDC_SETTING_REPEAT_TEXT, false) :
						CheckDlgButton(hWnds.Setting, IDC_SETTING_REPEAT_TEXT, true);

					std::wstringstream ws;
					std::wstring str;

					ws << L"반복 문자 처리";
					if (Cl.Config->GetRepeatTextProc() == 1)
						ws << L"(미)";
					else if (Cl.Config->GetRepeatTextProc() == 2)
						ws << L"(약)";
					else if (Cl.Config->GetRepeatTextProc() == 3)
						ws << L"(중)";
					else if (Cl.Config->GetRepeatTextProc() == 4)
						ws << L"(강)";
					str = ws.str();
					SetDlgItemTextW(hWnds.Setting, IDC_SETTING_REPEAT_TEXT, str.c_str());
				}


				CheckDlgButton(hWnds.Setting, IDC_SETTING_TEXTEND_NAME, Cl.Config->GetReviseName());
				CheckDlgButton(hWnds.Setting, IDC_SETTING_FORCE_ANEDIC, Cl.Config->GetForceAneDic());
				CheckDlgButton(hWnds.Setting, IDC_SETTING_ANE_REMOCON, Cl.Config->GetRemoconMode());
				CheckDlgButton(hWnds.Setting, IDC_SETTING_EXTERN_HOTKEY, Cl.Config->GetExternHotkey());

				CheckDlgButton(hWnds.Setting, IDC_SETTING_TEXTALIGN_LEFT, false);
				CheckDlgButton(hWnds.Setting, IDC_SETTING_TEXTALIGN_MID, false);
				CheckDlgButton(hWnds.Setting, IDC_SETTING_TEXTALIGN_RIGHT, false);

				switch (Cl.Config->GetTextAlign())
				{
				case 0:
				{
					CheckDlgButton(hWnds.Setting, IDC_SETTING_TEXTALIGN_LEFT, true);
				}
					break;
				case 1:
				{
					CheckDlgButton(hWnds.Setting, IDC_SETTING_TEXTALIGN_MID, true);
				}
					break;
				case 2:
				{
					CheckDlgButton(hWnds.Setting, IDC_SETTING_TEXTALIGN_RIGHT, true);
				}
					break;
				}

				CheckDlgButton(hWnds.Setting, IDC_SETTING_BACKGROUND_SWITCH, Cl.Config->GetBGSwitch());
				
				{
					int nBGAlpha = (Cl.Config->GetBGColor() >> 24) & 0xFF;

					SendDlgItemMessage(hWnds.Setting, IDC_SETTING_BACKGROUND_TRACKBAR, TBM_SETRANGE, (WPARAM)TRUE, (LPARAM)MAKELONG(0, 255));
					SendDlgItemMessage(hWnds.Setting, IDC_SETTING_BACKGROUND_TRACKBAR, TBM_SETPOS, (WPARAM)TRUE, (LPARAM)nBGAlpha);

					std::wstringstream ws;
					std::wstring str;

					ws << L"투명도 설정";
					ws << L" (";
					ws << nBGAlpha;
					ws << L")";
					str = ws.str();
					SetDlgItemTextW(hWnds.Setting, IDC_SETTING_BACKGROUND_TEXT, str.c_str());
				}

				{
					int nTextSize = (Cl.Config->GetTextSize(CFG_TRANS, CFG_A));
					SendDlgItemMessage(hWnds.Setting, IDC_SETTING_TEXTSIZE_TRACKBAR, TBM_SETRANGE, (WPARAM)TRUE, (LPARAM)MAKELONG(6, 100));
					SendDlgItemMessage(hWnds.Setting, IDC_SETTING_TEXTSIZE_TRACKBAR, TBM_SETPOS, (WPARAM)TRUE, (LPARAM)nTextSize);

					std::wstringstream ws;
					std::wstring str;

					ws << L"글자 크기";
					ws << L" (";
					ws << nTextSize;
					ws << L")";
					str = ws.str();
					SetDlgItemTextW(hWnds.Setting, IDC_SETTING_TEXTSIZE_TEXT, str.c_str());
				}

				{
					int nOutlineSize = (Cl.Config->GetTextSize(CFG_TRANS, CFG_B));
					SendDlgItemMessage(hWnds.Setting, IDC_SETTING_OUTLINE1_SIZE_TRACKBAR, TBM_SETRANGE, (WPARAM)TRUE, (LPARAM)MAKELONG(0, 20));
					SendDlgItemMessage(hWnds.Setting, IDC_SETTING_OUTLINE1_SIZE_TRACKBAR, TBM_SETPOS, (WPARAM)TRUE, (LPARAM)nOutlineSize);

					std::wstringstream ws;
					std::wstring str;

					ws << L"외곽선 굵기";
					ws << L" (";
					ws << nOutlineSize;
					ws << L")";
					str = ws.str();
					SetDlgItemTextW(hWnds.Setting, IDC_SETTING_OUTLINE1_SIZE_TEXT, str.c_str());
				}

				{
					int nOutlineSize = (Cl.Config->GetTextSize(CFG_TRANS, CFG_C));
					SendDlgItemMessage(hWnds.Setting, IDC_SETTING_OUTLINE2_SIZE_TRACKBAR, TBM_SETRANGE, (WPARAM)TRUE, (LPARAM)MAKELONG(0, 20));
					SendDlgItemMessage(hWnds.Setting, IDC_SETTING_OUTLINE2_SIZE_TRACKBAR, TBM_SETPOS, (WPARAM)TRUE, (LPARAM)nOutlineSize);

					std::wstringstream ws;
					std::wstring str;

					ws << L"외곽선 굵기";
					ws << L" (";
					ws << nOutlineSize;
					ws << L")";
					str = ws.str();
					SetDlgItemTextW(hWnds.Setting, IDC_SETTING_OUTLINE2_SIZE_TEXT, str.c_str());
				}

				CheckDlgButton(hWnds.Setting, IDC_SETTING_NAME_SHADOW, Cl.Config->GetTextShadow(CFG_NAME));
				CheckDlgButton(hWnds.Setting, IDC_SETTING_ORG_SHADOW, Cl.Config->GetTextShadow(CFG_ORG));
				CheckDlgButton(hWnds.Setting, IDC_SETTING_TRANS_SHADOW, Cl.Config->GetTextShadow(CFG_TRANS));

				{
					int n = Cl.Config->GetShadowX();

					SendDlgItemMessage(hWnds.Setting, IDC_SETTING_SHADOW_X_TRACKBAR, TBM_SETRANGE, (WPARAM)TRUE, (LPARAM)MAKELONG(0, 20));
					SendDlgItemMessage(hWnds.Setting, IDC_SETTING_SHADOW_X_TRACKBAR, TBM_SETPOS, (WPARAM)TRUE, (LPARAM)n);

					std::wstringstream ws;
					std::wstring str;

					ws << L"그림자 X축";
					ws << L" (";
					ws << n-10;
					ws << L")";
					str = ws.str();
					SetDlgItemTextW(hWnds.Setting, IDC_SETTING_SHADOW_X_TEXT, str.c_str());
				}

				{
					int n = Cl.Config->GetShadowY();

					SendDlgItemMessage(hWnds.Setting, IDC_SETTING_SHADOW_Y_TRACKBAR, TBM_SETRANGE, (WPARAM)TRUE, (LPARAM)MAKELONG(0, 20));
					SendDlgItemMessage(hWnds.Setting, IDC_SETTING_SHADOW_Y_TRACKBAR, TBM_SETPOS, (WPARAM)TRUE, (LPARAM)n);

					std::wstringstream ws;
					std::wstring str;

					ws << L"그림자 Y축";
					ws << L" (";
					ws << n-10;
					ws << L")";
					str = ws.str();
					SetDlgItemTextW(hWnds.Setting, IDC_SETTING_SHADOW_Y_TEXT, str.c_str());
				}

				{
					int n = Cl.Config->GetTextMarginX();

					SendDlgItemMessage(hWnds.Setting, IDC_SETTING_TEXTMARGIN_X_TRACKBAR, TBM_SETRANGE, (WPARAM)TRUE, (LPARAM)MAKELONG(0, 300));
					SendDlgItemMessage(hWnds.Setting, IDC_SETTING_TEXTMARGIN_X_TRACKBAR, TBM_SETPOS, (WPARAM)TRUE, (LPARAM)n);

					std::wstringstream ws;
					std::wstring str;

					ws << L"좌우 간격";
					ws << L" (";
					ws << n;
					ws << L")";
					str = ws.str();
					SetDlgItemTextW(hWnds.Setting, IDC_SETTING_TEXTMARGIN_X_TEXT, str.c_str());
				}

				{
					int n = Cl.Config->GetTextMarginY();

					SendDlgItemMessage(hWnds.Setting, IDC_SETTING_TEXTMARGIN_Y_TRACKBAR, TBM_SETRANGE, (WPARAM)TRUE, (LPARAM)MAKELONG(0, 300));
					SendDlgItemMessage(hWnds.Setting, IDC_SETTING_TEXTMARGIN_Y_TRACKBAR, TBM_SETPOS, (WPARAM)TRUE, (LPARAM)n);

					std::wstringstream ws;
					std::wstring str;

					ws << L"상하 간격";
					ws << L" (";
					ws << n;
					ws << L")";
					str = ws.str();
					SetDlgItemTextW(hWnds.Setting, IDC_SETTING_TEXTMARGIN_Y_TEXT, str.c_str());
				}

				{
					int n = Cl.Config->GetTextMarginName();

					SendDlgItemMessage(hWnds.Setting, IDC_SETTING_TEXTMARGIN_NAME_TRACKBAR, TBM_SETRANGE, (WPARAM)TRUE, (LPARAM)MAKELONG(0, 300));
					SendDlgItemMessage(hWnds.Setting, IDC_SETTING_TEXTMARGIN_NAME_TRACKBAR, TBM_SETPOS, (WPARAM)TRUE, (LPARAM)n);

					std::wstringstream ws;
					std::wstring str;

					ws << L"이름 여백";
					ws << L" (";
					ws << n;
					ws << L")";
					str = ws.str();
					SetDlgItemTextW(hWnds.Setting, IDC_SETTING_TEXTMARGIN_NAME_TEXT, str.c_str());
				}

				{
					std::wstringstream ws;
					std::wstring str;

					ws << L"글꼴 : ";
					ws << L"";
					ws << Cl.Config->GetTextFont(CFG_NAME);
					ws << L"";
					str = ws.str();
					SetDlgItemTextW(hWnds.Setting, IDC_SETTING_NAME_FONT, str.c_str());
				}

				{
					std::wstringstream ws;
					std::wstring str;

					ws << L"글꼴 : ";
					ws << L"";
					ws << Cl.Config->GetTextFont(CFG_ORG);
					ws << L"";
					str = ws.str();
					SetDlgItemTextW(hWnds.Setting, IDC_SETTING_ORG_FONT, str.c_str());
				}

				{
					std::wstringstream ws;
					std::wstring str;

					ws << L"글꼴 : ";
					ws << L"";
					ws << Cl.Config->GetTextFont(CFG_TRANS);
					ws << L"";
					str = ws.str();
					SetDlgItemTextW(hWnds.Setting, IDC_SETTING_TRANS_FONT, str.c_str());
				}

				{
					int n = Cl.Config->GetClipLength();

					std::wstringstream ws;
					std::wstring str;

					SendDlgItemMessage(hWnds.Setting, IDC_SETTING_CLIP_TRACKBAR, TBM_SETRANGE, (WPARAM)TRUE, (LPARAM)MAKELONG(100, 1000));
					SendDlgItemMessage(hWnds.Setting, IDC_SETTING_CLIP_TRACKBAR, TBM_SETPOS, (WPARAM)TRUE, (LPARAM)n);

					ws << L"클립보드 최대 글자수 ";
					ws << L"(";
					ws << n;
					ws << L")";
					str = ws.str();
					SetDlgItemTextW(hWnds.Setting, IDC_SETTING_CLIP_TEXT, str.c_str());
				}
			}
		}
		break;
		case IDM_WINDOW_TRANS:
		{
			if (IsWindow(hWnds.Trans) == false)
			{
				RECT rect;
				int cx = GetSystemMetrics(SM_CXSCREEN);
				int cy = GetSystemMetrics(SM_CYSCREEN);

				hWnds.Trans = CreateDialog(hInst, MAKEINTRESOURCE(IDD_TRANSWIN), hWnd, TransWinProc);

				GetWindowRect(hWnds.Trans, &rect);

				SetWindowPos(hWnds.Trans, 0, (cx - rect.right + rect.left) / 2, (cy - rect.bottom + rect.top) / 2, 0, 0, SWP_NOSIZE);
				ShowWindow(hWnds.Trans, 1);

				SendMessage(GetDlgItem(hWnds.Trans, IDC_TRANSWIN_DEST), EM_SETLIMITTEXT, -1, 0);
				SendMessage(GetDlgItem(hWnds.Trans, IDC_TRANSWIN_SRC), EM_SETLIMITTEXT, -1, 0);
			}
			else
			{
				DestroyWindow(hWnds.Trans);
				hWnds.Trans = NULL;
				break;
			}
		}
			break;
		case IDM_TEXTALIGN_LEFT:
			Cl.Config->SetTextAlign(0);
			PostMessage(hWnds.Main, WM_PAINT, 0, 0);
			break;
		case IDM_TEXTALIGN_MID:
			Cl.Config->SetTextAlign(1);
			PostMessage(hWnds.Main, WM_PAINT, 0, 0);
			break;
		case IDM_TEXTALIGN_RIGHT:
			Cl.Config->SetTextAlign(2);
			PostMessage(hWnds.Main, WM_PAINT, 0, 0);
			break;
		case IDM_SEPERATE_NAME:
			(Cl.Config->GetTextSwitch(CFG_NAME) ? Cl.Config->SetTextSwitch(CFG_NAME, false) : Cl.Config->SetTextSwitch(CFG_NAME, true));
			PostMessage(hWnds.Main, WM_PAINT, 0, 0);
			break;
		case IDM_EXTERN_HOTKEY:
			(Cl.Config->GetExternHotkey() ? Cl.Config->SetExternHotkey(false) : Cl.Config->SetExternHotkey(true));
			break;
		case IDM_PRINT_ORGTEXT:
			(Cl.Config->GetTextSwitch(CFG_ORG) ? Cl.Config->SetTextSwitch(CFG_ORG, false) : Cl.Config->SetTextSwitch(CFG_ORG, true));
			PostMessage(hWnds.Main, WM_PAINT, 0, 0);
			break;
		case IDM_PRINT_ORGNAME:
			(Cl.Config->GetTextSwitch(CFG_NAME_ORG) ? Cl.Config->SetTextSwitch(CFG_NAME_ORG, false) : Cl.Config->SetTextSwitch(CFG_NAME_ORG, true));
			PostMessage(hWnds.Main, WM_PAINT, 0, 0);
			break;

		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
	}
	case WM_NCRBUTTONUP:
	case WM_CONTEXTMENU:
	{
		POINT pt;
		pt.x = LOWORD(lParam);
		pt.y = HIWORD(lParam);
		if (!OnContextMenu(hWnd, pt.x,pt.y))
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

		if (Cl.Config->GetMagneticMode())
		{
			MagnetWnd.diff_x = prc->left - MagnetWnd.rect_x;
			MagnetWnd.diff_y = prc->top - MagnetWnd.rect_y;
		}
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
	break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

INT_PTR CALLBACK SettingProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	int wmId, wmEvent;

	switch (message)
	{
	case WM_CREATE:
	{
		//CheckDlgButton(hWnd, IDC_SETTING_TOPMOST, true);
	}
		break;
	case WM_DESTROY:
	{
		Cl.Config->SaveConfig();
	}
		break;
	case WM_COMMAND:
		wmId = LOWORD(wParam);
		wmEvent = HIWORD(wParam);
		// 메뉴 선택을 구문 분석합니다.
		switch (wmId)
		{
		case IDM_SETTING_EXIT:
			SendMessage(hWnds.Main, WM_COMMAND, IDM_TERMINATE_ANEMONE, 0);
			break;
		case IDM_SETTING_OPENDIC:
			SendMessage(hWnds.Main, WM_COMMAND, IDM_OPENDIC, 0);
			break;
		case IDM_SETTING_OPENINI:
			SendMessage(hWnds.Main, WM_COMMAND, IDM_OPENINI, 0);
			break;
		case IDM_SETTING_WINRESET:
			break;
		case IDM_SETTING_ADV:
			break;
		case IDM_SETTING_CLOSE:
			DestroyWindow(hWnd);
			break;
		case IDC_SETTING_TOPMOST:
			SendMessage(hWnds.Main, WM_COMMAND, IDM_TOPMOST, 0);
			break;
		case IDC_SETTING_MAGNETIC_MINIMIZE:
			(Cl.Config->GetMagneticMinimize() ? Cl.Config->SetMagneticMinimize(false) : Cl.Config->SetMagneticMinimize(true));
			break;
		case IDC_SETTING_HIDEWIN:
			SendMessage(hWnds.Main, WM_COMMAND, IDM_WINDOW_VISIBLE, 0);
			break;
		case IDC_SETTING_HIDEWIN_UNWATCH_CLIPBOARD:
			SendMessage(hWnds.Main, WM_COMMAND, IDM_HIDEWIN_UNWATCH_CLIPBOARD, 0);
			break;
		case IDC_SETTING_HIDEWIN_UNLOCK_HOTKEY:
			SendMessage(hWnds.Main, WM_COMMAND, IDM_HIDEWIN_UNLOCK_HOTKEY, 0);
			break;
		case IDC_SETTING_CLIPBOARD_WATCH:
			SendMessage(hWnds.Main, WM_COMMAND, IDM_CLIPBOARD_SWITCH, 0);
			break;
		case IDC_SETTING_WNDCLICK_THOUGH:
			SendMessage(hWnds.Main, WM_COMMAND, IDM_TEMP_CLICK_THOUGH, 0);
			break;
		case IDC_SETTING_USE_MAGNETIC:
			SendMessage(hWnds.Main, WM_COMMAND, IDM_MAGNETIC_MODE, 0);
			break;
		case IDC_SETTING_WND_BORDER_MODE:
			SendMessage(hWnds.Main, WM_COMMAND, IDM_WND_BORDER_MODE, 0);
			break;
		case IDC_SETTING_PRINT_ORGTEXT:
			SendMessage(hWnds.Main, WM_COMMAND, IDM_PRINT_ORGTEXT, 0);
			break;
		case IDC_SETTING_PRINT_ORGNAME:
			SendMessage(hWnds.Main, WM_COMMAND, IDM_PRINT_ORGNAME, 0);
			break;
		case IDC_SETTING_SEPERATE_NAME:
			SendMessage(hWnds.Main, WM_COMMAND, IDM_SEPERATE_NAME, 0);
			break;
		case IDC_SETTING_REPEAT_TEXT:
			switch (Cl.Config->GetRepeatTextProc())
			{
			case 0:
				Cl.Config->SetRepeatTextProc(1);
				break;
			case 1:
				Cl.Config->SetRepeatTextProc(2);
				break;
			case 2:
				Cl.Config->SetRepeatTextProc(3);
				break;
			case 3:
				Cl.Config->SetRepeatTextProc(4);
				break;
			case 4:
				Cl.Config->SetRepeatTextProc(0);
				break;
			}
			break;
		case IDC_SETTING_TEXTEND_NAME:
			(Cl.Config->GetReviseName() ? Cl.Config->SetReviseName(false) : Cl.Config->SetReviseName(true));
			break;
		case IDC_SETTING_FORCE_ANEDIC:
			break;
		case IDC_SETTING_ANE_REMOCON:
			break;
		case IDC_SETTING_EXTERN_HOTKEY:
			SendMessage(hWnds.Main, WM_COMMAND, IDM_EXTERN_HOTKEY, 0);
			break;
		case IDC_SETTING_TEXTALIGN_LEFT:
			SendMessage(hWnds.Main, WM_COMMAND, IDM_TEXTALIGN_LEFT, 0);
			break;
		case IDC_SETTING_TEXTALIGN_MID:
			SendMessage(hWnds.Main, WM_COMMAND, IDM_TEXTALIGN_MID, 0);
			break;
		case IDC_SETTING_TEXTALIGN_RIGHT:
			SendMessage(hWnds.Main, WM_COMMAND, IDM_TEXTALIGN_RIGHT, 0);
			break;
		case IDC_SETTING_BACKGROUND_SWITCH:
			(Cl.Config->GetBGSwitch() ? Cl.Config->SetBGSwitch(false) : Cl.Config->SetBGSwitch(true));
			PostMessage(hWnds.Main, WM_PAINT, 0, 0);
			break;
		case IDC_SETTING_BACKGROUND_COLOR:
		case IDC_SETTING_NAME_COLOR:
		case IDC_SETTING_NAME_OUTLINE1:
		case IDC_SETTING_NAME_OUTLINE2:
		case IDC_SETTING_NAME_SHADOW_COLOR:
		case IDC_SETTING_ORG_COLOR:
		case IDC_SETTING_ORG_OUTLINE1:
		case IDC_SETTING_ORG_OUTLINE2:
		case IDC_SETTING_ORG_SHADOW_COLOR:
		case IDC_SETTING_TRANS_COLOR:
		case IDC_SETTING_TRANS_OUTLINE1:
		case IDC_SETTING_TRANS_OUTLINE2:
		case IDC_SETTING_TRANS_SHADOW_COLOR:
			{
				CHOOSECOLOR cc; 
				DWORD dwColor;

				switch (wmId)
				{
				case IDC_SETTING_BACKGROUND_COLOR:
					dwColor = Cl.Config->GetBGColor();
					break;
				case IDC_SETTING_NAME_COLOR:
					dwColor = Cl.Config->GetTextColor(CFG_NAME, CFG_A);
					break;
				case IDC_SETTING_NAME_OUTLINE1:
					dwColor = Cl.Config->GetTextColor(CFG_NAME, CFG_B);
					break;
				case IDC_SETTING_NAME_OUTLINE2:
					dwColor = Cl.Config->GetTextColor(CFG_NAME, CFG_C);
					break;
				case IDC_SETTING_NAME_SHADOW_COLOR:
					dwColor = Cl.Config->GetTextColor(CFG_NAME, CFG_S);
					break;
				case IDC_SETTING_ORG_COLOR:
					dwColor = Cl.Config->GetTextColor(CFG_ORG, CFG_A);
					break;
				case IDC_SETTING_ORG_OUTLINE1:
					dwColor = Cl.Config->GetTextColor(CFG_ORG, CFG_B);
					break;
				case IDC_SETTING_ORG_OUTLINE2:
					dwColor = Cl.Config->GetTextColor(CFG_ORG, CFG_C);
					break;
				case IDC_SETTING_ORG_SHADOW_COLOR:
					dwColor = Cl.Config->GetTextColor(CFG_ORG, CFG_S);
					break;
				case IDC_SETTING_TRANS_COLOR:
					dwColor = Cl.Config->GetTextColor(CFG_TRANS, CFG_A);
					break;
				case IDC_SETTING_TRANS_OUTLINE1:
					dwColor = Cl.Config->GetTextColor(CFG_TRANS, CFG_B);
					break;
				case IDC_SETTING_TRANS_OUTLINE2:
					dwColor = Cl.Config->GetTextColor(CFG_TRANS, CFG_C);
					break;
				case IDC_SETTING_TRANS_SHADOW_COLOR:
					dwColor = Cl.Config->GetTextColor(CFG_TRANS, CFG_S);
					break;
				}

				if (ColorDialog(hWnd, cc, dwColor))
				{
					DWORD dw;

					dw = (cc.lCustData & 0xFF) << 24;
					dw |= (cc.rgbResult & 0xFF) << 16;
					dw |= ((cc.rgbResult >> 8) & 0xFF) << 8;
					dw |= ((cc.rgbResult >> 16) & 0xFF);

					switch (wmId)
					{
					case IDC_SETTING_BACKGROUND_COLOR:
						Cl.Config->SetBGColor(dw);
						break;
					case IDC_SETTING_NAME_COLOR:
						Cl.Config->SetTextColor(CFG_NAME, CFG_A, dw);
						break;
					case IDC_SETTING_NAME_OUTLINE1:
						Cl.Config->SetTextColor(CFG_NAME, CFG_B, dw);
						break;
					case IDC_SETTING_NAME_OUTLINE2:
						Cl.Config->SetTextColor(CFG_NAME, CFG_C, dw);
						break;
					case IDC_SETTING_NAME_SHADOW_COLOR:
						Cl.Config->SetTextColor(CFG_NAME, CFG_S, dw);
						break;
					case IDC_SETTING_ORG_COLOR:
						Cl.Config->SetTextColor(CFG_ORG, CFG_A, dw);
						break;
					case IDC_SETTING_ORG_OUTLINE1:
						Cl.Config->SetTextColor(CFG_ORG, CFG_B, dw);
						break;
					case IDC_SETTING_ORG_OUTLINE2:
						Cl.Config->SetTextColor(CFG_ORG, CFG_C, dw);
						break;
					case IDC_SETTING_ORG_SHADOW_COLOR:
						Cl.Config->SetTextColor(CFG_ORG, CFG_S, dw);
						break;
					case IDC_SETTING_TRANS_COLOR:
						Cl.Config->SetTextColor(CFG_TRANS, CFG_A, dw);
						break;
					case IDC_SETTING_TRANS_OUTLINE1:
						Cl.Config->SetTextColor(CFG_TRANS, CFG_B, dw);
						break;
					case IDC_SETTING_TRANS_OUTLINE2:
						Cl.Config->SetTextColor(CFG_TRANS, CFG_C, dw);
						break;
					case IDC_SETTING_TRANS_SHADOW_COLOR:
						Cl.Config->SetTextColor(CFG_TRANS, CFG_S, dw);
						break;
					}
					
					SetDlgItemTextW(hWnd, wmId, L"");
					PostMessage(hWnds.Main, WM_PAINT, 0, 0);
				}
			}
			break;
		case IDC_SETTING_TEXTSIZE_MINUS:
		{
			int nTextSize = Cl.Config->GetTextSize(CFG_TRANS, CFG_A);
			if (nTextSize - 1 < 6) break;
			Cl.Config->SetTextSize(CFG_NAME, CFG_A, nTextSize - 1);
			Cl.Config->SetTextSize(CFG_ORG, CFG_A, nTextSize - 1);
			Cl.Config->SetTextSize(CFG_TRANS, CFG_A, nTextSize - 1);
			PostMessage(hWnds.Main, WM_PAINT, 0, 0);
		}
			break;
		case IDC_SETTING_TEXTSIZE_PLUS:
		{
			int nTextSize = Cl.Config->GetTextSize(CFG_TRANS, CFG_A);
			Cl.Config->SetTextSize(CFG_NAME, CFG_A, nTextSize + 1);
			Cl.Config->SetTextSize(CFG_ORG, CFG_A, nTextSize + 1);
			Cl.Config->SetTextSize(CFG_TRANS, CFG_A, nTextSize + 1);
			PostMessage(hWnds.Main, WM_PAINT, 0, 0);
		}
			break;
		case IDC_SETTING_OUTLINE1_SIZE_MINUS:
		{
			int nTextSize = Cl.Config->GetTextSize(CFG_TRANS, CFG_B);
			if (nTextSize - 1 < 0) break;
			Cl.Config->SetTextSize(CFG_NAME, CFG_B, nTextSize - 1);
			Cl.Config->SetTextSize(CFG_ORG, CFG_B, nTextSize - 1);
			Cl.Config->SetTextSize(CFG_TRANS, CFG_B, nTextSize - 1);
			PostMessage(hWnds.Main, WM_PAINT, 0, 0);
		}
			break;
		case IDC_SETTING_OUTLINE1_SIZE_PLUS:
		{
			int nTextSize = Cl.Config->GetTextSize(CFG_TRANS, CFG_B);
			Cl.Config->SetTextSize(CFG_NAME, CFG_B, nTextSize + 1);
			Cl.Config->SetTextSize(CFG_ORG, CFG_B, nTextSize + 1);
			Cl.Config->SetTextSize(CFG_TRANS, CFG_B, nTextSize + 1);
			PostMessage(hWnds.Main, WM_PAINT, 0, 0);
		}
		case IDC_SETTING_OUTLINE2_SIZE_MINUS:
		{
			int nTextSize = Cl.Config->GetTextSize(CFG_TRANS, CFG_C);
			if (nTextSize - 1 < 0) break;
			Cl.Config->SetTextSize(CFG_NAME, CFG_C, nTextSize - 1);
			Cl.Config->SetTextSize(CFG_ORG, CFG_C, nTextSize - 1);
			Cl.Config->SetTextSize(CFG_TRANS, CFG_C, nTextSize - 1);
			PostMessage(hWnds.Main, WM_PAINT, 0, 0);
		}
			break;
		case IDC_SETTING_OUTLINE2_SIZE_PLUS:
		{
			int nTextSize = Cl.Config->GetTextSize(CFG_TRANS, CFG_C);
			Cl.Config->SetTextSize(CFG_NAME, CFG_C, nTextSize + 1);
			Cl.Config->SetTextSize(CFG_ORG, CFG_C, nTextSize + 1);
			Cl.Config->SetTextSize(CFG_TRANS, CFG_C, nTextSize + 1);
			PostMessage(hWnds.Main, WM_PAINT, 0, 0);
		}
			break;
		case IDC_SETTING_NAME_SHADOW:
			(Cl.Config->GetTextShadow(CFG_NAME) ? Cl.Config->SetTextShadow(CFG_NAME, false) : Cl.Config->SetTextShadow(CFG_NAME, true));
			PostMessage(hWnds.Main, WM_PAINT, 0, 0);
			break;
		case IDC_SETTING_ORG_SHADOW:
			(Cl.Config->GetTextShadow(CFG_ORG) ? Cl.Config->SetTextShadow(CFG_ORG, false) : Cl.Config->SetTextShadow(CFG_ORG, true));
			PostMessage(hWnds.Main, WM_PAINT, 0, 0);
			break;
		case IDC_SETTING_TRANS_SHADOW:
			(Cl.Config->GetTextShadow(CFG_TRANS) ? Cl.Config->SetTextShadow(CFG_TRANS, false) : Cl.Config->SetTextShadow(CFG_TRANS, true));
			PostMessage(hWnds.Main, WM_PAINT, 0, 0);
			break;

		case IDC_SETTING_NAME_FONT:
		{
			CHOOSEFONT cf;
			LOGFONT lf;

			ZeroMemory(&lf, sizeof(lf));
			wcscpy_s(lf.lfFaceName, Cl.Config->GetTextFont(CFG_NAME));

			PAINTSTRUCT ps;
			HDC hDC = BeginPaint(hWnd, &ps);
			lf.lfHeight = -MulDiv(22, GetDeviceCaps(hDC, LOGPIXELSY), 72);
			EndPaint(hWnd, &ps);

			if (FontDialog(hWnd, cf, lf))
			{
				Cl.Config->SetTextFont(CFG_NAME, lf.lfFaceName);
			}
			PostMessage(hWnds.Main, WM_PAINT, 0, 0);
		}
			break;
		case IDC_SETTING_ORG_FONT:
		{
			CHOOSEFONT cf;
			LOGFONT lf;

			ZeroMemory(&lf, sizeof(lf));
			wcscpy_s(lf.lfFaceName, Cl.Config->GetTextFont(CFG_ORG));

			PAINTSTRUCT ps;
			HDC hDC = BeginPaint(hWnd, &ps);
			lf.lfHeight = -MulDiv(22, GetDeviceCaps(hDC, LOGPIXELSY), 72);
			EndPaint(hWnd, &ps);

			if (FontDialog(hWnd, cf, lf))
			{
				Cl.Config->SetTextFont(CFG_ORG, lf.lfFaceName);
			}
			PostMessage(hWnds.Main, WM_PAINT, 0, 0);
		}
			break;
		case IDC_SETTING_TRANS_FONT:
		{
			CHOOSEFONT cf;
			LOGFONT lf;

			ZeroMemory(&lf, sizeof(lf));
			wcscpy_s(lf.lfFaceName, Cl.Config->GetTextFont(CFG_TRANS));

			PAINTSTRUCT ps;
			HDC hDC = BeginPaint(hWnd, &ps);
			lf.lfHeight = -MulDiv(22, GetDeviceCaps(hDC, LOGPIXELSY), 72);
			EndPaint(hWnd, &ps);

			if (FontDialog(hWnd, cf, lf))
			{
				Cl.Config->SetTextFont(CFG_TRANS, lf.lfFaceName);
			}
			PostMessage(hWnds.Main, WM_PAINT, 0, 0);
		}
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
		SendMessage(hWnds.Main, WM_COMMAND, IDM_SETTING_CHECK, 0);
		break;
	case WM_LBUTTONDOWN:
		SendMessage(hWnd, WM_NCLBUTTONDOWN, HTCAPTION, 0);
		break;
	case WM_ERASEBKGND:
		return false;
	case WM_MOVING:
	case WM_SIZING:
	{
		RECT *prc = (RECT *)lParam;
		SetWindowPos(hWnd, NULL, prc->left, prc->top, prc->right - prc->left, prc->bottom - prc->top, 0);
	}
		break;

	case WM_DRAWITEM:
	{
		switch (wParam)
		{
		case IDC_SETTING_BACKGROUND_COLOR:
		case IDC_SETTING_NAME_COLOR:
		case IDC_SETTING_NAME_OUTLINE1:
		case IDC_SETTING_NAME_OUTLINE2:
		case IDC_SETTING_NAME_SHADOW_COLOR:
		case IDC_SETTING_ORG_COLOR:
		case IDC_SETTING_ORG_OUTLINE1:
		case IDC_SETTING_ORG_OUTLINE2:
		case IDC_SETTING_ORG_SHADOW_COLOR:
		case IDC_SETTING_TRANS_COLOR:
		case IDC_SETTING_TRANS_OUTLINE1:
		case IDC_SETTING_TRANS_OUTLINE2:
		case IDC_SETTING_TRANS_SHADOW_COLOR:
		{
			LPDRAWITEMSTRUCT pdis = (LPDRAWITEMSTRUCT)lParam;

			DWORD ColorVar;

			switch (wParam)
			{
			case IDC_SETTING_BACKGROUND_COLOR:
				ColorVar = Cl.Config->GetBGColor();
				break;
			case IDC_SETTING_NAME_COLOR:
				ColorVar = Cl.Config->GetTextColor(CFG_NAME, CFG_A);
				break;
			case IDC_SETTING_NAME_OUTLINE1:
				ColorVar = Cl.Config->GetTextColor(CFG_NAME, CFG_B);
				break;
			case IDC_SETTING_NAME_OUTLINE2:
				ColorVar = Cl.Config->GetTextColor(CFG_NAME, CFG_C);
				break;
			case IDC_SETTING_NAME_SHADOW_COLOR:
				ColorVar = Cl.Config->GetTextColor(CFG_NAME, CFG_S);
				break;
			case IDC_SETTING_ORG_COLOR:
				ColorVar = Cl.Config->GetTextColor(CFG_ORG, CFG_A);
				break;
			case IDC_SETTING_ORG_OUTLINE1:
				ColorVar = Cl.Config->GetTextColor(CFG_ORG, CFG_B);
				break;
			case IDC_SETTING_ORG_OUTLINE2:
				ColorVar = Cl.Config->GetTextColor(CFG_ORG, CFG_C);
				break;
			case IDC_SETTING_ORG_SHADOW_COLOR:
				ColorVar = Cl.Config->GetTextColor(CFG_ORG, CFG_S);
				break;
			case IDC_SETTING_TRANS_COLOR:
				ColorVar = Cl.Config->GetTextColor(CFG_TRANS, CFG_A);
				break;
			case IDC_SETTING_TRANS_OUTLINE1:
				ColorVar = Cl.Config->GetTextColor(CFG_TRANS, CFG_B);
				break;
			case IDC_SETTING_TRANS_OUTLINE2:
				ColorVar = Cl.Config->GetTextColor(CFG_TRANS, CFG_C);
				break;
			case IDC_SETTING_TRANS_SHADOW_COLOR:
				ColorVar = Cl.Config->GetTextColor(CFG_TRANS, CFG_S);
				break;
			}

			int a, c1, c2, c3;
			a = (ColorVar >> 24) & 0xFF;
			c1 = (ColorVar >> 16) & 0xFF;
			c2 = (ColorVar >> 8) & 0xFF;
			c3 = (ColorVar)& 0xFF;

			COLORREF color = ((COLORREF)(((BYTE)(c1) | (((BYTE)(c2)) << 8)) | (((BYTE)(c3)) << 16)));

			HBRUSH hBrush = CreateSolidBrush(color);
			FillRect(pdis->hDC, &pdis->rcItem, hBrush);
		}
		}
	}
		break;

	case WM_HSCROLL:
	{
		if ((HWND)lParam == GetDlgItem(hWnd, IDC_SETTING_BACKGROUND_TRACKBAR))
		{
			int i;

			switch (LOWORD(wParam))
			{
			case TB_LINEUP:
			case TB_LINEDOWN:
			case TB_PAGEUP:
			case TB_PAGEDOWN:
			case TB_TOP:
			case TB_BOTTOM:
			case TB_ENDTRACK:
				i = (INT)SendDlgItemMessage(hWnd, IDC_SETTING_BACKGROUND_TRACKBAR, TBM_GETPOS, 0, 0);
				break;

			case TB_THUMBTRACK:
				i = (INT)HIWORD(wParam);
				break;
			}

			// 투명도 255 이상이면 무시
			if (i > 255) break;

			DWORD BGColor = (i << 24);
			BGColor |= (Cl.Config->GetBGColor() & 0xFFFFFF);

			Cl.Config->SetBGColor(BGColor);

			PostMessage(hWnds.Main, WM_COMMAND, IDM_SETTING_CHECK, 0);
			PostMessage(hWnds.Main, WM_PAINT, 0, 0);
		}
		else if ((HWND)lParam == GetDlgItem(hWnd, IDC_SETTING_TEXTSIZE_TRACKBAR))
		{
			int i;

			switch (LOWORD(wParam))
			{
			case TB_LINEUP:
			case TB_LINEDOWN:
			case TB_PAGEUP:
			case TB_PAGEDOWN:
			case TB_TOP:
			case TB_BOTTOM:
			case TB_ENDTRACK:
				i = (INT)SendDlgItemMessage(hWnd, IDC_SETTING_TEXTSIZE_TRACKBAR, TBM_GETPOS, 0, 0);
				break;

			case TB_THUMBTRACK:
				i = (INT)HIWORD(wParam);
				break;
			}

			if (i > 100) break;

			Cl.Config->SetTextSize(CFG_NAME, CFG_A, i);
			Cl.Config->SetTextSize(CFG_ORG, CFG_A, i);
			Cl.Config->SetTextSize(CFG_TRANS, CFG_A, i);

			PostMessage(hWnds.Main, WM_COMMAND, IDM_SETTING_CHECK, 0);
			PostMessage(hWnds.Main, WM_PAINT, 0, 0);
		}
		else if ((HWND)lParam == GetDlgItem(hWnd, IDC_SETTING_OUTLINE1_SIZE_TRACKBAR))
		{
			int i;

			switch (LOWORD(wParam))
			{
			case TB_LINEUP:
			case TB_LINEDOWN:
			case TB_PAGEUP:
			case TB_PAGEDOWN:
			case TB_TOP:
			case TB_BOTTOM:
			case TB_ENDTRACK:
				i = (INT)SendDlgItemMessage(hWnd, IDC_SETTING_OUTLINE1_SIZE_TRACKBAR, TBM_GETPOS, 0, 0);
				break;

			case TB_THUMBTRACK:
				i = (INT)HIWORD(wParam);
				break;
			}

			if (i > 20) break;

			Cl.Config->SetTextSize(CFG_NAME, CFG_B, i);
			Cl.Config->SetTextSize(CFG_ORG, CFG_B, i);
			Cl.Config->SetTextSize(CFG_TRANS, CFG_B, i);

			PostMessage(hWnds.Main, WM_COMMAND, IDM_SETTING_CHECK, 0);
			PostMessage(hWnds.Main, WM_PAINT, 0, 0);
		}
		else if ((HWND)lParam == GetDlgItem(hWnd, IDC_SETTING_OUTLINE2_SIZE_TRACKBAR))
		{
			int i;

			switch (LOWORD(wParam))
			{
			case TB_LINEUP:
			case TB_LINEDOWN:
			case TB_PAGEUP:
			case TB_PAGEDOWN:
			case TB_TOP:
			case TB_BOTTOM:
			case TB_ENDTRACK:
				i = (INT)SendDlgItemMessage(hWnd, IDC_SETTING_OUTLINE2_SIZE_TRACKBAR, TBM_GETPOS, 0, 0);
				break;

			case TB_THUMBTRACK:
				i = (INT)HIWORD(wParam);
				break;
			}

			if (i > 20) break;

			Cl.Config->SetTextSize(CFG_NAME, CFG_C, i);
			Cl.Config->SetTextSize(CFG_ORG, CFG_C, i);
			Cl.Config->SetTextSize(CFG_TRANS, CFG_C, i);

			PostMessage(hWnds.Main, WM_COMMAND, IDM_SETTING_CHECK, 0);
			PostMessage(hWnds.Main, WM_PAINT, 0, 0);
		}
		else if ((HWND)lParam == GetDlgItem(hWnd, IDC_SETTING_SHADOW_X_TRACKBAR))
		{
			int i;

			switch (LOWORD(wParam))
			{
			case TB_LINEUP:
			case TB_LINEDOWN:
			case TB_PAGEUP:
			case TB_PAGEDOWN:
			case TB_TOP:
			case TB_BOTTOM:
			case TB_ENDTRACK:
				i = (INT)SendDlgItemMessage(hWnd, IDC_SETTING_SHADOW_X_TRACKBAR, TBM_GETPOS, 0, 0);
				break;

			case TB_THUMBTRACK:
				i = (INT)HIWORD(wParam);
				break;
			}

			Cl.Config->SetShadowX(i);

			PostMessage(hWnds.Main, WM_COMMAND, IDM_SETTING_CHECK, 0);
			PostMessage(hWnds.Main, WM_PAINT, 0, 0);
		}
		else if ((HWND)lParam == GetDlgItem(hWnd, IDC_SETTING_SHADOW_Y_TRACKBAR))
		{
			int i;

			switch (LOWORD(wParam))
			{
			case TB_LINEUP:
			case TB_LINEDOWN:
			case TB_PAGEUP:
			case TB_PAGEDOWN:
			case TB_TOP:
			case TB_BOTTOM:
			case TB_ENDTRACK:
				i = (INT)SendDlgItemMessage(hWnd, IDC_SETTING_SHADOW_Y_TRACKBAR, TBM_GETPOS, 0, 0);
				break;

			case TB_THUMBTRACK:
				i = (INT)HIWORD(wParam);
				break;
			}

			Cl.Config->SetShadowY(i);

			PostMessage(hWnds.Main, WM_COMMAND, IDM_SETTING_CHECK, 0);
			PostMessage(hWnds.Main, WM_PAINT, 0, 0);
		}
		else if ((HWND)lParam == GetDlgItem(hWnd, IDC_SETTING_TEXTMARGIN_X_TRACKBAR))
		{
			int i;

			switch (LOWORD(wParam))
			{
			case TB_LINEUP:
			case TB_LINEDOWN:
			case TB_PAGEUP:
			case TB_PAGEDOWN:
			case TB_TOP:
			case TB_BOTTOM:
			case TB_ENDTRACK:
				i = (INT)SendDlgItemMessage(hWnd, IDC_SETTING_TEXTMARGIN_X_TRACKBAR, TBM_GETPOS, 0, 0);
				break;

			case TB_THUMBTRACK:
				i = (INT)HIWORD(wParam);
				break;
			}

			if (i > 300) break;

			Cl.Config->SetTextMarginX(i);

			PostMessage(hWnds.Main, WM_COMMAND, IDM_SETTING_CHECK, 0);
			PostMessage(hWnds.Main, WM_PAINT, 0, 0);
		}
		else if ((HWND)lParam == GetDlgItem(hWnd, IDC_SETTING_TEXTMARGIN_Y_TRACKBAR))
		{
			int i;

			switch (LOWORD(wParam))
			{
			case TB_LINEUP:
			case TB_LINEDOWN:
			case TB_PAGEUP:
			case TB_PAGEDOWN:
			case TB_TOP:
			case TB_BOTTOM:
			case TB_ENDTRACK:
				i = (INT)SendDlgItemMessage(hWnd, IDC_SETTING_TEXTMARGIN_Y_TRACKBAR, TBM_GETPOS, 0, 0);
				break;

			case TB_THUMBTRACK:
				i = (INT)HIWORD(wParam);
				break;
			}

			if (i > 300) break;

			Cl.Config->SetTextMarginY(i);

			PostMessage(hWnds.Main, WM_COMMAND, IDM_SETTING_CHECK, 0);
			PostMessage(hWnds.Main, WM_PAINT, 0, 0);
		}
		else if ((HWND)lParam == GetDlgItem(hWnd, IDC_SETTING_TEXTMARGIN_NAME_TRACKBAR))
		{
			int i;

			switch (LOWORD(wParam))
			{
			case TB_LINEUP:
			case TB_LINEDOWN:
			case TB_PAGEUP:
			case TB_PAGEDOWN:
			case TB_TOP:
			case TB_BOTTOM:
			case TB_ENDTRACK:
				i = (INT)SendDlgItemMessage(hWnd, IDC_SETTING_TEXTMARGIN_NAME_TRACKBAR, TBM_GETPOS, 0, 0);
				break;

			case TB_THUMBTRACK:
				i = (INT)HIWORD(wParam);
				break;
			}

			if (i > 300) break;

			Cl.Config->SetTextMarginName(i);

			PostMessage(hWnds.Main, WM_COMMAND, IDM_SETTING_CHECK, 0);
			PostMessage(hWnds.Main, WM_PAINT, 0, 0);
		}
		else if ((HWND)lParam == GetDlgItem(hWnd, IDC_SETTING_CLIP_TRACKBAR))
		{
			int i;

			switch (LOWORD(wParam))
			{
			case TB_LINEUP:
			case TB_LINEDOWN:
			case TB_PAGEUP:
			case TB_PAGEDOWN:
			case TB_TOP:
			case TB_BOTTOM:
			case TB_ENDTRACK:
				i = (INT)SendDlgItemMessage(hWnd, IDC_SETTING_CLIP_TRACKBAR, TBM_GETPOS, 0, 0);
				break;

			case TB_THUMBTRACK:
				i = (INT)HIWORD(wParam);
				break;
			}

			if (i > 1000) break;

			Cl.Config->SetClipLength(i);

			PostMessage(hWnds.Main, WM_COMMAND, IDM_SETTING_CHECK, 0);
			PostMessage(hWnds.Main, WM_PAINT, 0, 0);
		}
	}
	}
	return 0;
}
CHOOSECOLOR *pCC;
UINT CALLBACK SettingColorWndHookProc(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	HFONT hFont;

	switch (uMsg) {
	case WM_INITDIALOG:
	{
		RECT rect;
		GetWindowRect(hDlg, &rect);

		SetWindowPos(hDlg, NULL, rect.left, rect.top, rect.right - rect.left + 60, rect.bottom - rect.top, 0);

		hFont = (HFONT)GetStockObject(DEFAULT_GUI_FONT);

		HWND hTrackBar = CreateWindow(L"msctls_trackbar32", L"", TBS_VERT | TBS_BOTH | TBS_NOTICKS | WS_CHILD | WS_VISIBLE,
			540, 2, 25, 225, hDlg, (HMENU)IDC_COLORDLG_ALPHA_TRACKBAR, hInst, NULL);
		HWND hStatic = CreateWindow(L"STATIC", L"불투명도", WS_CHILD | WS_VISIBLE,
			528, 256, 150, 15, hDlg, 0, hInst, NULL);
		HWND hEdit = CreateWindow(L"EDIT", L"", ES_CENTER | ES_AUTOHSCROLL | WS_CHILD | WS_VISIBLE | WS_BORDER | WS_BORDER,
			540, 230, 25, 18, hDlg, (HMENU)IDC_COLORDLG_ALPHA_EDIT, hInst, NULL);

		pCC = (CHOOSECOLOR *)lParam;
		int iAlpha = pCC->lCustData;

		SendDlgItemMessage(hDlg, IDC_COLORDLG_ALPHA_TRACKBAR, TBM_SETRANGE, (WPARAM)TRUE, (LPARAM)MAKELONG(0, 255));

		std::wstring bgAlpha;

		SendDlgItemMessage(hDlg, IDC_COLORDLG_ALPHA_TRACKBAR, TBM_SETPOS, (WPARAM)TRUE, (LPARAM)iAlpha);

		std::wstringstream ws;
		std::wstring str;

		ws << iAlpha;
		str = ws.str();
		SetDlgItemTextW(hDlg, IDC_COLORDLG_ALPHA_EDIT, str.c_str());

		SendMessage(hTrackBar, WM_SETFONT, (WPARAM)hFont, NULL);
		SendMessage(hStatic, WM_SETFONT, (WPARAM)hFont, NULL);
		SendMessage(hEdit, WM_SETFONT, (WPARAM)hFont, NULL);

		int nExStyleValue = GetWindowLong(hDlg, GWL_EXSTYLE);

		nExStyleValue |= WS_EX_NOACTIVATE;

		SetWindowLong(hDlg, GWL_EXSTYLE, nExStyleValue);

		//GetCursorPos(&pt); // 현재 커서 위치를 얻어 내고
		//SetWindowPos(hDlg, NULL, pt.x, pt.y, 0, 0,
		//            SWP_NOSIZE | SWP_NOZORDER | SWP_NOREDRAW);
	}
		return TRUE;

	case WM_HSCROLL:
	case WM_VSCROLL:
	{
		int i_AlphaInt;

		switch (LOWORD(wParam))
		{
		case TB_LINEUP:
		case TB_LINEDOWN:
		case TB_PAGEUP:
		case TB_PAGEDOWN:
		case TB_TOP:
		case TB_BOTTOM:
		case TB_ENDTRACK:
			i_AlphaInt = (INT)SendDlgItemMessage(hDlg, IDC_COLORDLG_ALPHA_TRACKBAR, TBM_GETPOS, 0, 0);
			break;

		case TB_THUMBTRACK:
			i_AlphaInt = (INT)HIWORD(wParam);
			break;
		}

		std::wstringstream ws;

		// Event Id = LOWORD(wParam) 5(Clicked) / 8(Out-Clicked)
		//if (LOWORD(wParam) == 8) break;

		ws << i_AlphaInt;
		std::wstring str(ws.str());
		SetDlgItemTextW(hDlg, IDC_COLORDLG_ALPHA_EDIT, str.c_str());

		pCC->lCustData = i_AlphaInt;

	}
		break;
	case WM_MOVING:
	case WM_SIZING:
	{
		RECT *prc = (RECT *)lParam;
		SetWindowPos(hDlg, NULL, prc->left, prc->top, prc->right - prc->left, prc->bottom - prc->top, 0);
	}
		break;

		/*
		case WM_LBUTTONDOWN:
		MessageBox(hDlg,0,0,0);
		break;*/
	}
	return FALSE;
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
WNDPROC pEditProc[2];
LRESULT CALLBACK EditProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	WNDPROC pEdit;
	
	switch (msg)
	{
	case WM_CHAR:
		if (wParam == 1) return -1;
	}
	
	if (msg == WM_KEYDOWN) {
		if (GetKeyState(VK_CONTROL) & 0x8000 && wParam == 'A') {
			SendMessage(hwnd, EM_SETSEL, 0, -1);
			return -1;
		}
	}
	if (hwnd == GetDlgItem(hWnds.Trans, IDC_TRANSWIN_SRC))
		pEdit = pEditProc[0];
	else if (hwnd == GetDlgItem(hWnds.Trans, IDC_TRANSWIN_DEST))
		pEdit = pEditProc[1];

	return CallWindowProc(pEdit, hwnd, msg, wParam, lParam);
}

INT_PTR CALLBACK TransWinProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	int wmId, wmEvent;

	switch (message)
	{
	case WM_SHOWWINDOW:
	{
		CheckDlgButton(hWnd, IDC_TRANSWIN_TRANS_ONEGO, Cl.Config->GetTransOneGo());

		if (!(pEditProc[0] = (WNDPROC)SetWindowLong(GetDlgItem(hWnd, IDC_TRANSWIN_SRC), GWL_WNDPROC, (LONG)&EditProc)) ||
			!(pEditProc[1] = (WNDPROC)SetWindowLong(GetDlgItem(hWnd, IDC_TRANSWIN_DEST), GWL_WNDPROC, (LONG)&EditProc)))
		{
			//assert(false);
			return false;
		}
	}
		break;
	case WM_COMMAND:
		wmId = LOWORD(wParam);
		wmEvent = HIWORD(wParam);
		// 메뉴 선택을 구문 분석합니다.
		switch (wmId)
		{
		case IDC_TRANSWIN_CLEAR:
			SetDlgItemText(hWnd, IDC_TRANSWIN_SRC, L"");
			SetDlgItemText(hWnd, IDC_TRANSWIN_DEST, L"");
			SetFocus(GetDlgItem(hWnd, IDC_TRANSWIN_SRC));
			break;
		case IDC_TRANSWIN_COPY:
		{
			int length = SendMessage(GetDlgItem(hWnd, IDC_TRANSWIN_DEST), WM_GETTEXTLENGTH, 0, 0) + 1;
			wchar_t *pStr = (wchar_t *)malloc(sizeof(wchar_t) * (length + 1));
			GetDlgItemText(hWnd, IDC_TRANSWIN_DEST, pStr, length);
			
			IsActive = 2;

			OpenClipboard(hWnds.Clip);
			EmptyClipboard();

			SetClipboardData(CF_UNICODETEXT, pStr);
			CloseClipboard();

			free(pStr);
		}
			break;
		case IDC_TRANSWIN_TRANSLATE:
		{
			Cl.TextProcess->TranslateAbort();

			SetFocus(GetDlgItem(hWnd, IDC_TRANSWIN_DEST));
			SetWindowText(GetDlgItem(hWnd, IDC_TRANSWIN_DEST), L"번역중...");
			int length = SendMessage(GetDlgItem(hWnd, IDC_TRANSWIN_SRC), WM_GETTEXTLENGTH, 0, 0) + 1;
			wchar_t *pStr = (wchar_t *)malloc(sizeof(wchar_t) * (length + 1));

			GetDlgItemText(hWnd, IDC_TRANSWIN_SRC, pStr, length);

			std::wstring original_context = pStr;
			free(pStr);

			Cl.TextProcess->TranslateText(hWnd, original_context);

			SetFocus(GetDlgItem(hWnd, IDC_TRANSWIN_DEST));
		}
			break;
		case IDM_TRANS_START:
		{
			SetDlgItemText(hWnd, IDC_TRANSWIN_TRANSLATE, L"번역중지");
		}
			break;
		case IDM_TRANS_PROGRESS:
		{
			SetWindowText(GetDlgItem(hWnd, IDC_TRANSWIN_DEST), (LPCWSTR)lParam);
		}
			break;
		case IDM_TRANS_COMPLETE:
		case IDM_TRANS_ERROR:
		case IDM_TRANS_ABORT:
		{
			SetDlgItemText(hWnd, IDC_TRANSWIN_TRANSLATE, L"번역하기");
			SetWindowText(GetDlgItem(hWnd, IDC_TRANSWIN_DEST), (LPCWSTR)lParam);
		}
			break;
		case IDC_TRANSWIN_TRANS_ONEGO:
		{
			(Cl.Config->GetTransOneGo() ? Cl.Config->SetTransOneGo(false) : Cl.Config->SetTransOneGo(true));
		}
			break;
		case IDC_TRANSWIN_FILETRANS:
			break;
		case IDOK:
		case IDCANCEL:
		case IDM_EXIT:
		case IDC_TRANSWIN_CLOSE:
			Cl.TextProcess->TranslateAbort();
			//EndDialog(hWnd, LOWORD(wParam));
			DestroyWindow(hWnd);
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
		SendMessage(hWnds.Main, WM_COMMAND, IDM_SETTING_CHECK, 0);
		break;
	case WM_LBUTTONDOWN:
		SendMessage(hWnd, WM_NCLBUTTONDOWN, HTCAPTION, 0);
		break;
	case WM_ERASEBKGND:
		return false;
	case WM_MOVING:
	case WM_SIZING:
	{
		RECT *prc = (RECT *)lParam;
		SetWindowPos(hWnd, NULL, prc->left, prc->top, prc->right - prc->left, prc->bottom - prc->top, 0);
	}
		break;
	}
	return 0;
}


LRESULT CALLBACK ParentWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_PAINT:
		return 0;
	case WM_DESTROY:
	{
		SetParent(hWnd, NULL);
		return 1;
		//MessageBox(0,L"파괴 메세지",0,0);
	}
		break;
	}
	//return DefWindowProc(hWnd, message, wParam, lParam);

}