// Anemone.cpp : 응용 프로그램에 대한 진입점을 정의합니다.
//

#include "stdafx.h"
#include "Anemone.h"

// 아네모네 버전
#define ANEMONE_VERSION 994
#define MAX_LOADSTRING 100

// 전역 변수:
TCHAR szTitle[MAX_LOADSTRING];					// 제목 표시줄 텍스트입니다.
TCHAR szWindowClass[MAX_LOADSTRING];			// 기본 창 클래스 이름입니다.
TCHAR szSettingClass[MAX_LOADSTRING];
TCHAR szParentClass[MAX_LOADSTRING];
TCHAR szBackLogClass[MAX_LOADSTRING];

// 전역 단축키
std::vector<_key_map> key_map;

// 인스턴스, 핸들
HINSTANCE hInst;
_hWnds hWnds;
_MagnetWnd MagnetWnd;
_Class Cl;
HANDLE AneHeap;

// 아네모네 사전
std::vector<aneDicStruct> AneDic;

// 트레이 아이콘
NOTIFYICONDATA niData;
UINT WM_TASKBARCHANGED;
int nMenuType = 0;

// 이전글/최근글 보기
std::list<_viewLog> viewLog;
int viewLogNum = 0;

int IsActive = 0;
int Elapsed_Prepare = 0;
int Elapsed_Translate = 0;

// 아네모네 윈도우 저장
std::vector<_wndinfo> WndInfo;

ATOM				WindowClassRegister(HINSTANCE hInstance, wchar_t *szClassName, void *lpfnProc);
BOOL				InitInstance(HINSTANCE, int);
LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK	ParentWndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK	SettingProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK	About(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK	TransWinProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK	TransWinProgProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK	FileTransWinProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK	BackLogProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK	HookCfgProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
bool __stdcall		UpdateNotify(HWND hWnd, bool IsCurMsg);
DWORD WINAPI		HttpSendRequestThread(LPVOID lpParam);
DWORD WINAPI		FileTransThread(LPVOID lpParam);
void				CreateTrayIcon(HWND hWnd);
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

	MSG msg;
	HACCEL hAccelTable;

	// 전역 문자열을 초기화합니다.
	LoadString(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadString(hInstance, IDC_ANEMONEWND, szWindowClass, MAX_LOADSTRING);
	wcscpy(szSettingClass, L"Anemone_Setting_Class");
	wcscpy(szParentClass, L"AneParentClass");
	wcscpy(szBackLogClass, L"AneBackLogClass");

	WindowClassRegister(hInstance, szWindowClass, WndProc);
	WindowClassRegister(hInstance, szParentClass, ParentWndProc);
	WindowClassRegister(hInstance, szBackLogClass, BackLogProc);

	// 아네모네가 실행중인지 확인
	if (FindWindow(szWindowClass, 0) || FindWindow(szParentClass, 0))
	{
		HWND hMsgWnd = FindWindow(0, L"알림");
		if (!hMsgWnd)
			MessageBox(0, L"아네모네가 이미 실행중입니다", L"알림", MB_ICONINFORMATION);
		else SetForegroundWindow(hMsgWnd);
		return false;
	}

	hWnds.Parent = CreateWindowEx(WS_EX_TOOLWINDOW | WS_EX_TOPMOST, szParentClass, L"아네모네", WS_POPUP,
		0, 0, 0, 0, NULL, NULL, hInstance, NULL);

	// Heap 생성 (1MB)
	AneHeap = HeapCreate(0, 1024 * 1024, 0);

	// 환경설정 클래스
	Cl.Config = new CConfig();
	Cl.Config->LoadConfig();
	Cl.Config->LoadWndConfig();
	/*
	std::wregex regex_field(L"^(\\d+)x(\\d+)=(\\d+)[|](\\d+)[|](\\d+)[|](\\d+)(\\s+)");
	std::wstring str = L"123x456=789|123|456|781\r\n";
	std::wsmatch m;
	if (std::regex_match(str, m, regex_field))
	{
		MessageBox(0, L"X", 0, 0);
	}
	*/
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

	if (!GetEZTPath(szEnginePath))
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
	
	// 트레이 아이콘 생성
	CreateTrayIcon(hWnds.Main);

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
	ShowWindow(hWnds.Main, false);
	TerminateThread(MagnetWnd.hThread, 0);
	Shell_NotifyIcon(NIM_DELETE, &niData);
	
	Cl.Config->SaveWndConfig();
	Cl.Config->SaveConfig();

	DestroyWindow(hWnds.Parent);

	delete Cl.TextProcess;
	delete Cl.TextRenderer;
	delete Cl.Remocon;
	delete Cl.FileWatch;
	delete Cl.Hotkey;
	delete Cl.TransEngine;
	delete Cl.Config;
	
	// Heap 삭제
	HeapDestroy(AneHeap);
}

void CreateTrayIcon(HWND hWnd)
{
	// 트레이 아이콘 생성
	ZeroMemory(&niData, sizeof(NOTIFYICONDATA));
	niData.cbSize = sizeof(NOTIFYICONDATA);
	niData.uID = ID_TRAY_EVENT;
	niData.uFlags = NIF_ICON | NIF_MESSAGE | NIF_TIP;
	niData.hIcon = (HICON)LoadImage(hInst, MAKEINTRESOURCE(IDI_ANEMONE), IMAGE_ICON,
		GetSystemMetrics(SM_CXSMICON), GetSystemMetrics(SM_CYSMICON), LR_DEFAULTCOLOR);
	niData.hWnd = hWnd;
	niData.uCallbackMessage = ID_TRAY_EVENT;

	wcscpy_s(niData.szTip, L"아네모네");
	Shell_NotifyIcon(NIM_ADD, &niData);

	WM_TASKBARCHANGED = RegisterWindowMessage(L"TaskBarCreated");
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
	hForeWnd = NULL;

	while (1)
	{
		// 자석모드 타겟창이 강제 종료되면 부모창까지 같이 날아가는데 이를 복구한다
		// 복구하지 않으면 자석모드 걸면 아네모네가 부모창에 달라붙지 못함
		if (!IsWindow(hWnds.Parent))
			SendMessage(hWnds.Main, WM_COMMAND, ID_RESTORE_PARENT, 0);

		// 아네모네 메뉴 창에 NOACTIVATE 속성 부여
		HWND hMenuWnd = FindWindowEx(0, 0, L"#32768", L"");
		HWND CurFore = GetForegroundWindow();

		if (IsWindow(hMenuWnd))
		{
			DWORD dwProcessId;
			GetWindowThreadProcessId(hMenuWnd, &dwProcessId);

			// 아네모네 프로세스에만 적용
			if (GetCurrentProcessId() == dwProcessId)
			{
				int nExStyle_Menu = GetWindowLong(hMenuWnd, GWL_EXSTYLE);
				hForeWnd = CurFore;
				if (nMenuType == 1) SetWindowText(hMenuWnd, L"AnemoneTrayMenu");
				else
				{
					SetWindowText(hMenuWnd, L"AnemoneMenu");

					if (!(nExStyle_Menu & WS_EX_NOACTIVATE))
					{
						nExStyle_Menu |= WS_EX_NOACTIVATE;
						SetWindowLong(hMenuWnd, GWL_EXSTYLE, nExStyle_Menu);

						SetWindowLongPtr(hMenuWnd, -8, (LONG)hWnds.Parent);
						SetWindowPos(hMenuWnd, HWND_TOP, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE | SWP_NOACTIVATE);

						InvalidateRect(hMenuWnd, NULL, TRUE);
						UpdateWindow(hMenuWnd);
					}
				}
			}
		}

		// 자석모드 윈도우가 활성화되면 아네모네를 위로 띄움
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
		// 메뉴를 연 상태에서 포커스가 다른 창으로 바뀌거나
		// 다른 메뉴창이 발견되면 아네모네 메뉴를 닫는다
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
				SendMessage(hWnds.Main, WM_COMMAND, ID_DESTROY_MENU, (LONG)hMenuWnd);
				//hForeWnd = GetForegroundWindow();

				// 다른 프로세스의 팝업 메뉴를 위로 올리기
				SetWindowPos(hOtherWnd, HWND_TOP, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE | SWP_NOACTIVATE);

				InvalidateRect(hOtherWnd, NULL, TRUE);
				UpdateWindow(hOtherWnd);
			}

		}
		
		// 아네모네 창이 항상 위로 오도록
		if (!Cl.Config->GetMagneticMode() &&
			Cl.Config->GetWindowTopMost())
		{
			if (FindWindowEx(0, GetForegroundWindow(), szWindowClass, 0))
			{
				SetWindowPos(hWnds.Parent, HWND_TOP, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE);
				SetWindowPos(hWnds.Main, HWND_TOP, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE);
			}
		}

		if (IsWindow(MagnetWnd.hWnd) && MagnetWnd.IsMagnet)
		{
			if (Cl.Config->GetMagneticMode())
			{
				GetWindowRect(MagnetWnd.hWnd, &rect);
				
				// 자석 부모창의 항상 위 설정을 감시해서 아네모네도 자석 부모창의 항상위 설정에 따른다
				bool CurrentTopMost = (GetWindowLong(MagnetWnd.hWnd, GWL_EXSTYLE) & WS_EX_TOPMOST ? TRUE : FALSE);
				if (MagnetWnd.IsTopMost != CurrentTopMost)
				{
					int nExStyle_Main = GetWindowLong(hWnds.Parent, GWL_EXSTYLE);
					int nExStyle_Target = GetWindowLong(MagnetWnd.hWnd, GWL_EXSTYLE);

					// CurrentTopMost : 자석 부모창의 항상 위 설정
					MagnetWnd.IsTopMost = CurrentTopMost;

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

				// 자석모드 부모창이 아네모네보다 앞에 있을 경우 아네모네를 땡겨준다
				if (FindWindowEx(0, MagnetWnd.hWnd, szWindowClass, 0))
				{
					SetWindowPos(hWnds.Parent, HWND_TOP, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE);
					SetWindowPos(hWnds.Main, HWND_TOP, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE);
				}

				// 자석 부모창이 최소화되면 같이 최소화한다
				// 이 기능은 최소화 시점에서만 작동하므로 IsMinimizeOnce 변수로 컨트롤
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
							SendMessage(hWnds.Main, WM_COMMAND, ID_SETTING_CHECK, 0);
						}
					}
				}
				else 
				{
					if (IsMinimizeOnce)
					{
						// 자석 부모창이 최소화되었다가 복구된 상황인 경우
						if (MagnetWnd.IsMinimize == true)
						{
							Cl.Config->SetWindowVisible(true);
							ShowWindow(hWnds.Main, true);
						}

						// 다음 최소화시 기능이 작동하도록 변수 초기화
						MagnetWnd.IsMinimize = false;
						IsMinimizeOnce = false;
						SendMessage(hWnds.Main, WM_COMMAND, ID_SETTING_CHECK, 0);
					}
				}

				// 자석 부모창이 움직이면 따라서 움직이는 부분
				// 단 깨짐 방지를 위해 부모창이 최소화 상태일때는 창 이동을 하지 않음
				if ((MagnetWnd.rect_x != rect.left || MagnetWnd.rect_y != rect.top) &&
					!(GetWindowLong(MagnetWnd.hWnd, GWL_STYLE) & WS_MINIMIZE))
				{
					// 자석모드 창 이동시 아네모네 메뉴를 끔
					if (IsWindow(hMenuWnd))
					{
						SendMessage(hWnds.Main, WM_COMMAND, ID_DESTROY_MENU, (LONG)hMenuWnd);
					}

					DEVMODE dmCurrent, dmRegistry;

					EnumDisplaySettings(NULL, ENUM_CURRENT_SETTINGS, &dmCurrent);
					EnumDisplaySettings(NULL, ENUM_REGISTRY_SETTINGS, &dmRegistry);

					if (dmCurrent.dmPelsWidth == dmRegistry.dmPelsWidth &&
						dmCurrent.dmPelsHeight == dmRegistry.dmPelsHeight)
					//if (dmCurrent.dmPelsWidth == MagnetWnd.res_x &&
					//	dmCurrent.dmPelsHeight == MagnetWnd.res_y)
					{
						if (GetTickCount() > MagnetWnd.IgnoreTick)
							SetWindowPos(hWnds.Main, NULL, rect.left + MagnetWnd.diff_x, rect.top + MagnetWnd.diff_y, 0, 0, SWP_NOSIZE | SWP_NOOWNERZORDER);
						else
						{
							MagnetWnd.rect_x = rect.left;
							MagnetWnd.rect_y = rect.top;

							RECT AneRect;
							GetWindowRect(hWnds.Main, &AneRect);

							MagnetWnd.diff_x = AneRect.left - MagnetWnd.rect_x;
							MagnetWnd.diff_y = AneRect.top - MagnetWnd.rect_y;

						}
					}

					MagnetWnd.rect_x = rect.left;
					MagnetWnd.rect_y = rect.top;
				}
			}
		}
		//
		// 자석 모드 상태가 아닐 때
		else if (MagnetWnd.IsMagnet)
		{
			// 이전 자석 부모창 상태가 최소화였던 경우 창을 보이도록 한다
			if (MagnetWnd.IsMinimize)
			{
				Cl.Config->SetWindowVisible(true);
				ShowWindow(hWnds.Main, true);
				MagnetWnd.IsMinimize = false;
			}

			// 자석 최소화 변수 초기화
			MagnetWnd.hWnd = NULL;
			SetParent(hWnds.Parent, NULL);
			Cl.Config->SetMagneticMode(false);

			// 자석 모드 상태를 사용중이 아님으로 돌림
			MagnetWnd.IsMagnet = false;

			// 본래 설정대로 항상 위 설정 초기화
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

			SendMessage(hWnds.Main, WM_COMMAND, ID_SETTING_CHECK, 0);
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
	CheckMenuItem(hmenuTrackPopup, ID_CLIPBOARD_SWITCH, (Cl.Config->GetClipSwitch() ? MF_CHECKED : MF_UNCHECKED));
	CheckMenuItem(hmenuTrackPopup, ID_HOOKER_MONITOR, (Cl.Config->GetHookMonitor() ? MF_CHECKED : MF_UNCHECKED));
	CheckMenuItem(hmenuTrackPopup, ID_WINDOW_THROUGH_CLICK, (Cl.Config->GetClickThough() ? MF_CHECKED : MF_UNCHECKED));
	CheckMenuItem(hmenuTrackPopup, ID_MAGNETIC_MODE, (Cl.Config->GetMagneticMode() ? MF_CHECKED : MF_UNCHECKED));
	CheckMenuItem(hmenuTrackPopup, ID_WND_BORDER_MODE, (Cl.Config->GetWndBorderMode() ? MF_CHECKED : MF_UNCHECKED));
	CheckMenuItem(hmenuTrackPopup, ID_BACKGROUND_SWITCH, (Cl.Config->GetBGSwitch() ? MF_CHECKED : MF_UNCHECKED));

	CheckMenuRadioItem(hmenuTrackPopup, ID_TEMP_WINDOW_HIDE, ID_WINDOW_VISIBLE, (Cl.Config->GetTempWinHide() ? ID_TEMP_WINDOW_HIDE : (Cl.Config->GetWindowVisible() ? 0 : ID_WINDOW_VISIBLE)), MF_BYCOMMAND);

	// 우클릭 메뉴 활성화
	TrackPopupMenu(hmenuTrackPopup, TPM_LEFTALIGN | TPM_RIGHTBUTTON, pt.x, pt.y, 0, hwnd, NULL);

	// 메뉴 소멸
	DestroyMenu(hmenu);
}

BOOL WINAPI OnContextMenu(HWND hwnd, int x, int y)
{
	RECT rc;
	POINT pt = { x, y };

	GetClientRect(hwnd, &rc);
	ScreenToClient(hwnd, &pt);

	// 트레이 아이콘 메뉴를 열 때 메인 아네모네 메뉴가 떠 있다면 없앤다
	// 이부분이 정확히는 아네모네 창에서 메뉴가 열렸는지를 확인하는 함수
	if (PtInRect(&rc, pt)) nMenuType = 0;
	else
	{
		SendMessage(hwnd, WM_COMMAND, ID_DESTROY_MENU, 0);
		nMenuType = 1;
	}

	ClientToScreen(hwnd, &pt);
	DisplayContextMenu(hwnd, pt);
	return TRUE;
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
	wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_ANEMONE));

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
bool OpenDicText(HWND hWnd)
{
	std::wstring path;
	GetLoadPath(path, L"\\AneDic.txt");
	wchar_t *AneDicHeader =
	{ L"// 아네모네 사전파일 V1\r\n" \
		L"//\r\n" \
		L"// 찾는 단어 (tab) 바꾸는 단어 (tab) 명사/상용어구 (tab) 속성\r\n" \
		L"// 예시) 雪々	유키	1	VHn\r\n" \
		L"//\r\n" \
		L"// * 명사/상용어구 필드에는 단어가 상용어구일 경우 0, 명사일 경우 1을,\r\n"\
		L"// * 사전 우선 적용을 원하는 단어라면 2를 넣어주세요.\r\n" \
		L"// * 사전 우선 적용 옵션이 꺼져 있을 경우 우선 적용 단어는 다른 단어보다 먼저 처리됩니다.\r\n" \
		L"//\r\n" \
		L"// * 명사/상용어구와 속성은 생략할 수 있으며 쓰지 않으면 명사로 인식됩니다.\r\n" \
		L"// * 아네모네 사전 우선 적용 옵션을 켜거나 단어가 사전 우선 적용인 경우 무조건 상용어구로 처리됩니다.\r\n" \
		L"//\r\n" \
		L"// * 줄 앞에 주석(//)이나 공백이 있으면 적용되지 않습니다.\r\n" \
		L"// * 바꾸는 단어에 아무것도 적혀있지 않는 경우 찾은 단어를 삭제합니다.\r\n" \
		L"// * 각 단어의 길이는 최대 15자입니다.\r\n" \
		L"//\r\n\r\n" };

	FILE *fp;
	if (_wfopen_s(&fp, path.c_str(), L"rt,ccs=UTF-8") != 0)
	{
		// 아네모네 사전 파일이 없는 경우 초기 내용 작성
		if (_wfopen_s(&fp, path.c_str(), L"wt,ccs=UTF-8") == 0)
		{
			fwrite(AneDicHeader, sizeof(wchar_t), wcslen(AneDicHeader), fp);
			fclose(fp);
			//MessageBox(hWnd, L"기본 사전 파일이 생성되었습니다.\r\n열린 텍스트 파일의 설명을 읽어보시고 사용하세요.", L"알림", 0);
		}
		else
		{
			//MessageBox(hWnd, L"AneDic.txt 파일에 쓰기 권한이 없습니다.", 0, 0);
			return false;
		}
	}
	else fclose(fp);
	ExecuteFile(path);
	return true;
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

   int sm_cx = GetSystemMetrics(SM_CXSCREEN);
   int sm_cy = GetSystemMetrics(SM_CYSCREEN);

   int cx = 500;
   int cy = 200;

   int x = (sm_cx - cx) / 2;
   int y = (sm_cy - cy) / 2;

   DEVMODE dmREG, dmCUR;
   EnumDisplaySettings(NULL, ENUM_CURRENT_SETTINGS, &dmCUR);
   EnumDisplaySettings(NULL, ENUM_REGISTRY_SETTINGS, &dmREG);

   _wndinfo wi;
   wi.res_x = dmCUR.dmPelsWidth;
   wi.res_y = dmCUR.dmPelsHeight;

   if (Cl.Config->GetWndRes(wi))
   {
	   x = wi.x;
	   y = wi.y;
	   cx = wi.cx;
	   cy = wi.cy;
   }

   hWnds.Main = CreateWindowEx(WS_EX_TOOLWINDOW | WS_EX_NOACTIVATE | WS_EX_LAYERED | WS_EX_TOPMOST, szWindowClass, szTitle, WS_POPUP,
      x, y, cx, cy, hWnds.Parent, NULL, hInstance, NULL);

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

	// explorer.exe가 강제 종료되어 트레이가 사라졌을 경우
	if (message == WM_TASKBARCHANGED)
	{
		Shell_NotifyIcon(NIM_ADD, &niData);
		return 0;
	}

	// 마우스 입력시 정보창 닫기
	if (IsWindow(hWnds.About))
	{
		switch (message)
		{
			case WM_COMMAND:
				if (LOWORD(wParam) != ID_DESTROY_MENU) break;
			case WM_MOVING:
			case WM_SIZING:
			case WM_LBUTTONUP:
			case WM_RBUTTONUP:
				DestroyWindow(hWnds.About);
				hWnds.About = NULL;
				break;
		}
	}

	switch (message)
	{
	case WM_CREATE:
	{
	}
		break;
	case WM_DISPLAYCHANGE:
	{
		Cl.Hotkey->RemoveHook();
		Cl.Hotkey->InstallHook();

		Cl.TextProcess->ResetWatchClip();

		DEVMODE dmREG, dmCUR;
		EnumDisplaySettings(NULL, ENUM_CURRENT_SETTINGS, &dmCUR);
		EnumDisplaySettings(NULL, ENUM_REGISTRY_SETTINGS, &dmREG);

		bool IsMagnet = Cl.Config->GetMagneticMode();
		if (IsMagnet)
		{
			//SendMessage(hWnds.Main, WM_COMMAND, ID_MAGNETIC_MODE, 0);
		}

		_wndinfo wi;
		wi.res_x = dmCUR.dmPelsWidth;
		wi.res_y = dmCUR.dmPelsHeight;

		if (Cl.Config->GetWndRes(wi))
		{
			Cl.Config->SetMagneticMode(true);

			MagnetWnd.IsMagnet = false;
			SetWindowPos(hWnds.Main, 0, wi.x, wi.y, wi.cx, wi.cy, SWP_NOZORDER);

			// 자석 모드 사용 중에는 MagnetWnd의 diff_x, diff_y를 변경시켜야 부모창이 움직이면 초기화되지 않음
			if (Cl.Config->GetMagneticMode())
			{
				MagnetWnd.diff_x = wi.x - MagnetWnd.rect_x;
				MagnetWnd.diff_y = wi.y - MagnetWnd.rect_y;
			}

			MagnetWnd.IsMagnet = true;

			// 1초동안 자석모드를 일시 해제한다
			MagnetWnd.IgnoreTick = GetTickCount() + 1000;
		}
	}
		break;
	case WM_COMMAND:
		wmId = LOWORD(wParam);
		wmEvent = HIWORD(wParam);
		// 메뉴 선택을 구문 분석합니다.
		switch (wmId)
		{
		// 자석모드로 물려놓은 프로세스가 죽으면 부모창이 통째로 날라가기 때문에 살려놔야함
		case ID_RESTORE_PARENT:
		{
			hWnds.Parent = CreateWindowEx(WS_EX_TOOLWINDOW | WS_EX_TOPMOST, szParentClass, L"아네모네", WS_POPUP,
				0, 0, 0, 0, NULL, NULL, hInst, NULL);

			SetWindowLongPtr(hWnds.Main, -8, (LONG)hWnds.Parent);
		}
			break;
		case ID_WINRESET:
		{
			RECT rect;
			GetWindowRect(hWnds.Main, &rect);

			int sm_cx = GetSystemMetrics(SM_CXSCREEN);
			int sm_cy = GetSystemMetrics(SM_CYSCREEN);

			int cx = 500;
			int cy = 200;

			int x = (sm_cx - cx) / 2;
			int y = (sm_cy - cy) / 2;

			SetWindowPos(hWnds.Main, 0, x, y, cx, cy, SWP_NOZORDER);

			GetWindowRect(hWnds.Main, &rect);
			SendMessage(hWnds.Main, WM_COMMAND, ID_SET_WNDRES, (LPARAM)&rect);

		}
			break;
		case ID_ABOUT:
		{
			if (!IsWindow(hWnds.About))
			{
				hWnds.About = CreateDialog(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnds.Main, About);
				ShowWindow(hWnds.About, true); 
			}
			else
			{
				DestroyWindow(hWnds.About);
				hWnds.About = NULL;
			}
		}
			break;
		case ID_EXIT:
		case ID_TERMINATE_ANEMONE:
			CleanUp();
			break;
		case ID_TEMP_CLICK_THOUGH:
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
			PostMessage(hWnds.Main, WM_PAINT, 0, 1); 
			//Cl.TextRenderer->Paint();
			SendMessage(hWnds.Main, WM_COMMAND, ID_SETTING_CHECK, 0);
			break;
		case ID_TEMP_WINDOW_HIDE:
		{
			(Cl.Config->GetTempWinHide() ? Cl.Config->SetTempWinHide(false) : (Cl.Config->GetWindowVisible() ? Cl.Config->SetTempWinHide(true) : Cl.Config->SetTempWinHide(false)));
			if (Cl.Config->GetTempWinHide())
			{
				ShowWindow(hWnd, false);
			}
			else
			{
				ShowWindow(hWnd, true);

				// 창이 뒤에 있으면 가려지므로 위로 띄운다
				if (Cl.Config->GetWindowTopMost())
					SetWindowPos(hWnds.Main, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE);
				else SetWindowPos(hWnds.Main, HWND_TOP, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE);
			}
		}
			break;
		case ID_WINDOW_VISIBLE:
		{
			(Cl.Config->GetWindowVisible() ? Cl.Config->SetWindowVisible(false) : Cl.Config->SetWindowVisible(true));
			if (!Cl.Config->GetWindowVisible())
			{
				ShowWindow(hWnd, false);
			}
			else
			{
				ShowWindow(hWnd, true);

				// 창이 뒤에 있으면 가려지므로 위로 띄운다
				if (Cl.Config->GetWindowTopMost())
					SetWindowPos(hWnds.Main, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE);
				else SetWindowPos(hWnds.Main, HWND_TOP, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE);
			}
		}
			break;
		case ID_HIDEWIN_UNLOCK_HOTKEY:
		{
			(Cl.Config->GetHideWinUnlockHotkey() == 2 ? Cl.Config->SetHideWinUnlockHotkey(0) : (Cl.Config->GetHideWinUnlockHotkey() == 0) ? Cl.Config->SetHideWinUnlockHotkey(1) : Cl.Config->SetHideWinUnlockHotkey(2));
		}
			break;
		case ID_HIDEWIN_UNWATCH_CLIPBOARD:
		{
			(Cl.Config->GetHideWinUnWatchClip() ? Cl.Config->SetHideWinUnWatchClip(false) : Cl.Config->SetHideWinUnWatchClip(true));
		}
			break;
		case ID_MAGNETIC_MODE:
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
		case ID_WND_BORDER_MODE:
			(Cl.Config->GetWndBorderMode() ? Cl.Config->SetWndBorderMode(false) : Cl.Config->SetWndBorderMode(true));
			PostMessage(hWnds.Main, WM_PAINT, 0, 1); 
			//Cl.TextRenderer->Paint();
			SendMessage(hWnds.Main, WM_COMMAND, ID_SETTING_CHECK, 0);
			break;
		case ID_CLIPBOARD_SWITCH:
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
			PostMessage(hWnds.Main, WM_PAINT, 0, 1); 
			//Cl.TextRenderer->Paint();
			SendMessage(hWnds.Main, WM_COMMAND, ID_SETTING_CHECK, 0);
		}
			break;
		case ID_TEXT_PREV:
		{
			std::list<_viewLog>::iterator iter;
			int i, size;
			
			size = viewLog.size()-1;
			i = 0;

			if (size - viewLogNum - 1 < 0) break;
			viewLogNum++;

			for (iter = viewLog.begin(); iter != viewLog.end(); iter++, i++)
			{
				if (i == size - viewLogNum)
				{
					Cl.TextRenderer->SetTextSet(
						(*iter).Name,
						(*iter).NameT,
						(*iter).Text,
						(*iter).TextT);
					break;
				}
			}

			PostMessage(hWnds.Main, WM_PAINT, 0, 1); 
			//Cl.TextRenderer->Paint();
		}
			break;
		case ID_TEXT_NEXT:
		{
			std::list<_viewLog>::iterator iter;
			int i, size;
			
			size = viewLog.size() - 1;
			i = 0;

			if (viewLogNum - 1 < 0) break;
			viewLogNum--;

			for (iter = viewLog.begin(); iter != viewLog.end(); iter++, i++)
			{
				if (i == size - viewLogNum)
				{
					Cl.TextRenderer->SetTextSet(
						(*iter).Name,
						(*iter).NameT,
						(*iter).Text,
						(*iter).TextT);
					break;
				}
			}

			PostMessage(hWnds.Main, WM_PAINT, 0, 1); 
			//Cl.TextRenderer->Paint();
		}
			break;
		case ID_OPENDIC:
		{
			OpenDicText(hWnd);
		}
			break;
		case ID_OPENINI:
		{
			std::wstring path;
			GetLoadPath(path, L"\\Anemone.ini");
			ExecuteFile(path);
		}
			break;
		case ID_TOPMOST:
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
		case ID_WINDOW_SETTING:
		{
			if (IsWindow(hWnds.Setting) == false)
			{
				RECT rect;
				int cx = GetSystemMetrics(SM_CXSCREEN);
				int cy = GetSystemMetrics(SM_CYSCREEN);

				Cl.Config->SaveWndConfig();
				Cl.Config->SaveConfig();
				hWnds.Setting = CreateDialog(hInst, MAKEINTRESOURCE(IDD_SETTING), hWnds.Main, SettingProc);

				GetWindowRect(hWnds.Setting, &rect);

				SetWindowPos(hWnds.Setting, 0, (cx - rect.right + rect.left) / 2, (cy - rect.bottom + rect.top) / 2, 0, 0, SWP_NOSIZE);
				ShowWindow(hWnds.Setting, 1);
			}
			else
			{
				Cl.Config->SaveWndConfig();
				Cl.Config->SaveConfig();

				DestroyWindow(hWnds.Setting);
				hWnds.Setting = NULL;
				break;
			}
		}
		case ID_WINDOW_BACKLOG:
		{
			if (IsWindow(hWnds.BackLog) == false)
			{
				RECT rect;
				GetWindowRect(hWnds.Main, &rect);
				int width = rect.right - rect.left;
				int height = rect.bottom - rect.top;
				int cx = GetSystemMetrics(SM_CXSCREEN);
				int cy = GetSystemMetrics(SM_CYSCREEN);

				hWnds.BackLog = CreateWindowEx(WS_EX_TOOLWINDOW | WS_EX_TOPMOST | WS_EX_NOACTIVATE, szBackLogClass, NULL, WS_BORDER,
					CW_USEDEFAULT, 0, width, height,
					hWnd, (HMENU)NULL, hInst, NULL);

				cx = GetSystemMetrics(SM_CXSCREEN);
				cy = GetSystemMetrics(SM_CYSCREEN);
				GetWindowRect(hWnds.BackLog, &rect);

				SetWindowPos(hWnds.BackLog, 0, ((cx - (rect.right - rect.left)) / 2), ((cy - (rect.bottom - rect.top)) / 2), 0, 0, SWP_NOSIZE | SWP_NOZORDER);

				//hWnds.BackLog = CreateDialog(hInst, MAKEINTRESOURCE(IDD_BACKLOG), hWnds.Main, SettingProc);

				GetWindowRect(hWnds.BackLog, &rect);

				SetWindowPos(hWnds.BackLog, 0, (cx - rect.right + rect.left) / 2, (cy - rect.bottom + rect.top) / 2, 0, 0, SWP_NOSIZE);
				ShowWindow(hWnds.BackLog, 1);
			}
			else
			{
				DestroyWindow(hWnds.BackLog);
				hWnds.BackLog = NULL;
				break;
			}
		}
		break;
		case ID_SETTING_CHECK:
		{
			if (IsWindow(hWnds.Setting))
			{
				// 체크박스
				CheckDlgButton(hWnds.Setting, IDC_SETTING_TOPMOST, Cl.Config->GetWindowTopMost());
				CheckDlgButton(hWnds.Setting, IDC_SETTING_MAGNETIC_MINIMIZE, Cl.Config->GetMagneticMinimize());
				CheckDlgButton(hWnds.Setting, IDC_SETTING_HIDEWIN, !Cl.Config->GetWindowVisible());
				CheckDlgButton(hWnds.Setting, IDC_SETTING_HIDEWIN_UNWATCH_CLIPBOARD, Cl.Config->GetHideWinUnWatchClip());
				CheckDlgButton(hWnds.Setting, IDC_SETTING_CLIPBOARD_WATCH, Cl.Config->GetClipSwitch());
				CheckDlgButton(hWnds.Setting, IDC_SETTING_HOOKER_MONITOR, Cl.Config->GetHookMonitor());
				CheckDlgButton(hWnds.Setting, IDC_SETTING_WNDCLICK_THOUGH, Cl.Config->GetClickThough());
				CheckDlgButton(hWnds.Setting, IDC_SETTING_USE_MAGNETIC, Cl.Config->GetMagneticMode());
				CheckDlgButton(hWnds.Setting, IDC_SETTING_WND_BORDER_MODE, Cl.Config->GetWndBorderMode());
				CheckDlgButton(hWnds.Setting, IDC_SETTING_PRINT_ORGTEXT, Cl.Config->GetTextSwitch(CFG_ORG));
				CheckDlgButton(hWnds.Setting, IDC_SETTING_PRINT_ORGNAME, Cl.Config->GetTextSwitch(CFG_NAME_ORG));
				CheckDlgButton(hWnds.Setting, IDC_SETTING_PREV_SEARCH_NUM, Cl.Config->GetPrevSearchNum());
				CheckDlgButton(hWnds.Setting, IDC_SETTING_SEPERATE_NAME, Cl.Config->GetTextSwitch(CFG_NAME));
				CheckDlgButton(hWnds.Setting, IDC_SETTING_UPDATE_NOTIFY, Cl.Config->GetUpdateNotify());

				{
					(Cl.Config->GetHideWinUnlockHotkey() == 0) ? CheckDlgButton(hWnds.Setting, IDC_SETTING_HIDEWIN_UNLOCK_HOTKEY, false) : 
						CheckDlgButton(hWnds.Setting, IDC_SETTING_HIDEWIN_UNLOCK_HOTKEY, true);

					std::wstringstream ws;
					std::wstring str;

					ws << L"창을 숨기면 단축키 일시 해제";
					if (Cl.Config->GetHideWinUnlockHotkey() == 2)
						ws << L" (전역)";
					else if (Cl.Config->GetHideWinUnlockHotkey() == 1)
						ws << L" (일부)";
					else ws << L"";
					str = ws.str();
					SetDlgItemTextW(hWnds.Setting, IDC_SETTING_HIDEWIN_UNLOCK_HOTKEY, str.c_str());
				}

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
					int nOutlineSize = (Cl.Config->GetWndBorderSize());
					SendDlgItemMessage(hWnds.Setting, IDC_SETTING_WND_BORDER_BAR, TBM_SETRANGE, (WPARAM)TRUE, (LPARAM)MAKELONG(1, 10));
					SendDlgItemMessage(hWnds.Setting, IDC_SETTING_WND_BORDER_BAR, TBM_SETPOS, (WPARAM)TRUE, (LPARAM)nOutlineSize);

					std::wstringstream ws;
					std::wstring str;

					ws << L"테두리 굵기";
					ws << L" (";
					ws << nOutlineSize;
					ws << L")";
					str = ws.str();
					SetDlgItemTextW(hWnds.Setting, IDC_SETTING_WND_BORDER_TEXT, str.c_str());
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

					ws << L"X축";
					ws << L" (";
					ws << n - 10;
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

					ws << L"Y축";
					ws << L" (";
					ws << n - 10;
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

					SendDlgItemMessage(hWnds.Setting, IDC_SETTING_CLIP_TRACKBAR, TBM_SETRANGE, (WPARAM)TRUE, (LPARAM)MAKELONG(10, 100));
					SendDlgItemMessage(hWnds.Setting, IDC_SETTING_CLIP_TRACKBAR, TBM_SETPOS, (WPARAM)TRUE, (LPARAM)n/10);
					
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
		case ID_WINDOW_TRANS:
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
		case ID_WINDOW_FILETRANS:
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
		case ID_TEXTALIGN_LEFT:
			Cl.Config->SetTextAlign(0);
			PostMessage(hWnds.Main, WM_PAINT, 0, 1); 
			//Cl.TextRenderer->Paint();
			break;
		case ID_TEXTALIGN_MID:
			Cl.Config->SetTextAlign(1);
			PostMessage(hWnds.Main, WM_PAINT, 0, 1); 
			//Cl.TextRenderer->Paint();
			break;
		case ID_TEXTALIGN_RIGHT:
			Cl.Config->SetTextAlign(2);
			PostMessage(hWnds.Main, WM_PAINT, 0, 1); 
			//Cl.TextRenderer->Paint();
			break;
		case ID_SEPERATE_NAME:
			(Cl.Config->GetTextSwitch(CFG_NAME) ? Cl.Config->SetTextSwitch(CFG_NAME, false) : Cl.Config->SetTextSwitch(CFG_NAME, true));
			PostMessage(hWnds.Main, WM_PAINT, 0, 1);
			//Cl.TextRenderer->Paint();
			break;
		case ID_EXTERN_HOTKEY:
			(Cl.Config->GetExternHotkey() ? Cl.Config->SetExternHotkey(false) : Cl.Config->SetExternHotkey(true));
			break;
		case ID_PRINT_ORGTEXT:
			(Cl.Config->GetTextSwitch(CFG_ORG) ? Cl.Config->SetTextSwitch(CFG_ORG, false) : Cl.Config->SetTextSwitch(CFG_ORG, true));
			PostMessage(hWnds.Main, WM_PAINT, 0, 1); 
			//Cl.TextRenderer->Paint();
			break;
		case ID_PRINT_ORGNAME:
			(Cl.Config->GetTextSwitch(CFG_NAME_ORG) ? Cl.Config->SetTextSwitch(CFG_NAME_ORG, false) : Cl.Config->SetTextSwitch(CFG_NAME_ORG, true));
			PostMessage(hWnds.Main, WM_PAINT, 0, 1); 
			//Cl.TextRenderer->Paint();
			break;

		case ID_TRANSTEXT_WNDMENU:
		{
			HWND hTargetWnd = GetForegroundWindow();

			// 아네모네 창은 번역하지 않는다
			if (hTargetWnd == GetActiveWindow()) break;

			HMENU hMenu = GetMenu(hTargetWnd);
			TransWndMenu(hMenu);

			SetMenu(hTargetWnd, hMenu);
		}
			break;
		case ID_TRANSTEXT_WNDTEXT:
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
		case ID_DESTROY_MENU:
		{
			HWND hMenuWnd = FindWindowEx(0, 0, L"#32768", L"AnemoneMenu");
			if (hMenuWnd) CloseWindow((HWND)hMenuWnd);
		}
			break;
		case ID_TEXTSIZE_MINUS:
		{
			int nTextSize = Cl.Config->GetTextSize(CFG_TRANS, CFG_A);
			if (nTextSize - 1 < 6) break;
			Cl.Config->SetTextSize(CFG_NAME, CFG_A, nTextSize - 1);
			Cl.Config->SetTextSize(CFG_ORG, CFG_A, nTextSize - 1);
			Cl.Config->SetTextSize(CFG_TRANS, CFG_A, nTextSize - 1);
			PostMessage(hWnds.Main, WM_PAINT, 0, 1); 
			//Cl.TextRenderer->Paint();
			PostMessage(hWnd, WM_COMMAND, ID_SETTING_CHECK, 0);
		}
			break;
		case ID_TEXTSIZE_PLUS:
		{
			int nTextSize = Cl.Config->GetTextSize(CFG_TRANS, CFG_A);
			Cl.Config->SetTextSize(CFG_NAME, CFG_A, nTextSize + 1);
			Cl.Config->SetTextSize(CFG_ORG, CFG_A, nTextSize + 1);
			Cl.Config->SetTextSize(CFG_TRANS, CFG_A, nTextSize + 1);
			PostMessage(hWnds.Main, WM_PAINT, 0, 1); 
			//Cl.TextRenderer->Paint();
			PostMessage(hWnd, WM_COMMAND, ID_SETTING_CHECK, 0);
		}
			break;
		case ID_WNDMOVE_TOP:
		case ID_WNDMOVE_BOTTOM:
		case ID_WNDMOVE_LEFT:
		case ID_WNDMOVE_RIGHT:
		{
			RECT rect;
			int nMovePoint, x, y;
			GetWindowRect(hWnd, &rect);
			nMovePoint = Cl.Config->GetWindowMovePoint();

			x = rect.left;
			y = rect.top;

			switch (wmId)
			{
			case ID_WNDMOVE_TOP:
				y -= nMovePoint;
				break;
			case ID_WNDMOVE_BOTTOM:
				y += nMovePoint;
				break;
			case ID_WNDMOVE_LEFT:
				x -= nMovePoint;
				break;
			case ID_WNDMOVE_RIGHT:
				x += nMovePoint;
				break;
			}

			// 자석 모드 사용 중에는 MagnetWnd의 diff_x, diff_y를 변경시켜야 부모창이 움직이면 초기화되지 않음
			if (Cl.Config->GetMagneticMode())
			{
				MagnetWnd.diff_x = x - MagnetWnd.rect_x;
				MagnetWnd.diff_y = y - MagnetWnd.rect_y;
			}

			// 팝업창이 떠 있으면 닫기
			SendMessage(hWnd, WM_COMMAND, ID_DESTROY_MENU, 0);

			// 윈도우 위치 저장
			SendMessage(hWnds.Main, WM_COMMAND, ID_SET_WNDRES, (LPARAM)&rect);

			SetWindowPos(hWnd, HWND_TOP, x, y, 0, 0, SWP_NOSIZE | SWP_NOACTIVATE | SWP_NOREDRAW | SWP_NOZORDER);
		}
			return 0;
		case ID_WNDSIZE_TOP:
		case ID_WNDSIZE_BOTTOM:
		case ID_WNDSIZE_LEFT:
		case ID_WNDSIZE_RIGHT:
		{
			RECT rect;
			int nMovePoint, cx, cy;
			GetWindowRect(hWnd, &rect);
			nMovePoint = Cl.Config->GetWindowMovePoint();

			cx = rect.right-rect.left;
			cy = rect.bottom-rect.top;

			switch (wmId)
			{
			case ID_WNDSIZE_TOP:
				cy -= nMovePoint;
				break;
			case ID_WNDSIZE_BOTTOM:
				cy += nMovePoint;
				break;
			case ID_WNDSIZE_LEFT:
				cx -= nMovePoint;
				break;
			case ID_WNDSIZE_RIGHT:
				cx += nMovePoint;
				break;
			}

			// 창의 최소 크기보다 줄어들지 못하도록 함
			if (cx < WND_MINTRACKSIZE) cx = WND_MINTRACKSIZE;
			if (cy < WND_MINTRACKSIZE) cy = WND_MINTRACKSIZE;

			// 팝업창이 떠 있으면 닫기
			SendMessage(hWnd, WM_COMMAND, ID_DESTROY_MENU, 0);

			// 윈도우 위치 저장
			SendMessage(hWnds.Main, WM_COMMAND, ID_SET_WNDRES, (LPARAM)&rect);

			SetWindowPos(hWnd, HWND_TOP, 0, 0, cx, cy, SWP_NOMOVE);

			PostMessage(hWnds.Main, WM_PAINT, 0, 1); 
			//Cl.TextRenderer->Paint();
		}
			break;

		// 마우스 좌클릭/우클릭
		case ID_MOUSE_LCLICK:
		{
			POINT point;
			GetCursorPos(&point);
			mouse_event(MOUSEEVENTF_LEFTDOWN, point.x, point.y, 0, 0);
			mouse_event(MOUSEEVENTF_LEFTUP, point.x, point.y, 0, 0);
		}
			break;
		case ID_MOUSE_RCLICK:
		{
			POINT point;
			GetCursorPos(&point);
			mouse_event(MOUSEEVENTF_RIGHTDOWN, point.x, point.y, 0, 0);
			mouse_event(MOUSEEVENTF_RIGHTUP, point.x, point.y, 0, 0);
		}
			break;
		case ID_LOAD_CONFIG:
		{
			Cl.Config->LoadConfig();
			PostMessage(hWnds.Main, WM_PAINT, 0, 1); 
			//Cl.TextRenderer->Paint();
			PostMessage(hWnds.Main, WM_COMMAND, ID_SETTING_CHECK, 0);
		}
			break;
		case ID_LOAD_DICTIONARY:
		{
			Cl.TextProcess->LoadDictionary();
		}
			break;
		case ID_HOOKER_CONFIG:
		{
			if (IsWindow(hWnds.HookCfg) == false)
			{
				RECT rect;
				int cx = GetSystemMetrics(SM_CXSCREEN);
				int cy = GetSystemMetrics(SM_CYSCREEN);

				hWnds.HookCfg = CreateDialog(hInst, MAKEINTRESOURCE(IDD_HOOKCFG), hWnds.Main, HookCfgProc);

				GetWindowRect(hWnds.HookCfg, &rect);

				SetWindowPos(hWnds.HookCfg, 0, (cx - rect.right + rect.left) / 2, (cy - rect.bottom + rect.top) / 2, 0, 0, SWP_NOSIZE);
				ShowWindow(hWnds.HookCfg, 1);

				SetWindowPos(hWnds.HookCfg, HWND_TOP, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE | SWP_NOACTIVATE);
			}
			else
			{
				DestroyWindow(hWnds.HookCfg);
				hWnds.HookCfg = NULL;
				break;
			}
		}
			break;
		case ID_BACKGROUND_SWITCH:
		{
			(Cl.Config->GetBGSwitch() ? Cl.Config->SetBGSwitch(false) : Cl.Config->SetBGSwitch(true));
			PostMessage(hWnds.Main, WM_PAINT, 0, 1); 
			//Cl.TextRenderer->Paint();
		}
			break;
		case ID_HOOKER_MONITOR:
		{
			(Cl.Config->GetHookMonitor() ? Cl.Config->SetHookMonitor(false) : Cl.Config->SetHookMonitor(true));
			PostMessage(hWnds.Main, WM_COMMAND, ID_SETTING_CHECK, 0);
		}
			break;
		case ID_HOOK_DRAWTEXT:
		{
			Cl.TextProcess->OnDrawClipboardByHooker((wchar_t *)lParam);
		}
			return 0;
		case ID_ANEDIC_SITE:
		{
			ShellExecute(NULL, L"open", L"http://www.minori.kr/anemone_dic/", L"", L"", SW_SHOW);
		}
			break;
		case ID_SET_WNDRES:
		{
			RECT *prc = (RECT *)lParam;

			// 현재 윈도우 위치를 변수에 저장
			DEVMODE dmCUR;
			EnumDisplaySettings(NULL, ENUM_CURRENT_SETTINGS, &dmCUR);

			_wndinfo wi;
			wi.res_x = dmCUR.dmPelsWidth;
			wi.res_y = dmCUR.dmPelsHeight;
			wi.x = prc->left;
			wi.y = prc->top;
			wi.cx = prc->right - prc->left;
			wi.cy = prc->bottom - prc->top;

			Cl.Config->SetWndRes(wi);
		}
		break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
	}
	case WM_PAINT:
	{
		if (lParam == 1) Cl.TextRenderer->Paint();
		//MessageBox(0, 0, 0, 0);
		//
	}
		return 0;

	// 트레이 아이콘 이벤트
	case ID_TRAY_EVENT:
	{
		switch (lParam)
		{
		case WM_LBUTTONUP:
		{
			SendMessage(hWnd, WM_COMMAND, ID_DESTROY_MENU, 0);
			
			Cl.Config->SetWindowVisible(true);
			ShowWindow(hWnds.Main, true);
		}
			break;
		case WM_RBUTTONUP:
		{
			POINT pt;
			GetCursorPos(&pt);
			if (!OnContextMenu(hWnds.Main, pt.x, pt.y))
				return DefWindowProc(hWnds.Main, message, wParam, lParam);
		}
			break;
		}
	}
		break;

	// 창을 이동하거나 크기 조정시
	case WM_MOVING:
	case WM_SIZING:
	{
		RECT *prc = (RECT *)lParam;
		SetWindowPos(hWnd, NULL, prc->left, prc->top, prc->right - prc->left, prc->bottom - prc->top, 0);

		// 자석 모드 사용 중에는 MagnetWnd의 diff_x, diff_y를 변경시켜야 부모창이 움직이면 초기화되지 않음
		if (Cl.Config->GetMagneticMode())
		{
			MagnetWnd.diff_x = prc->left - MagnetWnd.rect_x;
			MagnetWnd.diff_y = prc->top - MagnetWnd.rect_y;
		}

		// 윈도우 위치 저장
		SendMessage(hWnds.Main, WM_COMMAND, ID_SET_WNDRES, (LPARAM)prc);

		// 크기 조정중에는 화면이 갱신되도록 한다
		if (message == WM_SIZING) PostMessage(hWnds.Main, WM_PAINT, 0, 1);
	}
		break;

	// 클립보드 데이터가 들어왔을때
	case WM_DRAWCLIPBOARD:
	{
		//Cl.TextRenderer->SetTextSet(L"", L"", L"", L"Clipboard_Input");

		SetDlgItemText(hWnds.HookCfg, IDC_HOOKCFG_STATUS, L"OnDrawClipboard");
		//Cl.TextRenderer->Paint();
		//Sleep(1);
		Cl.TextProcess->OnDrawClipboard();
	}
		break;
	case WM_ERASEBKGND:
		return false;

	// 창의 최소 크기를 설정
	case WM_GETMINMAXINFO:
	{
		MINMAXINFO *mm = (MINMAXINFO *)lParam;

		mm->ptMinTrackSize.x = WND_MINTRACKSIZE;
		mm->ptMinTrackSize.y = WND_MINTRACKSIZE;
	}
		break;
	case WM_LBUTTONDOWN:
		SendMessage(hWnd, WM_NCLBUTTONDOWN, HTCAPTION, 0);
		break;
	case WM_NCLBUTTONDOWN:
		// 아네모네 윈도우를 클릭하면 자석 부모창에 포커스를 띄운다
		if (Cl.Config->GetMagneticMode() && IsWindow(MagnetWnd.hWnd) && GetForegroundWindow() != MagnetWnd.hWnd)
		{
			SetForegroundWindow(MagnetWnd.hWnd);
		}
		return DefWindowProc(hWnd, message, wParam, lParam);
	case WM_NCRBUTTONUP:
		SendMessage(hWnd, WM_CONTEXTMENU, wParam, lParam);
		break;
	case WM_CONTEXTMENU:
	{
		// 아네모네 윈도우를 클릭하면 자석 부모창에 포커스를 띄운다
		if (Cl.Config->GetMagneticMode() && IsWindow(MagnetWnd.hWnd) && GetForegroundWindow() != MagnetWnd.hWnd)
		{
			SetForegroundWindow(MagnetWnd.hWnd);
		}

		POINT pt;
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
	case WM_SHOWWINDOW:
	{
		SendMessage(hWnds.Main, WM_COMMAND, ID_SETTING_CHECK, 0);
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
	case WM_COMMAND:
		wmId = LOWORD(wParam);
		wmEvent = HIWORD(wParam);

		switch (wmId)
		{
		case ID_SETTING_EXIT:
			SendMessage(hWnds.Main, WM_COMMAND, ID_TERMINATE_ANEMONE, 0);
			break;
		case ID_SETTING_OPENDIC:
			SendMessage(hWnds.Main, WM_COMMAND, ID_OPENDIC, 0);
			break;
		case ID_SETTING_OPENINI:
			SendMessage(hWnds.Main, WM_COMMAND, ID_OPENINI, 0);
			break;
		case ID_SETTING_WINRESET:
			SendMessage(hWnds.Main, WM_COMMAND, ID_WINRESET, 0);
			break;
		case ID_SETTING_ADV:
			break;
		case ID_SETTING_HOMEPAGE:
		{
			ShellExecute(NULL, L"open", L"http://www.eroha.net/", L"", L"", SW_SHOW);
		}
			break;
		case ID_SETTING_CLOSE:
			DestroyWindow(hWnd);
			hWnds.Setting = NULL;
			break;
		case IDC_SETTING_TOPMOST:
			SendMessage(hWnds.Main, WM_COMMAND, ID_TOPMOST, 0);
			break;
		case IDC_SETTING_MAGNETIC_MINIMIZE:
			(Cl.Config->GetMagneticMinimize() ? Cl.Config->SetMagneticMinimize(false) : Cl.Config->SetMagneticMinimize(true));
			break;
		case IDC_SETTING_HIDEWIN:
			SendMessage(hWnds.Main, WM_COMMAND, ID_WINDOW_VISIBLE, 0);
			break;
		case IDC_SETTING_HIDEWIN_UNWATCH_CLIPBOARD:
			SendMessage(hWnds.Main, WM_COMMAND, ID_HIDEWIN_UNWATCH_CLIPBOARD, 0);
			break;
		case IDC_SETTING_HIDEWIN_UNLOCK_HOTKEY:
			SendMessage(hWnds.Main, WM_COMMAND, ID_HIDEWIN_UNLOCK_HOTKEY, 0);
			break;
		case IDC_SETTING_CLIPBOARD_WATCH:
			SendMessage(hWnds.Main, WM_COMMAND, ID_CLIPBOARD_SWITCH, 0);
			break;
		case IDC_SETTING_WNDCLICK_THOUGH:
			SendMessage(hWnds.Main, WM_COMMAND, ID_TEMP_CLICK_THOUGH, 0);
			break;
		case IDC_SETTING_USE_MAGNETIC:
			SendMessage(hWnds.Main, WM_COMMAND, ID_MAGNETIC_MODE, 0);
			break;
		case IDC_SETTING_WND_BORDER_MODE:
			SendMessage(hWnds.Main, WM_COMMAND, ID_WND_BORDER_MODE, 0);
			break;
		case IDC_SETTING_PRINT_ORGTEXT:
			SendMessage(hWnds.Main, WM_COMMAND, ID_PRINT_ORGTEXT, 0);
			break;
		case IDC_SETTING_PRINT_ORGNAME:
			SendMessage(hWnds.Main, WM_COMMAND, ID_PRINT_ORGNAME, 0);
			break;
		case IDC_SETTING_SEPERATE_NAME:
			SendMessage(hWnds.Main, WM_COMMAND, ID_SEPERATE_NAME, 0);
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
			(Cl.Config->GetForceAneDic() ? Cl.Config->SetForceAneDic(false) : Cl.Config->SetForceAneDic(true));
			break;
		case IDC_SETTING_ANE_REMOCON:
			(Cl.Config->GetRemoconMode() ? Cl.Config->SetRemoconMode(false) : Cl.Config->SetRemoconMode(true));
			break;
		case IDC_SETTING_EXTERN_HOTKEY:
			SendMessage(hWnds.Main, WM_COMMAND, ID_EXTERN_HOTKEY, 0);
			break;
		case IDC_SETTING_TEXTALIGN_LEFT:
			SendMessage(hWnds.Main, WM_COMMAND, ID_TEXTALIGN_LEFT, 0);
			break;
		case IDC_SETTING_TEXTALIGN_MID:
			SendMessage(hWnds.Main, WM_COMMAND, ID_TEXTALIGN_MID, 0);
			break;
		case IDC_SETTING_TEXTALIGN_RIGHT:
			SendMessage(hWnds.Main, WM_COMMAND, ID_TEXTALIGN_RIGHT, 0);
			break;
		case IDC_SETTING_BACKGROUND_SWITCH:
			SendMessage(hWnds.Main, WM_COMMAND, ID_BACKGROUND_SWITCH, 0);
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
		case IDC_SETTING_WND_BORDER_COLOR:
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
				case IDC_SETTING_WND_BORDER_COLOR:
					dwColor = Cl.Config->GetWndBorderColor();
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
					case IDC_SETTING_WND_BORDER_COLOR:
						Cl.Config->SetWndBorderColor(dw);
						break;
					}
					
					SetDlgItemTextW(hWnd, wmId, L"");
					PostMessage(hWnds.Main, WM_PAINT, 0, 1);
					//Cl.TextRenderer->Paint();
				}
			}
			break;
		case IDC_SETTING_TEXTSIZE_MINUS:
		{
			PostMessage(hWnds.Main, WM_COMMAND, ID_TEXTSIZE_MINUS, 0);
		}
			return 0;
		case IDC_SETTING_TEXTSIZE_PLUS:
		{
			PostMessage(hWnds.Main, WM_COMMAND, ID_TEXTSIZE_PLUS, 0);
		}
			return 0;
		case IDC_SETTING_OUTLINE1_SIZE_MINUS:
		{
			int nTextSize = Cl.Config->GetTextSize(CFG_TRANS, CFG_B);
			if (nTextSize - 1 < 0) break;
			Cl.Config->SetTextSize(CFG_NAME, CFG_B, nTextSize - 1);
			Cl.Config->SetTextSize(CFG_ORG, CFG_B, nTextSize - 1);
			Cl.Config->SetTextSize(CFG_TRANS, CFG_B, nTextSize - 1);
			PostMessage(hWnds.Main, WM_PAINT, 0, 1);
			//Cl.TextRenderer->Paint();
		}
			break;
		case IDC_SETTING_OUTLINE1_SIZE_PLUS:
		{
			int nTextSize = Cl.Config->GetTextSize(CFG_TRANS, CFG_B);
			Cl.Config->SetTextSize(CFG_NAME, CFG_B, nTextSize + 1);
			Cl.Config->SetTextSize(CFG_ORG, CFG_B, nTextSize + 1);
			Cl.Config->SetTextSize(CFG_TRANS, CFG_B, nTextSize + 1);
			PostMessage(hWnds.Main, WM_PAINT, 0, 1); 
			//Cl.TextRenderer->Paint();
		}
		case IDC_SETTING_OUTLINE2_SIZE_MINUS:
		{
			int nTextSize = Cl.Config->GetTextSize(CFG_TRANS, CFG_C);
			if (nTextSize - 1 < 0) break;
			Cl.Config->SetTextSize(CFG_NAME, CFG_C, nTextSize - 1);
			Cl.Config->SetTextSize(CFG_ORG, CFG_C, nTextSize - 1);
			Cl.Config->SetTextSize(CFG_TRANS, CFG_C, nTextSize - 1);
			PostMessage(hWnds.Main, WM_PAINT, 0, 1); 
			//Cl.TextRenderer->Paint();
		}
			break;
		case IDC_SETTING_OUTLINE2_SIZE_PLUS:
		{
			int nTextSize = Cl.Config->GetTextSize(CFG_TRANS, CFG_C);
			Cl.Config->SetTextSize(CFG_NAME, CFG_C, nTextSize + 1);
			Cl.Config->SetTextSize(CFG_ORG, CFG_C, nTextSize + 1);
			Cl.Config->SetTextSize(CFG_TRANS, CFG_C, nTextSize + 1);
			PostMessage(hWnds.Main, WM_PAINT, 0, 1);
			//Cl.TextRenderer->Paint();
		}
			break;
		case IDC_SETTING_NAME_SHADOW:
			(Cl.Config->GetTextShadow(CFG_NAME) ? Cl.Config->SetTextShadow(CFG_NAME, false) : Cl.Config->SetTextShadow(CFG_NAME, true));
			PostMessage(hWnds.Main, WM_PAINT, 0, 1);
			//Cl.TextRenderer->Paint();
			break;
		case IDC_SETTING_ORG_SHADOW:
			(Cl.Config->GetTextShadow(CFG_ORG) ? Cl.Config->SetTextShadow(CFG_ORG, false) : Cl.Config->SetTextShadow(CFG_ORG, true));
			PostMessage(hWnds.Main, WM_PAINT, 0, 1); 
			//Cl.TextRenderer->Paint();
			break;
		case IDC_SETTING_TRANS_SHADOW:
			(Cl.Config->GetTextShadow(CFG_TRANS) ? Cl.Config->SetTextShadow(CFG_TRANS, false) : Cl.Config->SetTextShadow(CFG_TRANS, true));
			PostMessage(hWnds.Main, WM_PAINT, 0, 1); 
			//Cl.TextRenderer->Paint();
			break;
		case IDC_SETTING_NAME_FONT:
		case IDC_SETTING_ORG_FONT:
		case IDC_SETTING_TRANS_FONT:
		{
			CHOOSEFONT cf;
			LOGFONT lf;

			int cfg_type;
			if (wmId == IDC_SETTING_NAME_FONT) cfg_type = CFG_NAME;
			else if (wmId == IDC_SETTING_ORG_FONT) cfg_type = CFG_ORG;
			else if (wmId == IDC_SETTING_TRANS_FONT) cfg_type = CFG_TRANS;

			ZeroMemory(&lf, sizeof(lf));
			wcscpy_s(lf.lfFaceName, Cl.Config->GetTextFont(cfg_type));

			// 400 - NM / 700 - BOLD
			int fontStyle = Cl.Config->GetFontStyle(cfg_type);
			if (fontStyle > 3) fontStyle = 0;

			(fontStyle / 2 == 1 ? lf.lfItalic = true : lf.lfItalic = false);
			(fontStyle % 2 == 1 ? lf.lfWeight = 700 : lf.lfWeight = 400);

			PAINTSTRUCT ps;
			HDC hDC = BeginPaint(hWnd, &ps);
			lf.lfHeight = -MulDiv(22, GetDeviceCaps(hDC, LOGPIXELSY), 72);
			EndPaint(hWnd, &ps);

			if (FontDialog(hWnd, cf, lf))
			{
				Cl.Config->SetTextFont(cfg_type, lf.lfFaceName);
				fontStyle = 0;

				if (lf.lfWeight == 700) fontStyle += 1;
				if (lf.lfItalic) fontStyle += 2;

				Cl.Config->SetFontStyle(cfg_type, fontStyle);
			}
			PostMessage(hWnds.Main, WM_PAINT, 0, 1); 
			//Cl.TextRenderer->Paint();
		}
			break;
		case IDC_SETTING_UPDATE_NOTIFY:
			(Cl.Config->GetUpdateNotify() ? Cl.Config->SetUpdateNotify(false) : Cl.Config->SetUpdateNotify(true));
			break;
		case IDC_SETTING_UPDATE_CHECK:
			UpdateNotify(hWnd, true);
			break;
		case IDC_SETTING_PREV_SEARCH_NUM:
			(Cl.Config->GetPrevSearchNum() ? Cl.Config->SetPrevSearchNum(false) : Cl.Config->SetPrevSearchNum(true));
			PostMessage(hWnds.Main, WM_PAINT, 0, 1); 
			//Cl.TextRenderer->Paint();
			PostMessage(hWnds.Main, WM_COMMAND, ID_SETTING_CHECK, 0);
			break;
		case IDC_SETTING_HOOKER_MONITOR:
			PostMessage(hWnds.Main, WM_COMMAND, ID_HOOKER_MONITOR, 0);
			break;
		case IDC_SETTING_HOOKER_CONFIG:
		{
			if (IsWindow(hWnds.HookCfg) == false)
			{
				RECT rect;
				int cx = GetSystemMetrics(SM_CXSCREEN);
				int cy = GetSystemMetrics(SM_CYSCREEN);

				hWnds.HookCfg = CreateDialog(hInst, MAKEINTRESOURCE(IDD_HOOKCFG), hWnds.Setting, HookCfgProc);

				GetWindowRect(hWnds.HookCfg, &rect);

				SetWindowPos(hWnds.HookCfg, 0, (cx - rect.right + rect.left) / 2, (cy - rect.bottom + rect.top) / 2, 0, 0, SWP_NOSIZE);
				ShowWindow(hWnds.HookCfg, 1);
			}
			else
			{
				DestroyWindow(hWnds.HookCfg);
				hWnds.HookCfg = NULL;
				break;
			}
		}
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
		SendMessage(hWnds.Main, WM_COMMAND, ID_SETTING_CHECK, 0);
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
		case IDC_SETTING_WND_BORDER_COLOR:
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
			case IDC_SETTING_WND_BORDER_COLOR:
				ColorVar = Cl.Config->GetWndBorderColor();
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

			PostMessage(hWnds.Main, WM_COMMAND, ID_SETTING_CHECK, 0);
			PostMessage(hWnds.Main, WM_PAINT, 0, 1); 
			//Cl.TextRenderer->Paint();
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

			PostMessage(hWnds.Main, WM_COMMAND, ID_SETTING_CHECK, 0);
			PostMessage(hWnds.Main, WM_PAINT, 0, 1); 
			//Cl.TextRenderer->Paint();
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

			PostMessage(hWnds.Main, WM_COMMAND, ID_SETTING_CHECK, 0);
			PostMessage(hWnds.Main, WM_PAINT, 0, 1); 
			//Cl.TextRenderer->Paint();
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

			PostMessage(hWnds.Main, WM_COMMAND, ID_SETTING_CHECK, 0);
			PostMessage(hWnds.Main, WM_PAINT, 0, 1); 
			//Cl.TextRenderer->Paint();
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

			PostMessage(hWnds.Main, WM_COMMAND, ID_SETTING_CHECK, 0);
			PostMessage(hWnds.Main, WM_PAINT, 0, 1);
			//Cl.TextRenderer->Paint();
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

			PostMessage(hWnds.Main, WM_COMMAND, ID_SETTING_CHECK, 0);
			PostMessage(hWnds.Main, WM_PAINT, 0, 1); 
			//Cl.TextRenderer->Paint();
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

			PostMessage(hWnds.Main, WM_COMMAND, ID_SETTING_CHECK, 0);
			PostMessage(hWnds.Main, WM_PAINT, 0, 1); 
			//Cl.TextRenderer->Paint();
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

			PostMessage(hWnds.Main, WM_COMMAND, ID_SETTING_CHECK, 0);
			PostMessage(hWnds.Main, WM_PAINT, 0, 1); 
			//Cl.TextRenderer->Paint();
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

			PostMessage(hWnds.Main, WM_COMMAND, ID_SETTING_CHECK, 0);
			PostMessage(hWnds.Main, WM_PAINT, 0, 1); 
			//Cl.TextRenderer->Paint();
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

			Cl.Config->SetClipLength(i*10);

			PostMessage(hWnds.Main, WM_COMMAND, ID_SETTING_CHECK, 0);
			PostMessage(hWnds.Main, WM_PAINT, 0, 1); 
			//Cl.TextRenderer->Paint();
		}
		else if ((HWND)lParam == GetDlgItem(hWnd, IDC_SETTING_WND_BORDER_BAR))
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
				i = (INT)SendDlgItemMessage(hWnd, IDC_SETTING_WND_BORDER_BAR, TBM_GETPOS, 0, 0);
				break;

			case TB_THUMBTRACK:
				i = (INT)HIWORD(wParam);
				break;
			}

			if (i > 10) break;

			Cl.Config->SetWndBorderSize(i);

			PostMessage(hWnds.Main, WM_COMMAND, ID_SETTING_CHECK, 0);
			PostMessage(hWnds.Main, WM_PAINT, 0, 1); 
			//Cl.TextRenderer->Paint();
		}
	}
	}
	return 0;
}
CHOOSECOLOR *pCC;
UINT CALLBACK CCHookProc(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
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

UINT CALLBACK CFHookProc(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg) {
	case WM_INITDIALOG:
	{
		int nExStyleValue = GetWindowLong(hDlg, GWL_EXSTYLE);
		nExStyleValue |= WS_EX_NOACTIVATE;
		SetWindowLong(hDlg, GWL_EXSTYLE, nExStyleValue);
	}
		return TRUE;
	case WM_MOVING:
	case WM_SIZING:
	{
		RECT *prc = (RECT *)lParam;
		SetWindowPos(hDlg, NULL, prc->left, prc->top, prc->right - prc->left, prc->bottom - prc->top, 0);
	}
		break;
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
	{
		RECT rect;
		int cx = GetSystemMetrics(SM_CXSCREEN);
		int cy = GetSystemMetrics(SM_CYSCREEN);

		GetWindowRect(hDlg, &rect);

		SetWindowPos(hDlg, 0, (cx - (rect.right - rect.left)) / 2, (cy - (rect.bottom - rect.top)) / 2, 0, 0, SWP_NOSIZE);
	}
		return (INT_PTR)TRUE;

	case WM_LBUTTONDOWN:
	case WM_RBUTTONDOWN:
		EndDialog(hDlg, LOWORD(wParam));
		return (INT_PTR)TRUE;
		break;

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
		case ID_TRANS_START:
		{
			SetDlgItemText(hWnd, IDC_TRANSWIN_TRANSLATE, L"번역중지");
		}
			break;
		case ID_TRANS_PROGRESS:
		{
			SetWindowText(GetDlgItem(hWnd, IDC_TRANSWIN_DEST), (LPCWSTR)lParam);
		}
			break;
		case ID_TRANS_COMPLETE:
		case ID_TRANS_ERROR:
		case ID_TRANS_ABORT:
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
		SendMessage(hWnds.Main, WM_COMMAND, ID_SETTING_CHECK, 0);
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
			OPENFILENAME ofn;
			wchar_t szFile[260];

			ZeroMemory(&ofn, sizeof(ofn));
			ofn.lStructSize = sizeof(ofn);
			ofn.hwndOwner = hWnd;
			ofn.lpstrFile = szFile;

			ofn.lpstrFile[0] = L'\0';
			ofn.nMaxFile = sizeof(szFile);
			ofn.lpstrFilter = L"텍스트 파일\0*.TXT\0모든 파일\0*.*\0";
			ofn.nFilterIndex = 1;
			ofn.lpstrFileTitle = NULL;
			ofn.nMaxFileTitle = 0;
			ofn.lpstrInitialDir = NULL;
			ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

			GetDlgItemText(hWnd, IDC_FILE_TRANSWIN_LOAD, szFile, 255);

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
				
				// 미리보기
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
			OPENFILENAME ofn;
			wchar_t szFile[260];

			ZeroMemory(&ofn, sizeof(ofn));
			ofn.lStructSize = sizeof(ofn);
			ofn.hwndOwner = hWnd;
			ofn.lpstrFile = szFile;

			ofn.lpstrFile[0] = L'\0';
			ofn.nMaxFile = sizeof(szFile);
			ofn.lpstrFilter = L"텍스트 파일\0*.TXT\0모든 파일\0*.*\0";
			ofn.nFilterIndex = 1;
			ofn.lpstrFileTitle = NULL;
			ofn.nMaxFileTitle = 0;
			ofn.lpstrInitialDir = NULL;
			ofn.Flags = OFN_PATHMUSTEXIST | OFN_OVERWRITEPROMPT;

			GetDlgItemText(hWnd, IDC_FILE_TRANSWIN_SAVE, szFile, 255);

			if (GetSaveFileName(&ofn) == TRUE)
			{
				std::wstring saveFile = ofn.lpstrFile;
				if (saveFile.find(L"\\") != std::wstring::npos)
				{
					if (saveFile.find(L".", saveFile.find(L"\\") + 1) == std::wstring::npos)
					{
						saveFile += L".txt";
					}
				}
				
				SetDlgItemText(hWnd, IDC_FILE_TRANSWIN_SAVE, saveFile.c_str());
			}
		}
			break;
		case IDC_FILE_TRANSWIN_TRANSLATE:
		{
			HANDLE   hThread;
			DWORD    dwThreadID;

			FILETRANS *FT = new FILETRANS;
			GetDlgItemText(hWnd, IDC_FILE_TRANSWIN_LOAD, FT->lpszInputFileName, 255);
			GetDlgItemText(hWnd, IDC_FILE_TRANSWIN_SAVE, FT->lpszOutputFileName, 255);
			FT->StartTickCount = GetTickCount();
			FT->WriteType = Cl.Config->GetFileTransOutput();

			if (FT->lpszInputFileName[0] == NULL || FT->lpszOutputFileName[0] == NULL)
			{
				MessageBox(hWnd, L"파일 위치를 선택해주세요", L"파일 경로 미지정", MB_ICONASTERISK);
				delete FT;
				return 0;
			}

			hThread = CreateThread(NULL, 0, FileTransThread, FT, 0, &dwThreadID);
			CloseHandle(hThread);

			/*
			
			
			Cl.TextProcess->TranslateAbort();

			SetFocus(GetDlgItem(hWnd, IDC_TRANSWIN_DEST));
			SetWindowText(GetDlgItem(hWnd, IDC_TRANSWIN_DEST), L"번역중...");
			int length = SendMessage(GetDlgItem(hWnd, IDC_TRANSWIN_SRC), WM_GETTEXTLENGTH, 0, 0) + 1;
			wchar_t *pStr = (wchar_t *)HeapAlloc(AneHeap, 0, sizeof(wchar_t) * (length + 1));

			GetDlgItemText(hWnd, IDC_TRANSWIN_SRC, pStr, length);

			std::wstring original_context = pStr;
			HeapFree(AneHeap, 0, pStr);

			Cl.TextProcess->TranslateText(hWnd, original_context);

			*/

			//SetDlgItemText(hWnd, IDC_FILE_TRANSWIN_PREVIEW, L"번역 누름");
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
		SendMessage(hWnds.Main, WM_COMMAND, ID_SETTING_CHECK, 0);
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

INT_PTR CALLBACK FileTransWinProgProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	int wmId, wmEvent;

	switch (message)
	{
	case WM_COMMAND:
		wmId = LOWORD(wParam);
		wmEvent = HIWORD(wParam);
		// 메뉴 선택을 구문 분석합니다.
		switch (wmId)
		{
		case ID_FILE_TRANSPROG_START:
		{
			wchar_t str[10];
			_itow(lParam, str, 16);
			SetWindowText(GetDlgItem(hWnd, IDC_FILE_TRANSPROG_STATUSADDR), str);
		}
			break;
		case ID_FILE_TRANSPROG_LISTSIZE:
		{
			wchar_t str[10];
			_itow(lParam, str, 10);
			SetWindowText(GetDlgItem(hWnd, IDC_FILE_TRANSPROG_LISTSIZE), str);
			SendMessage(GetDlgItem(hWnd, IDC_FILE_TRANSPROG_BAR), PBM_SETRANGE, 0, MAKELPARAM(0, lParam));
			SendMessage(GetDlgItem(hWnd, IDC_FILE_TRANSPROG_BAR), PBM_SETSTEP, (WPARAM)1, 0);
		}
			break;
		case ID_FILE_TRANSPROG_PROGRESS:
		{
			wchar_t str[10];
			int i = lParam;
			GetWindowText(GetDlgItem(hWnd, IDC_FILE_TRANSPROG_LISTSIZE), str, 8);

			int size = _wtoi(str);
			std::wstringstream logstream;
			logstream << i;
			logstream << L"/";
			logstream << size;

			PostMessage(GetDlgItem(hWnd, IDC_FILE_TRANSPROG_BAR), PBM_STEPIT, 0, 0);
			SetWindowText(GetDlgItem(hWnd, IDC_FILE_TRANSPROG_TEXT), (LPCWSTR)logstream.str().c_str());
		}
			break;
		case ID_FILE_TRANSPROG_COMPLETE:
		{
			SetWindowText(GetDlgItem(hWnd, IDC_FILE_TRANSPROG_TEXT), (LPCWSTR)lParam);
		}
			break;
		case ID_FILE_TRANSPROG_NAME:
		{
			SetWindowText(GetDlgItem(hWnd, IDC_FILE_TRANSPROG_NAME), (LPCWSTR)lParam);
		}
			break;
		case IDC_FILE_TRANSPROG_CANCEL:
		{
			wchar_t addrstr[10];
			GetWindowText(GetDlgItem(hWnd, IDC_FILE_TRANSPROG_STATUSADDR), addrstr, 9);
			
			int *nStatus = (int *)wcstoul(addrstr, NULL, 16);

			(*nStatus) = 2;
		}
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
		SendMessage(hWnds.Main, WM_COMMAND, ID_SETTING_CHECK, 0);
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

INT_PTR CALLBACK HookCfgProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	int wmId, wmEvent;

	switch (message)
	{
	case WM_SHOWWINDOW:
	{
		HWND hITH = FindWindow(L"ITH", 0);
		if (hITH)
			SetDlgItemText(hWnd, IDC_HOOKCFG_STATUS, L"ITH가 켜져 있습니다.");
		else SetDlgItemText(hWnd, IDC_HOOKCFG_STATUS, L"ITH가 꺼져 있습니다.");

		int n = Cl.Config->GetHookInterval();

		SendDlgItemMessage(hWnds.HookCfg, IDC_HOOKCFG_INTERVAL_BAR, TBM_SETRANGE, (WPARAM)TRUE, (LPARAM)MAKELONG(1, 200));
		SendDlgItemMessage(hWnds.HookCfg, IDC_HOOKCFG_INTERVAL_BAR, TBM_SETPOS, (WPARAM)TRUE, (LPARAM)n/5);

		std::wstringstream ws;

		ws << n;
		SetDlgItemTextW(hWnds.HookCfg, IDC_HOOKCFG_INTERVAL_EDIT, ws.str().c_str());

		/*
		HWND hAGTH = FindWindow(L"AGTHClass", 0);
		if (hAGTH)
			SetDlgItemText(hWnd, IDC_HOOKCFG_AGTH, L"AGTH ON");
		else SetDlgItemText(hWnd, IDC_HOOKCFG_AGTH, L"AGTH OFF");
		*/
	}
		break;
	case WM_HSCROLL:
	{
		if ((HWND)lParam == GetDlgItem(hWnd, IDC_HOOKCFG_INTERVAL_BAR))
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
				i = (INT)SendDlgItemMessage(hWnd, IDC_HOOKCFG_INTERVAL_BAR, TBM_GETPOS, 0, 0);
				break;

			case TB_THUMBTRACK:
				i = (INT)HIWORD(wParam);
				break;
			}

			if (i > 200) break;

			std::wstringstream ws;
			i *= 5;
			ws << i;

			SetDlgItemText(hWnd, IDC_HOOKCFG_INTERVAL_EDIT, ws.str().c_str());

			Cl.Config->SetHookInterval(i);

			PostMessage(hWnds.Main, WM_COMMAND, ID_SETTING_CHECK, 0);
		}
	}
		break;
	case WM_COMMAND:
	{
		HWND hITH = FindWindow(L"ITH", 0);
		HWND hAGTH = FindWindow(L"AGTHClass", 0);
		wmId = LOWORD(wParam);
		wmEvent = HIWORD(wParam);
		HWND hwnd = 0;
		// 메뉴 선택을 구문 분석합니다.
		switch (wmId)
		{
		case IDC_HOOKCFG_SETBTN:
		{
			//HWND hID = FindWindowEx(hAGTH, 0, L"Edit", 0);
		
			//GetWindowText(hID, buf, 10000);
			//SetDlgItemText(hWnds.HookCfg, IDC_HOOKCFG_EDIT2, L"...");
			//SetDlgItemText(hWnds.HookCfg, IDC_HOOKCFG_EDIT2, buf);
			//break;
			int nCombo = 0;

			while ((hwnd = FindWindowEx(hITH, hwnd, 0, 0)) != 0)
			{
				// COMBOBOX
				if (GetWindowLong(hwnd, GWL_STYLE) == 0x50010303)
				{
					if (nCombo == 0)
					{

						nCombo++;
					}
					else
					{
						SendMessage(GetDlgItem(hWnds.HookCfg, IDC_HOOKCFG_LIST1), LB_RESETCONTENT, 0, 0);

						int cb_current = SendMessage(hwnd, CB_GETCURSEL, 0, 0);
						int cb_count = SendMessage(hwnd, CB_GETCOUNT, 0, 0);
						for (int i = 0; i < cb_count; i++)
						{
							SendMessage(hwnd, CB_SETCURSEL, i, 0);
							int cch = SendMessage(hwnd, WM_GETTEXTLENGTH, 0, 0);
							wchar_t *buf = (wchar_t *)malloc((cch + 1) * 2);
							buf[0] = 0x00;
							if (SendMessage(hwnd, CB_GETLBTEXT, cch + 1, (WPARAM)buf) == CB_ERR)
								SendMessage(hwnd, WM_GETTEXT, cch + 1, (WPARAM)buf);

							std::wstringstream wss;
							wss << L"길이 : ";
							wss << cch;
							wss << L"\r\n내용 : ";
							wss << buf;
							SetDlgItemText(hWnd, IDC_HOOKCFG_EDIT2, wss.str().c_str());

							// Listbox에 등록
							int max = SendMessage(GetDlgItem(hWnds.HookCfg, IDC_HOOKCFG_LIST1), LB_GETCOUNT, 0, 0);
							//SendMessage(GetDlgItem(hWnds.HookCfg, IDC_HOOKCFG_LIST1), LB_SETSEL, 0, max);
							SendMessage(GetDlgItem(hWnds.HookCfg, IDC_HOOKCFG_LIST1), LB_ADDSTRING, 0, (LPARAM)buf);
							free(buf);
						}
					}
				}

				// EDIT
				if (GetWindowLong(hwnd, GWL_STYLE) == 0x50200144)
				{
					SetDlgItemText(hWnd, IDC_HOOKCFG_STATUS, L"윈도우를 찾음");
					
					// EDIT 내용을 읽음
					int cch = SendMessage(hwnd, WM_GETTEXTLENGTH, 0, 0);
					wchar_t *buf = (wchar_t *)malloc((cch + 1) * 2);
					SendMessage(hwnd, WM_GETTEXT, (WPARAM)(cch + 1), (LPARAM)buf);
					

					std::wstringstream wss;

					wss << L"길이 : ";
					wss << cch;
					wss << L"\r\n";
					wss << L"내용 : ";
					wss << buf;
					
					//GetWindowText(hwnd, buf, 10000);
					//MessageBox(0, buf, 0, 0);
					SetDlgItemText(hWnd, IDC_HOOKCFG_EDIT1, wss.str().c_str());

					free(buf);
				}
			}
		}
				break;

		case IDC_HOOKCFG_REFRESH:
		{
			hwnd = FindWindowEx(hITH, 0, L"Button", L"Option");
			SendMessage(hwnd, WM_LBUTTONDOWN, 0, 0);
			SendMessage(hwnd, WM_LBUTTONUP, 0, 0);
			//SendMessage(hwnd, WM_KEYDOWN, VK_SPACE, 0);
			//SendMessage(hwnd, WM_KEYUP, VK_SPACE, 0);
			SetForegroundWindow(FindWindow(L"#32770", L"Option"));
		}
			break;
		case IDOK:
			DestroyWindow(hWnd);
			break;
		case IDCANCEL:
			DestroyWindow(hWnd);
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
	}
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

INT_PTR CALLBACK BackLogProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	int wmId, wmEvent;

	switch (message)
	{
	case WM_SHOWWINDOW:
	{

	}
		break;
	case WM_COMMAND:
	{
		wmId = LOWORD(wParam);
		wmEvent = HIWORD(wParam);
		HWND hwnd = 0;
		// 메뉴 선택을 구문 분석합니다.
		switch (wmId)
		{
		case IDOK:
			DestroyWindow(hWnd);
			break;
		case IDCANCEL:
			DestroyWindow(hWnd);
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
	}
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

bool __stdcall UpdateNotify(HWND hWnd, bool IsCurMsg)
{
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

		if (IsCurMsg == true) MessageBox(hWnd, L"아네모네 버전 확인 실패", L"업데이트 확인", MB_ICONASTERISK);
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
	else
	{
		if (IsCurMsg == true) MessageBox(hWnd, L"아네모네 버전 확인 실패", L"업데이트 확인", MB_ICONASTERISK);
		return false;
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
	else if (IsCurMsg == true && ver == 0)
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

DWORD WINAPI FileTransThread(LPVOID lpParam)
{
	FILE *fpr, *fpw;
	FILETRANS* FT;
	FT = (FILETRANS*)lpParam;
	wchar_t wstr[1024];

	HWND hDlg = CreateDialog(hInst, MAKEINTRESOURCE(IDD_FILE_TRANSWIN_PROG), hWnds.FileTrans, FileTransWinProgProc);
	SetWindowLongPtr(hDlg, -8, (LONG)0);
	ShowWindow(hDlg, true);
	int nLines;

	if (_wfopen_s(&fpr, FT->lpszInputFileName, L"rt,ccs=UTF-8") != 0)
	{
		std::wstringstream wss;
		wss << L"원문 파일이 존재하지 않거나 읽기 권한이 없습니다 :\r\n\r\n";
		wss << FT->lpszOutputFileName;
		MessageBox(hDlg, wss.str().c_str(), L"아네모네", 0);
		DestroyWindow(hDlg);
		delete FT;
		return false;
	}

	if (_wfopen_s(&fpw, FT->lpszOutputFileName, L"wt,ccs=UTF-8") != 0)
	{
		std::wstringstream wss;
		wss << L"저장될 파일이 사용중이거나 쓰기 권한이 없습니다 :\r\n\r\n";
		wss << FT->lpszOutputFileName;
		MessageBox(hDlg, wss.str().c_str(), L"아네모네", 0);
		DestroyWindow(hDlg);
		fclose(fpr);
		delete FT;
		return false;
	}

	std::wstring input;
	for (nLines = 0; fgetws(wstr, 1000, fpr) != NULL; nLines++) {
		input.append(wstr);
		//input.append(L"\r\n");
	}

	fclose(fpr);
	
	std::wstringstream content;
	std::wstring proclog;

	Elapsed_Prepare = 0;
	Elapsed_Translate = 0;

	std::wstring::size_type nprev = 0;
	std::wstring::size_type npos = -1;
	std::list<std::wstring> list_org, list_trans, list;
	std::wstring output;
	int i = 0, length = input.length();
	std::wstring empty = L"Abort";

	int nStatus = 1;
	
	std::list<std::wstring>::iterator iter, iter_trans;

	SendMessage(hDlg, WM_COMMAND, ID_FILE_TRANSPROG_START, (LPARAM)&nStatus);

	std::wstring filename = FT->lpszInputFileName;
	filename = filename.substr(filename.rfind(L"\\")+1);

	SendMessage(hDlg, WM_COMMAND, ID_FILE_TRANSPROG_NAME, (LPARAM)filename.c_str());

	for (;; i++)
	{
		nprev = npos + 1;
		npos = input.find(L"\n", nprev);
		if (npos != std::string::npos)
		{
			list_org.push_back(input.substr(nprev, npos - nprev + 1));
		}
		else
		{
			list_org.push_back(input.substr(nprev));
			break;
		}
	}

	unsigned int div = input.length() / 1024 + 1;

	if (list_org.size() > div)
	{
		std::wstring line;
		std::list<std::wstring> list2 = list_org;
		int j = 1;
		for (i = 1, iter = list2.begin(); iter != list2.end(); iter++, i++)
		{
			if ((float)i > (float)(list2.size() * j / div))
			{
				list.push_back(line);
				line = L"";
				j++;
			}

			line += (*iter);

			if (i == list2.size())
			{
				list.push_back(line);
				line = L"";
			}

		}
	}
	else
	{
		list = list_org;
	}

	SendMessage(hDlg, WM_COMMAND, ID_FILE_TRANSPROG_LISTSIZE, (LPARAM)list.size());

	for (i = 0, iter = list.begin(); iter != list.end(); iter++, i++)
	{
		if (nStatus == 2)
		{
			nStatus = 0;
			MessageBox(hDlg, L"텍스트 번역을 중단했습니다.", L"아네모네", MB_ICONINFORMATION);
			DestroyWindow(hDlg);
			fclose(fpw);
			delete FT;
			return -1;
		}
			output += Cl.TextProcess->eztrans_proc(*iter);

		SendMessage(hDlg, WM_COMMAND, ID_FILE_TRANSPROG_PROGRESS, (LPARAM)i+1);
	}
	/*
	std::wstringstream logstream;
	logstream << list.size();
	logstream << L"/";
	logstream << list.size();

	proclog = logstream.str();
	PostMessage(hDlg, WM_COMMAND, ID_FILE_TRANSPROG_COMPLETE, (LPARAM)proclog.c_str());
	*/
	nStatus = 0;

	for (;; i++)
	{
		nprev = npos + 1;
		npos = output.find(L"\n", nprev);
		if (npos != std::string::npos)
		{
			list_trans.push_back(output.substr(nprev, npos - nprev + 1));
		}
		else
		{
			list_trans.push_back(output.substr(nprev));
			break;
		}
	}

	for (iter = list_org.begin(), iter_trans = list_trans.begin(); iter != list_org.end(); iter++, iter_trans++)
	{
		if (FT->WriteType == 1)
		{
			// 마지막 라인이면 원문 뒤에 \r\n 추가
			if (std::next(iter, 1) == list_org.end())
			{
				fwrite((*iter).c_str(), sizeof(wchar_t), wcslen((*iter).c_str()), fpw);
				fwrite(L"\r\n", sizeof(wchar_t), wcslen(L"\r\n"), fpw);
				fwrite((*iter_trans).c_str(), sizeof(wchar_t), wcslen((*iter_trans).c_str()), fpw);
				break;
			}

			fwrite((*iter).c_str(), sizeof(wchar_t), wcslen((*iter).c_str()), fpw);
			fwrite((*iter_trans).c_str(), sizeof(wchar_t), wcslen((*iter_trans).c_str()), fpw);
			fwrite(L"\r\n", sizeof(wchar_t), wcslen(L"\r\n"), fpw);
		}
		else fwrite((*iter_trans).c_str(), sizeof(wchar_t), wcslen((*iter_trans).c_str()), fpw);
	}
	/*
	if (FT->WriteType == 1)
	{
		output += (*iter);
		output += L"\r\n\r\n";
	}
	*/
	//fwrite(output.c_str(), sizeof(wchar_t), wcslen(output.c_str()), fp);
	fclose(fpw);
	MessageBox(hDlg, L"번역을 완료했습니다", L"아네모네", MB_ICONINFORMATION);

	/*
	return output;







	for (int i = 0; fgetws(wstr, 1000, fp) != NULL; i++)
	{
		std::wstringstream textStr;
		textStr << i;
		textStr << L"/";
		textStr << nLines;
		SetDlgItemText(hDlg, IDC_FILE_TRANSPROG_TEXT, textStr.str().c_str());


		if (i > 0) content.append(L"\r\n");
		content.append(wstr);

		if (i == 6) break;
	}
	*/
	delete FT;
	return 0; // 성공
}