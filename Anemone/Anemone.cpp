// Anemone.cpp : 응용 프로그램에 대한 진입점을 정의합니다.
//

#include "stdafx.h"
#include "Anemone.h"

// 아네모네 버전
#define ANEMONE_VERSION 1000
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
ATOM				WindowClassRegister(HINSTANCE hInstance, wchar_t *szClassName, void *lpfnProc);
BOOL				InitInstance(HINSTANCE, int);
LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK	ParentWndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK	SettingProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK	About(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK	TransWinProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK	FileTransWinProc(HWND, UINT, WPARAM, LPARAM);
DWORD WINAPI		HttpSendRequestThread(LPVOID lpParam);
bool __stdcall		UpdateNotify(HWND hWnd, bool IsCurMsg);

unsigned int WINAPI MagneticThread(void *arg);
char* __stdcall J2K_Translate_Web(int data0, const char *jpStr);
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
	LoadString(hInstance, IDC_ANEMONEWND, szWindowClass, MAX_LOADSTRING);
	wcscpy(szSettingClass, L"Anemone_Setting_Class");
	wcscpy(szParentClass, L"AneParentClass");

	WindowClassRegister(hInstance, szWindowClass, WndProc);
	WindowClassRegister(hInstance, szParentClass, ParentWndProc);
	//WindowClassRegister(hInstance, szSettingClass,SettingProc);

	// 아네모네가 실행중인지 확인
	if (FindWindow(szWindowClass, 0))
	{
		HWND hMsgWnd = FindWindow(0, L"알림");
		if (!hMsgWnd)
			MessageBox(0, L"아네모네가 이미 실행중입니다", L"알림", MB_ICONINFORMATION);
		else SetForegroundWindow(hMsgWnd);
		return false;
	}

	// Heap 생성 (1MB)
	AneHeap = HeapCreate(0, 1024 * 1024, 0);

	// 환경설정 클래스
	Cl.Config = new CConfig();
	Cl.Config->LoadConfig();

	// 업데이트 체크
	if (Cl.Config->GetUpdateNotify())
		if (UpdateNotify(0, false)) return FALSE;

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
	TerminateThread(MagnetWnd.hWnd, 0);

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

HWND hForeWnd;
unsigned int WINAPI MagneticThread(void *arg)
{
	bool IsForegroundCheck = false;
	bool IsMinimizeOnce = false;
	RECT rect;
	//int nExStyle_Main, nExStyle_Target;
	hForeWnd = NULL;

	while (1)
	{
		// 부모 창이 날아갔을 때 긴급복구하기
		if (!IsWindow(hWnds.Parent))
		{
			SendMessage(hWnds.Main, WM_COMMAND, IDM_RESTORE_PARENT, 0);
			//MessageBox(0, L"복구", 0, 0);
		}

		// 메뉴 창에 WS_EX_NOACTIVATE 속성을 강제 부여
		HWND hMenuWnd = FindWindowEx(0, 0, L"#32768", L"");
		HWND CurFore = GetForegroundWindow();

		if (IsWindow(hMenuWnd))
		{
			DWORD dwProcessId;
			GetWindowThreadProcessId(hMenuWnd, &dwProcessId);

			if (GetCurrentProcessId() == dwProcessId)
			{
				int nExStyle_Menu = GetWindowLong(hMenuWnd, GWL_EXSTYLE);

				hForeWnd = CurFore;
				SetWindowText(hMenuWnd, L"AnemoneMenu");

				if (!(nExStyle_Menu & WS_EX_NOACTIVATE))
				{
					nExStyle_Menu |= WS_EX_NOACTIVATE;
					SetWindowLong(hMenuWnd, GWL_EXSTYLE, nExStyle_Menu);

					SetWindowLongPtr(hMenuWnd, -8, (LONG)hWnds.Parent);
					SetWindowPos(hMenuWnd, HWND_TOP, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE | SWP_NOACTIVATE);
				}

			}
		}

		// 자석모드 윈도우가 활성화시 아네모네도 위로 띄우기
		if (IsWindow(MagnetWnd.hWnd) && MagnetWnd.IsMagnet)
		{
			if (Cl.Config->GetMagneticMode())
			{
				if (GetForegroundWindow() == MagnetWnd.hWnd && !(GetWindowLong(MagnetWnd.hWnd, GWL_STYLE) & WS_MINIMIZE))
				{
					if (!IsForegroundCheck)
					{
						IsForegroundCheck = true;
						SetWindowPos(hWnds.Parent, HWND_TOP, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE | SWP_NOACTIVATE);
						SetWindowPos(hWnds.Main, HWND_TOP, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE | SWP_NOACTIVATE);

						// 팝업창을 위로 띄우기
						SetWindowPos(MagnetWnd.hWnd, HWND_TOP, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE | SWP_NOACTIVATE);
						
						Sleep(10);
					}
				}
				else
				{
					hForeWnd = GetForegroundWindow();
					IsForegroundCheck = false;
				}
			}
		}
		// 메뉴를 연 상태에서 포커스가 다른 창으로 옮겨가거나 다른 메뉴창이 발견되면 메뉴를 닫는다
		hMenuWnd = FindWindowEx(0, 0, L"#32768", L"AnemoneMenu");
		HWND hOtherWnd = FindWindowEx(0, 0, L"#32768", L"");
		
		DWORD dwOtherWndPID;
		GetWindowThreadProcessId(hOtherWnd, &dwOtherWndPID);

		if (IsWindow(hMenuWnd))
		{
			if (IsWindow(hOtherWnd) && dwOtherWndPID != GetCurrentProcessId() ||
				(CurFore != GetActiveWindow() && CurFore != 0 && hForeWnd != CurFore) || 
				Cl.Config->GetMagneticMode() && CurFore != MagnetWnd.hWnd)
			{
				SendMessage(hWnds.Main, WM_COMMAND, IDM_DESTROY_MENU, (LONG)hMenuWnd);
				CurFore = GetForegroundWindow();

				// 다른 프로세스의 팝업 메뉴를 위로 올리기
				SetWindowPos(hOtherWnd, HWND_TOP, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE | SWP_NOACTIVATE);
			}

		}
		
		if (IsWindow(MagnetWnd.hWnd) && MagnetWnd.IsMagnet)
		{
			if (Cl.Config->GetMagneticMode())
			{
				GetWindowRect(MagnetWnd.hWnd, &rect);
				
				// 자석 부모창의 항상 위가 바뀌면 아네모네도 따라서 바꿈
				/*bool CurrentTopMost = (GetWindowLong(MagnetWnd.hWnd, GWL_EXSTYLE) & WS_EX_TOPMOST ? TRUE : FALSE);
				if (MagnetWnd.IsTopMost != CurrentTopMost)
				{
					SetWindowPos(hWnds.Main, (CurrentTopMost ? HWND_TOPMOST : HWND_NOTOPMOST), 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE | SWP_NOOWNERZORDER);
					MagnetWnd.IsTopMost = CurrentTopMost;
					//MessageBox(0, (CurrentTopMost ? L"TOPMOST" : L"NOTOPMOST"), 0, 0);
				}*/

				bool CurrentTopMost = (GetWindowLong(MagnetWnd.hWnd, GWL_EXSTYLE) & WS_EX_TOPMOST ? TRUE : FALSE);
				if (MagnetWnd.IsTopMost != CurrentTopMost)
				{
					// 자석 타겟창에 맞춰서 항상위 변경
					// 이부분에 주기적으로 refresh가 가해짐
					int nExStyle_Main = GetWindowLong(hWnds.Parent, GWL_EXSTYLE);
					int nExStyle_Target = GetWindowLong(MagnetWnd.hWnd, GWL_EXSTYLE);

					MagnetWnd.IsTopMost = CurrentTopMost;
					//MessageBox(0, (CurrentTopMost ? L"TOPMOST" : L"NOTOPMOST"), 0, 0);

					if (MagnetWnd.IsTopMost)
					{
						SetWindowPos(hWnds.Parent, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE);
						SetWindowPos(hWnds.Main, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE);
					}
					else
					{
						SetWindowPos(hWnds.Parent, HWND_NOTOPMOST, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE);
						SetWindowPos(hWnds.Main, HWND_NOTOPMOST, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE);
					}

				}

				// 자석모드 부모창이 아네모네보다 앞에 있을 경우
				if (FindWindowEx(0, MagnetWnd.hWnd, szWindowClass, 0))
				{
					SetWindowPos(hWnds.Parent, HWND_TOP, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE);
					SetWindowPos(hWnds.Main, HWND_TOP, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE);
				}

				// 자석 부모창이 최소화되면 같이 최소화
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

				// 자석모드 부모창이 최소화되어 있으면 이동 처리를 하지 않는다
				if ((MagnetWnd.rect_x != rect.left || MagnetWnd.rect_y != rect.top) &&
					!(GetWindowLong(MagnetWnd.hWnd, GWL_STYLE) & WS_MINIMIZE))
				{
					// 자석모드 창 이동시 팝업 메뉴를 끈다
					if (IsWindow(hMenuWnd))
					{
						SendMessage(hWnds.Main, WM_COMMAND, IDM_DESTROY_MENU, (LONG)hMenuWnd);
					}


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

			SendMessage(hWnds.Main, WM_COMMAND, IDM_SETTING_CHECK, 0);
		}
		Sleep(1);
	}
	return 0;
}

//
// 우클릭 메뉴
//

VOID APIENTRY DisplayContextMenu(HWND hwnd, POINT pt)
{
	HMENU hmenu;
	HMENU hmenuTrackPopup;

	// 메뉴 리소스를 읽어온다
	if ((hmenu = LoadMenu(hInst, MAKEINTRESOURCE(IDC_ANEMONE))) == NULL)
		return;

	// 첫번째 숏컷 메뉴 핸들을 얻어온다
	hmenuTrackPopup = GetSubMenu(hmenu, 0);

	// 각 메뉴 아이템의 체크 여부를 선택한다

	CheckMenuItem(hmenuTrackPopup, IDM_CLIPBOARD_SWITCH, (Cl.Config->GetClipSwitch() ? MF_CHECKED : MF_UNCHECKED));
	CheckMenuItem(hmenuTrackPopup, IDM_WINDOW_THROUGH_CLICK, (Cl.Config->GetClickThough() ? MF_CHECKED : MF_UNCHECKED));
	CheckMenuItem(hmenuTrackPopup, IDM_MAGNETIC_MODE, (Cl.Config->GetMagneticMode() ? MF_CHECKED : MF_UNCHECKED));
	CheckMenuItem(hmenuTrackPopup, IDM_WND_BORDER_MODE, (Cl.Config->GetWndBorderMode() ? MF_CHECKED : MF_UNCHECKED));

	// 우클릭 메뉴 활성화
	TrackPopupMenu(hmenuTrackPopup,
		TPM_LEFTALIGN | TPM_RIGHTBUTTON,
		pt.x, pt.y, 0, hwnd, NULL);

	// 메뉴 소멸
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
ATOM WindowClassRegister(HINSTANCE hInstance, wchar_t *szClassName, void *lpfnWndProc)
{
	WNDCLASSEX wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = (WNDPROC)lpfnWndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;
	wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ANEMONE));
	wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wcex.lpszMenuName = 0;
	wcex.lpszClassName = szClassName;
	wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

	return RegisterClassEx(&wcex);
}

void TransWndMenu(HMENU hMenu)
{
	MENUITEMINFO mii;
	std::wstringstream ws;
	int max = GetMenuItemCount(hMenu);
	wchar_t *buf;

	ZeroMemory(&mii, sizeof(MENUITEMINFO));

	for (int i = 0; i < GetMenuItemCount(hMenu); i++)
	{
		HMENU hSubMenu = GetSubMenu(hMenu, i);
		if (hSubMenu != NULL) TransWndMenu(hSubMenu);

		mii.dwTypeData = 0;
		mii.fMask = MIIM_TYPE;
		mii.cbSize = sizeof(MENUITEMINFO);
		GetMenuItemInfo(hMenu, i, true, &mii);

		mii.cch = mii.cch + 1;
		buf = (wchar_t *)HeapAlloc(AneHeap, 0, (mii.cch + 1) * 2);
		
		buf[0] = 0x00;

		mii.dwTypeData = buf;
		mii.fMask = MIIM_STRING;
		GetMenuItemInfo(hMenu, i, true, &mii);

		//std::wstring str = buf;
		std::wstring str = Cl.TextProcess->eztrans_proc(buf);
		mii.dwTypeData = (wchar_t *)str.c_str();
		SetMenuItemInfo(hMenu, i, true, &mii);
		HeapFree(AneHeap, 0, buf);
	}

}
void TransWndText(HWND hTarget)
{
	HWND m_hWnd;
	wchar_t buf[255];

	m_hWnd = FindWindowEx(hTarget, NULL, NULL, NULL);
	while (m_hWnd != NULL)
	{
		if (FindWindowEx(m_hWnd, NULL, NULL, NULL)) TransWndText(m_hWnd);

		GetWindowText(m_hWnd, buf, 255);
		//MessageBox(0, buf, 0, 0);
		std::wstring str = Cl.TextProcess->eztrans_proc(buf);

		SetWindowText(m_hWnd, str.c_str());
		m_hWnd = FindWindowEx(hTarget, m_hWnd, NULL, NULL);
		
		InvalidateRect(m_hWnd, NULL, TRUE);
		UpdateWindow(m_hWnd);
	}
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

   //hWnds.Parent = CreateWindowEx(WS_EX_TOOLWINDOW | WS_EX_TOPMOST, szParentClass, L"아네모네", WS_POPUP,
	//   0, 0, 0, 0, NULL, NULL, hInstance, NULL);

   hWnds.Parent = (HWND)2;

   hWnds.Main = CreateWindowEx(WS_EX_TOOLWINDOW | WS_EX_NOACTIVATE | WS_EX_LAYERED | WS_EX_TOPMOST, szWindowClass, szTitle, WS_POPUP,
      (cx-x)/2, (cy-y)/2, x, y, NULL, NULL, hInstance, NULL);

   SetWindowLongPtr(hWnds.Main, -8, (LONG)hWnds.Parent);
   SetWindowLongPtr(hWnds.Main, -8, (LONG)NULL);

//   SetParent(hWnds.Main, hWnds.Parent);

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
		// 자석모드로 물려놓은 프로세스가 죽으면 부모창이 통째로 날라가기 때문에 살려놔야함
		case IDM_RESTORE_PARENT:
		{
			hWnds.Parent = CreateWindowEx(WS_EX_TOOLWINDOW | WS_EX_TOPMOST, szParentClass, L"아네모네", WS_POPUP,
			0, 0, 0, 0, NULL, NULL, hInst, NULL);

			SetWindowLongPtr(hWnds.Main, -8, (LONG)hWnds.Parent);
		}
		break;
		case IDM_ABOUT:
			DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
			break;
		case IDM_EXIT:
		case IDM_TERMINATE_ANEMONE:
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

				// 창이 뒤에 있을때 가려지므로 이 문제를 보완
				if (Cl.Config->GetWindowTopMost())
					SetWindowPos(hWnds.Main, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE);
				else SetWindowPos(hWnds.Main, HWND_TOP, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE);

				//int nExStyle_Target = GetWindowLong(MagnetWnd.hWnd, GWL_EXSTYLE);
				//SetWindowPos(hWnds.Main, (nExStyle_Target & WS_EX_TOPMOST ? HWND_TOPMOST : HWND_NOTOPMOST), 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE);

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

				// 창이 뒤에 있을때 가려지므로 이 문제를 보완
				if (Cl.Config->GetWindowTopMost())
					SetWindowPos(hWnds.Main, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE);
				else SetWindowPos(hWnds.Main, HWND_TOP, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE);

				//int nExStyle_Target = GetWindowLong(MagnetWnd.hWnd, GWL_EXSTYLE);
				//SetWindowPos(hWnds.Main, (nExStyle_Target & WS_EX_TOPMOST ? HWND_TOPMOST : HWND_NOTOPMOST), 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE);
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

			DWORD dwProcessId;
			GetWindowThreadProcessId(GetForegroundWindow(), &dwProcessId);

			HANDLE Handle = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, dwProcessId);
			wchar_t *lpszProcName = (wchar_t *)HeapAlloc(AneHeap, 0, 260);

			// 프로세스 경로를 얻어옵니다
			if (Handle && GetModuleFileNameEx(Handle, 0, lpszProcName, 255))
			{
				lpszProcName = _wcslwr(lpszProcName);

				// explorer.exe 자석 모드 불가
				if (wcsstr(lpszProcName, L"explorer.exe\0") != NULL)
				{
					HeapFree(AneHeap, 0, lpszProcName);
					break;
				}

			}
			else
			{
				HeapFree(AneHeap, 0, lpszProcName);
				break;
			}
			HeapFree(AneHeap, 0, lpszProcName);

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
				MagnetWnd.IsMinimize = false;
				MagnetWnd.IsTopMost = true;
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

				hWnds.Setting = CreateDialog(hInst, MAKEINTRESOURCE(IDD_SETTING), hWnds.Main, SettingProc);

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
				CheckDlgButton(hWnds.Setting, IDC_SETTING_UPDATE_NOTIFY, Cl.Config->GetUpdateNotify());

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
		case IDM_WINDOW_FILETRANS:
		{
			if (IsWindow(hWnds.FileTrans) == false)
			{
				RECT rect;
				int cx = GetSystemMetrics(SM_CXSCREEN);
				int cy = GetSystemMetrics(SM_CYSCREEN);

				hWnds.FileTrans = CreateDialog(hInst, MAKEINTRESOURCE(IDD_FILE_TRANSWIN), hWnd, FileTransWinProc);

				GetWindowRect(hWnds.FileTrans, &rect);

				SetWindowPos(hWnds.FileTrans, 0, (cx - rect.right + rect.left) / 2, (cy - rect.bottom + rect.top) / 2, 0, 0, SWP_NOSIZE);
				ShowWindow(hWnds.FileTrans, 1);
			}
			else
			{
				DestroyWindow(hWnds.FileTrans);
				hWnds.FileTrans = NULL;
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

		case IDM_TRANSTEXT_WNDMENU:
		{
			HWND hTargetWnd = GetForegroundWindow();

			// 아네모네 창은 번역하지 않는다
			if (hTargetWnd == GetActiveWindow()) break;

			HMENU hMenu = GetMenu(hTargetWnd);
			TransWndMenu(hMenu);

			SetMenu(hTargetWnd, hMenu);
		}
			break;
		case IDM_TRANSTEXT_WNDTEXT:
		{
			wchar_t buf[255];
			HWND hTargetWnd = GetForegroundWindow();

			// 아네모네 창은 번역하지 않는다
			if (hTargetWnd == GetActiveWindow()) break;

			GetWindowText(hTargetWnd, buf, 255);

			std::wstring str = Cl.TextProcess->eztrans_proc(buf);
			SetWindowText(hTargetWnd, str.c_str());

			TransWndText(hTargetWnd);			
		}
			break;
		case IDM_DESTROY_MENU:
		{
			CloseWindow((HWND)lParam);
		}
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
	}
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
	case WM_ERASEBKGND:
		return false;
	case WM_GETMINMAXINFO:
	{
		MINMAXINFO *mm = (MINMAXINFO *)lParam;

		mm->ptMinTrackSize.x = 90;
		mm->ptMinTrackSize.y = 90;
	}
		break;
	case WM_LBUTTONDOWN:
		SendMessage(hWnd, WM_NCLBUTTONDOWN, HTCAPTION, 0);
		break;
	case WM_NCLBUTTONDOWN:
		// 자석모드일때 아네모네 윈도우를 클릭하면 포커스를 자석모드창으로 돌린다
		if (Cl.Config->GetMagneticMode() && IsWindow(MagnetWnd.hWnd) && GetForegroundWindow() != MagnetWnd.hWnd)
		{
			SetForegroundWindow(MagnetWnd.hWnd);
			//SetWindowPos(MagnetWnd.hWnd, HWND_TOP, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE | SWP_NOACTIVATE);
		}
		return DefWindowProc(hWnd, message, wParam, lParam);
	case WM_RBUTTONDOWN:
		break;
	case WM_NCRBUTTONUP:
		SendMessage(hWnd, WM_CONTEXTMENU, wParam, lParam);
		break;
	case WM_CONTEXTMENU:
	{
		POINT pt;
		
		// 자석모드일때 아네모네 윈도우를 클릭하면 포커스를 자석모드창으로 돌린다
		if (Cl.Config->GetMagneticMode() && IsWindow(MagnetWnd.hWnd) && GetForegroundWindow() != MagnetWnd.hWnd)
		{
			SetForegroundWindow(MagnetWnd.hWnd);
			//SetWindowPos(MagnetWnd.hWnd, HWND_TOP, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE | SWP_NOACTIVATE);
		}

		pt.x = LOWORD(lParam);
		pt.y = HIWORD(lParam);
		if (!OnContextMenu(hWnd, pt.x, pt.y))
			return DefWindowProc(hWnd, message, wParam, lParam);
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
	case WM_LBUTTONDOWN:
		SendMessage(hWnd, WM_NCLBUTTONDOWN, HTCAPTION, 0);
		break;
	case WM_SHOWWINDOW:
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
		case IDC_SETTING_UPDATE_NOTIFY:
			(Cl.Config->GetUpdateNotify() ? Cl.Config->SetUpdateNotify(false) : Cl.Config->SetUpdateNotify(true));
			break;
		case IDC_SETTING_UPDATE_CHECK:
			UpdateNotify(hWnd, true);
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
		SendMessage(hWnds.Main, WM_COMMAND, IDM_SETTING_CHECK, 0);
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
		SetFocus(GetDlgItem(hWnd, IDC_TRANSWIN_SRC));
		SetForegroundWindow(hWnds.Trans);
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
			wchar_t *pStr = (wchar_t *)HeapAlloc(AneHeap, 0, sizeof(wchar_t) * (length + 1));
			GetDlgItemText(hWnd, IDC_TRANSWIN_DEST, pStr, length);

			IsActive = 2;

			OpenClipboard(hWnds.Clip);
			EmptyClipboard();

			SetClipboardData(CF_UNICODETEXT, pStr);
			CloseClipboard();

			HeapFree(AneHeap, 0, pStr);
		}
			break;
		case IDC_TRANSWIN_TRANSLATE:
		{
			Cl.TextProcess->TranslateAbort();

			SetFocus(GetDlgItem(hWnd, IDC_TRANSWIN_DEST));
			SetWindowText(GetDlgItem(hWnd, IDC_TRANSWIN_DEST), L"번역중...");
			int length = SendMessage(GetDlgItem(hWnd, IDC_TRANSWIN_SRC), WM_GETTEXTLENGTH, 0, 0) + 1;
			wchar_t *pStr = (wchar_t *)HeapAlloc(AneHeap, 0, sizeof(wchar_t) * (length + 1));

			GetDlgItemText(hWnd, IDC_TRANSWIN_SRC, pStr, length);

			std::wstring original_context = pStr;
			HeapFree(AneHeap, 0, pStr);

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
		case IDOK:
		case IDCANCEL:
		case IDC_TRANSWIN_CLOSE:
		{
			SetWindowLong(GetDlgItem(hWnds.Trans, IDC_TRANSWIN_SRC), GWL_WNDPROC, (LONG)pEditProc[0]);
			SetWindowLong(GetDlgItem(hWnds.Trans, IDC_TRANSWIN_DEST), GWL_WNDPROC, (LONG)pEditProc[1]);
			hWnds.Trans = NULL;

			Cl.TextProcess->TranslateAbort();
			//EndDialog(hWnd, LOWORD(wParam));
			DestroyWindow(hWnd);
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
	}
	return 0;
}


INT_PTR CALLBACK FileTransWinProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	int wmId, wmEvent;

	switch (message)
	{
	case WM_SHOWWINDOW:
	{
		SetForegroundWindow(hWnds.FileTrans);
		if (Cl.Config->GetFileTransOutput() == 0)
		{
			CheckDlgButton(hWnds.FileTrans, IDC_FILE_TRANSWIN_OUTPUT1, true);
			CheckDlgButton(hWnds.FileTrans, IDC_FILE_TRANSWIN_OUTPUT2, false);
		}
		else
		{
			CheckDlgButton(hWnds.FileTrans, IDC_FILE_TRANSWIN_OUTPUT1, false);
			CheckDlgButton(hWnds.FileTrans, IDC_FILE_TRANSWIN_OUTPUT2, true);
		}
	}
		break;
	case WM_COMMAND:
		wmId = LOWORD(wParam);
		wmEvent = HIWORD(wParam);
		// 메뉴 선택을 구문 분석합니다.
		switch (wmId)
		{
		case IDC_FILE_TRANSWIN_LOAD_BROWSER:
		{
			OPENFILENAME ofn;       // common dialog box structure
			wchar_t szFile[260];       // buffer for file name
			HANDLE hf;              // file handle

			// Initialize OPENFILENAME
			ZeroMemory(&ofn, sizeof(ofn));
			ofn.lStructSize = sizeof(ofn);
			ofn.hwndOwner = hWnd;
			ofn.lpstrFile = szFile;
			// Set lpstrFile[0] to '\0' so that GetOpenFileName does not 
			// use the contents of szFile to initialize itself.
			ofn.lpstrFile[0] = L'\0';
			ofn.nMaxFile = sizeof(szFile);
			ofn.lpstrFilter = L"텍스트 파일\0*.TXT\0모든 파일\0*.*\0";
			ofn.nFilterIndex = 1;
			ofn.lpstrFileTitle = NULL;
			ofn.nMaxFileTitle = 0;
			ofn.lpstrInitialDir = NULL;
			ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

			GetDlgItemText(hWnd, IDC_FILE_TRANSWIN_LOAD, szFile, 255);

			// Display the Open dialog box. 

			if (GetOpenFileName(&ofn) == TRUE)
			{
				std::wstring saveFile = ofn.lpstrFile;
				saveFile = saveFile.substr(0, saveFile.rfind(L'.'));
				saveFile += L"-번역.txt";

				SetDlgItemText(hWnd, IDC_FILE_TRANSWIN_LOAD, ofn.lpstrFile);
				SetDlgItemText(hWnd, IDC_FILE_TRANSWIN_SAVE, saveFile.c_str());

				FILE *fp;
				wchar_t wstr[1024];

				if (_wfopen_s(&fp, ofn.lpstrFile, L"rt,ccs=UTF-8") != 0)
				{
					SetDlgItemText(hWnd, IDC_FILE_TRANSWIN_PREVIEW, L"! 파일을 열 수 없습니다.");
					return false;
				}

				std::wstring content;
				// 한줄씩 읽기
				for (int i = 0; fgetws(wstr, 1000, fp) != NULL; i++)
				{
					if (i > 0) content.append(L"\r\n");
					content.append(wstr);
					
					if (i == 6) break;
				}

				fclose(fp);

				SetDlgItemText(hWnd, IDC_FILE_TRANSWIN_PREVIEW, content.c_str());
			}
		}
			break;
		case IDC_FILE_TRANSWIN_SAVE_BROWSER:
		{
			OPENFILENAME ofn;       // common dialog box structure
			wchar_t szFile[260];       // buffer for file name
			HANDLE hf;              // file handle

			// Initialize OPENFILENAME
			ZeroMemory(&ofn, sizeof(ofn));
			ofn.lStructSize = sizeof(ofn);
			ofn.hwndOwner = hWnd;
			ofn.lpstrFile = szFile;
			// Set lpstrFile[0] to '\0' so that GetOpenFileName does not 
			// use the contents of szFile to initialize itself.
			ofn.lpstrFile[0] = L'\0';
			ofn.nMaxFile = sizeof(szFile);
			ofn.lpstrFilter = L"텍스트 파일\0*.TXT\0모든 파일\0*.*\0";
			ofn.nFilterIndex = 1;
			ofn.lpstrFileTitle = NULL;
			ofn.nMaxFileTitle = 0;
			ofn.lpstrInitialDir = NULL;
			ofn.Flags = OFN_PATHMUSTEXIST | OFN_OVERWRITEPROMPT;

			GetDlgItemText(hWnd, IDC_FILE_TRANSWIN_SAVE, szFile, 255);

			// Display the Open dialog box. 

			if (GetSaveFileName(&ofn) == TRUE)
				hf = CreateFile(ofn.lpstrFile,
				GENERIC_READ,
				0,
				(LPSECURITY_ATTRIBUTES)NULL,
				OPEN_EXISTING,
				FILE_ATTRIBUTE_NORMAL,
				(HANDLE)NULL);

			SetDlgItemText(hWnd, IDC_FILE_TRANSWIN_SAVE, ofn.lpstrFile);
		}
			break;
		case IDC_FILE_TRANSWIN_TRANSLATE:
		{
			SetDlgItemText(hWnd, IDC_FILE_TRANSWIN_PREVIEW, L"번역 누름");
		}
			break;
		case IDOK:
		case IDCANCEL:
		{
			hWnds.FileTrans = NULL;
			DestroyWindow(hWnd);
		}
			break;
		case IDC_FILE_TRANSWIN_OUTPUT1:
			Cl.Config->SetFileTransOutput(0);
			break;
		case IDC_FILE_TRANSWIN_OUTPUT2:
			Cl.Config->SetFileTransOutput(1);
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
	case WM_NCDESTROY:
	case WM_QUIT:
	case WM_CLOSE:
	case WM_DESTROY:
	{
		SetParent(hWnd, NULL);
		return 1;
		
	}
		break;
	}
	return DefWindowProc(hWnd, message, wParam, lParam);

}

bool __stdcall UpdateNotify(HWND hWnd, bool IsCurMsg)
{
	wchar_t *lpwszNULL = L"";

	DWORD   dwTimeout = 5000;
	DWORD   dwExitCode = 0;
	PARM    threadParm;

	HINTERNET hInternet = InternetOpenA("HTTPEX", INTERNET_OPEN_TYPE_PRECONFIG, NULL, NULL, 0);
	HINTERNET hURL = InternetConnectA(hInternet, "docs.google.com", 80, NULL, NULL, INTERNET_SERVICE_HTTP, 0, 0);
	HINTERNET hRequest = HttpOpenRequestA(hURL, "GET", "/document/d/1AiZgmm78sj1ZqVurKBNKBM5ZUiEUU8sAp6f2O2ZQCfM/pub", "HTTP/1.1", NULL, NULL, 0, INTERNET_FLAG_RELOAD);

	HANDLE   hThread;
	DWORD    dwThreadID;

	int ver;
	char *strBuf;
	DWORD dwSize;
	DWORD dwBytesRead = 0;

	std::string StrContext;

	char *pStr;

	char *lpszVER;
	char *lpszDOWN;
	char *lpszMEMO;

	wchar_t *lpwszDOWN;
	wchar_t *lpwszMEMO;

	int nLen;

	threadParm.hInternet = hInternet;
	threadParm.hRequest = hRequest;
	threadParm.hURL = hURL;
	threadParm.strHeaders = "";
	threadParm.szPostData = "";


	hThread = CreateThread (NULL, 0, HttpSendRequestThread,	&threadParm, 0, &dwThreadID);
	if ((WaitForSingleObject(hThread, dwTimeout) == WAIT_TIMEOUT) && !GetExitCodeThread(hThread, &dwExitCode) && dwExitCode)
	{
		if (hInternet) InternetCloseHandle(hInternet);
		if (hURL) InternetCloseHandle(hURL);
		if (hRequest) InternetCloseHandle(hRequest);

		MessageBox(hWnd, L"아네모네 버전 확인 실패", L"업데이트 확인", MB_ICONASTERISK);
		return false;
	}

	CloseHandle(hThread);

	InternetQueryDataAvailable(hRequest, &dwSize, 0, 0);

	while (dwSize != 0)
	{
		// 인터넷 정보 받아오기 실패
		if (dwSize == 0)
		{
			break;
		}

		strBuf = (char *)HeapAlloc(AneHeap, 0, dwSize);

		InternetReadFile(hRequest, strBuf, dwSize, &dwBytesRead);

		if (dwBytesRead != 0)
		{
			StrContext.append((char *)strBuf, dwBytesRead);
			InternetQueryDataAvailable(hRequest, &dwSize, 0, 0);
			HeapFree(AneHeap, 0, strBuf);
		}
		else
		{
			HeapFree(AneHeap, 0, strBuf);
			break;
		}
		
	}
	
	pStr = strstr((char *)StrContext.c_str(), "ANEMONE_VERSION");

	if (pStr != NULL)
	{
		nLen = (int)strstr(pStr, "]") - (int)strstr(pStr, "[") - 1;
		if (nLen > 0)
		{
			lpszVER = (char *)HeapAlloc(AneHeap, 0, nLen+1);
			memcpy(lpszVER, strstr(pStr, "[") + 1, nLen);
			lpszVER[nLen] = 0x00;
		}
		else
		{
			lpszVER = (char *)HeapAlloc(AneHeap, 0, 1);
			lpszVER[0] = 0x00;
		}

		pStr = strstr(pStr, "{DOWN:");

		nLen = (int)strstr(pStr, "}") - (int)strstr(pStr, "{DOWN:") - 6;
		if (nLen > 0 && nLen != (int)strstr(pStr, "}") - 6)
		{
			lpszDOWN = (char *)HeapAlloc(AneHeap, 0, nLen + 1);
			memcpy(lpszDOWN, strstr(pStr, "{DOWN:") + 6, nLen);
			lpszDOWN[nLen] = 0x00;
		}
		else
		{
			lpszDOWN = (char *)HeapAlloc(AneHeap, 0, 1);
			lpszDOWN[0] = 0x00;
		}

		pStr = strstr(pStr, "{MEMO:");

		nLen = (int)strstr(pStr, "}") - (int)strstr(pStr, "{MEMO:") - 6;
		if (nLen > 0 && nLen != (int)strstr(pStr, "}") - 6)
		{
			lpszMEMO = (char *)HeapAlloc(AneHeap, 0, nLen + 3);
			memcpy(lpszMEMO, strstr(pStr, "{MEMO:") + 6, nLen);
			lpszMEMO[nLen] = 0x00;
			for (int i = 0, j = 0; i <= nLen + 2; i++, j++)
			{
				if (i == nLen + 2) lpszMEMO[j] = 0x00;
				else if (lpszMEMO[i] == '\\' && lpszMEMO[i + 1] == 'r')
				{
					i++;
					lpszMEMO[j] = '\r';
					continue;
				}
				else if (lpszMEMO[i] == '\\' && lpszMEMO[i + 1] == 'n')
				{
					i++;
					lpszMEMO[j] = '\n';
					continue;
				}
				lpszMEMO[j] = lpszMEMO[i];
			}
		}
		else
		{
			lpszMEMO = (char *)HeapAlloc(AneHeap, 0, 1);
			lpszMEMO[0] = 0x00;
		}
	}

	if (hInternet) InternetCloseHandle(hInternet);
	if (hURL) InternetCloseHandle(hURL);
	if (hRequest) InternetCloseHandle(hRequest);

	ver = atoi(lpszVER);

	nLen = MultiByteToWideChar(CP_UTF8, 0, lpszDOWN, strlen(lpszDOWN) + 1, NULL, NULL);
	lpwszDOWN = (wchar_t *)HeapAlloc(AneHeap, 0, (nLen + 1) * 2);
	MultiByteToWideChar(CP_UTF8, 0, lpszDOWN, strlen(lpszDOWN) + 1, lpwszDOWN, nLen);

	nLen = MultiByteToWideChar(CP_UTF8, 0, lpszMEMO, strlen(lpszMEMO) + 1, NULL, NULL);
	lpwszMEMO = (wchar_t *)HeapAlloc(AneHeap, 0, (nLen + 1) * 2);
	MultiByteToWideChar(CP_UTF8, 0, lpszMEMO, strlen(lpszMEMO) + 1, lpwszMEMO, nLen);

	std::wstringstream wss;

	wss << L"아네모네 새로운 버전을 확인했습니다.\r\n홈페이지로 이동할까요?";
	wss << L"\r\n\r\n";
	wss << lpwszMEMO;

	if (ver > ANEMONE_VERSION)
	{
		if (MessageBox(hWnd, wss.str().c_str(), L"업데이트 확인", MB_ICONINFORMATION | MB_YESNO) == IDYES)
		{
			ShellExecute(NULL, L"open", lpwszDOWN, L"", L"", SW_SHOW);

			HeapFree(AneHeap, 0, lpszVER);
			HeapFree(AneHeap, 0, lpszDOWN);
			HeapFree(AneHeap, 0, lpszMEMO);
			HeapFree(AneHeap, 0, lpwszDOWN);
			HeapFree(AneHeap, 0, lpwszMEMO);
			return true;
		}
	}
	else if (ver == 0)
	{
		MessageBox(hWnd, L"아네모네 버전 확인 실패", L"업데이트 확인", MB_ICONASTERISK);
	}
	else if (IsCurMsg == true)
	{
		MessageBox(hWnd, L"아네모네가 최신 버전입니다", L"업데이트 확인", MB_ICONINFORMATION);
	}

	HeapFree(AneHeap, 0, lpszVER);
	HeapFree(AneHeap, 0, lpszDOWN);
	HeapFree(AneHeap, 0, lpszMEMO);
	HeapFree(AneHeap, 0, lpwszDOWN);
	HeapFree(AneHeap, 0, lpwszMEMO);
	return false;
}
char* __stdcall J2K_Translate_Web(int data0, const char *jpStr)
{
	//char *szActData = "%EC%9D%B4%EC%A7%80%ED%8A%B8%EB%9E%9C%EC%8A%A4";
	//wchar_t *szInput = L"こんにちは";

	char *szParameter = "mode=j2k&body=%s";

	char *strHeaders = "Content-Type: application/x-www-form-urlencoded";
	char szPostData[65500] = "";
	char szEncodeData[200] = "";

	wchar_t wstr1[65500];
	char str1[65500] = "";
	char str2[65500] = "";

	char *buf;

	DWORD   dwTimeout;
	DWORD   dwExitCode;
	PARM    threadParm;

	// 버퍼 할당
	buf = (char *)malloc(65500 + (strlen(jpStr) + 1)*sizeof(char));
	if (buf == NULL)
	{
		MessageBox(0, L"[Memory] 메모리를 할당하지 못했습니다.\n", 0, 0);
	}

	//	sprintf(tempstr, "1\0", urltemp2);

	//WriteLog(L"[WebTrans]: %s\n", jpStr);

	// 932로 들어온걸 유니코드로 바꿈
	int nLen = MultiByteToWideChar(932, MB_PRECOMPOSED, jpStr, -1, NULL, NULL);
	MultiByteToWideChar(932, 0, jpStr, -1, wstr1, nLen);

	// 유니코드를 UTF-8로 바꿈
	nLen = WideCharToMultiByte(CP_UTF8, 0, wstr1, lstrlenW(wstr1), NULL, 0, NULL, NULL);
	WideCharToMultiByte(CP_UTF8, 0, wstr1, lstrlenW(wstr1), str1, nLen, NULL, NULL);

	for (int i = 0; i<nLen; i++)
	{
		sprintf_s(str2, "%s%%%02X\0", str2, str1[i] & 0xFF);
	}

	sprintf_s(szPostData, szParameter, str2);

	HINTERNET hInternet = InternetOpenA("HTTPFILE", INTERNET_OPEN_TYPE_DIRECT, NULL, NULL, 0);
	HINTERNET hURL = InternetConnectA(hInternet, "jptrans.naver.net", 80, "HTTP/1.1", NULL, INTERNET_SERVICE_HTTP, 0, 0);
	HINTERNET hRequest = HttpOpenRequestA(hURL, "POST", "/short_trans/translate_300_japan_service_trans.php", NULL, NULL, NULL, 0, 0);


	HANDLE   hThread;
	DWORD    dwThreadID;
	threadParm.hInternet = hInternet;
	threadParm.hRequest = hRequest;
	threadParm.hURL = hURL;
	threadParm.strHeaders = strHeaders;
	threadParm.szPostData = szPostData;


	//	   WriteLog(L"CreateThread\n");
	hThread = CreateThread(
		NULL,            // Pointer to thread security attributes 
		0,               // Initial thread stack size, in bytes 
		HttpSendRequestThread,  // Pointer to thread function 
		&threadParm,     // The argument for the new thread
		0,               // Creation flags 
		&dwThreadID      // Pointer to returned thread identifier 
		);           // Wait for the call to InternetConnect in worker function to complete
	dwTimeout = 5000; // in milliseconds
	if (WaitForSingleObject(hThread, dwTimeout) == WAIT_TIMEOUT)
	{
		std::wstring strResult = L"[WebTrans] 인터넷 연결시간이 초과되었습니다: HttpSendRequestA Timeout Error";

		int len = WideCharToMultiByte(949, 0, strResult.c_str(), -1, NULL, NULL, NULL, NULL);
		WideCharToMultiByte(949, 0, strResult.c_str(), -1, buf, len, NULL, NULL);

		if (hInternet) InternetCloseHandle(hInternet);
		if (hURL) InternetCloseHandle(hURL);
		if (hRequest) InternetCloseHandle(hRequest);

		MessageBox(0, L"[WebTrans] 인터넷 연결시간이 초과되었습니다: HttpSendRequestA Timeout Error\n", 0, 0);
		return buf;
	}


	// The state of the specified object (thread) is signaled
	dwExitCode = 0;
	if (!GetExitCodeThread(hThread, &dwExitCode))
	{
		std::wstring strResult = L"[WebTrans] GetExitCodeThread Error";

		int len = WideCharToMultiByte(949, 0, strResult.c_str(), -1, NULL, NULL, NULL, NULL);
		WideCharToMultiByte(949, 0, strResult.c_str(), -1, buf, len, NULL, NULL);

		MessageBox(0, L"[WebTrans] GetExitCodeThread Error\n", 0, 0);

		if (hInternet) InternetCloseHandle(hInternet);
		if (hURL) InternetCloseHandle(hURL);
		if (hRequest) InternetCloseHandle(hRequest);
		return buf;
	}
	CloseHandle(hThread);

	if (dwExitCode)
	{
		MessageBox(0, L"[WebTrans] HttpSendRequestA Error\n", 0, 0);

		if (hInternet) InternetCloseHandle(hInternet);
		if (hURL) InternetCloseHandle(hURL);
		if (hRequest) InternetCloseHandle(hRequest);
		return buf;
	}

	DWORD dwContentLen = 0;
	DWORD dwBufLen = sizeof(dwContentLen);
	BOOL bRet2 = HttpQueryInfo(
		hRequest,
		HTTP_QUERY_CONTENT_LENGTH | HTTP_QUERY_FLAG_NUMBER,
		(LPVOID)&dwContentLen,
		&dwBufLen,
		0
		);

	char readBuffer[6000];

	memset(readBuffer, 0, 6000);
	DWORD dwRecvLen = 0;
	DWORD dwBytesRead = 0;

	BOOL bRet3 = InternetReadFile(hRequest, readBuffer, sizeof(readBuffer), &dwBytesRead);
	if (!bRet3 || dwBytesRead == 0)
	{
		MessageBox(0, L"[WebTrans] InternetReadFile Err\n", 0, 0);
	}

	nLen = MultiByteToWideChar(CP_UTF8, 0, readBuffer, strlen(readBuffer) + 1, NULL, NULL);
	MultiByteToWideChar(CP_UTF8, 0, readBuffer, strlen(readBuffer) + 1, wstr1, nLen);

	nLen = WideCharToMultiByte(949, 0, wstr1, -1, NULL, 0, NULL, NULL);
	WideCharToMultiByte(949, 0, wstr1, -1, str1, nLen, NULL, NULL);

	if (hInternet) InternetCloseHandle(hInternet);
	if (hURL) InternetCloseHandle(hURL);
	if (hRequest) InternetCloseHandle(hRequest);

	if (strstr(str1, "result") != NULL)
	{
		char *first;
		char *end;

		first = strchr(str1, '"');
		end = strrchr(str1, '"');

		memcpy(buf, first + 1, end - first);
		buf[end - first - 1] = 0x00;
	}
	else
	{
		//std::wstring strResult = L"[WebTrans] 번역이 실패했습니다.";
		std::wstring strResult = L"";

		int len = WideCharToMultiByte(949, 0, strResult.c_str(), -1, NULL, NULL, NULL, NULL);
		WideCharToMultiByte(949, 0, strResult.c_str(), -1, buf, len, NULL, NULL);

		MessageBox(0, L"[WebTrans] 번역이 실패했거나 반환된 문자열이 없습니다.\n", 0, 0);
	}

	if (hInternet) InternetCloseHandle(hInternet);
	if (hURL) InternetCloseHandle(hURL);
	if (hRequest) InternetCloseHandle(hRequest);

	return buf;
}

DWORD WINAPI HttpSendRequestThread(LPVOID lpParam)
{
	PARM* pThreadParm;
	pThreadParm = (PARM*)lpParam;
	BOOL bRet = HttpSendRequestA(pThreadParm->hRequest, pThreadParm->strHeaders, strlen(pThreadParm->strHeaders), pThreadParm->szPostData, strlen(pThreadParm->szPostData));

	if (bRet == false)
	{
		//MessageBox(0, L"HttpSendRequestThread Failed\n", 0, 0);
		if (pThreadParm->hInternet) InternetCloseHandle(pThreadParm->hInternet);
		if (pThreadParm->hURL) InternetCloseHandle(pThreadParm->hURL);
		if (pThreadParm->hRequest) InternetCloseHandle(pThreadParm->hRequest);
		return 1; // 실패
	}
	return 0; // 성공
}