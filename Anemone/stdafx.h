// stdafx.h : 자주 사용하지만 자주 변경되지는 않는
// 표준 시스템 포함 파일 및 프로젝트 관련 포함 파일이
// 들어 있는 포함 파일입니다.
//

#pragma once
#define OEMRESOURCE
#include "targetver.h"

#define CFG_NAME 0
#define CFG_ORG 1
#define CFG_TRANS 2
#define CFG_NAME_ORG 3
#define CFG_BACKGROUND 4
#define CFG_BORDER 5

#define CFG_A 0
#define CFG_B 1
#define CFG_C 2
#define CFG_S 3
#define CFG_ALL 4

#pragma warning(disable: 4244) // wstring-string 경고
#pragma warning(disable: 4996) // wcscpy

// Windows 헤더 파일:
#include <windows.h>
#include <process.h>

// C 런타임 헤더 파일입니다.
#include <ios>
#include <tchar.h>
#include <vector>
#include <regex>

#include "Library.h"

#include "TransEngine.h"
#include "TextRenderer.h"
#include "TextProcess.h"
#include "Hotkey.h"
#include "Config.h"
#include "FileWatch.h"
#include "Remocon.h"

#include "Resource.h"

// GDIPlus
#include <GdiPlus.h>
#pragma comment(lib,"GdiPlus.lib")
using namespace Gdiplus;

// WinInet
#include <WinInet.h>
#pragma comment(lib, "WinInet.lib")

// STL
#include <string>
#include <sstream>
#include <hash_map>

// control
#include <commctrl.h>
#pragma comment(lib, "Comctl32.lib")
#pragma comment(linker,"/manifestdependency:\" type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0'  processorArchitecture='x86' publicKeyToken='6595b64144ccf1df' language='*'\"") 

// MinTrackSize
#define WND_MINTRACKSIZE 90

extern HINSTANCE hInst;

// hWnd 모음
struct _hWnds
{
	HWND About;
	HWND Main;
	HWND Parent;
	HWND Setting;
	HWND Remocon;
	HWND View;
	HWND Clip;
	HWND Trans;
	HWND FileTrans;
	HWND HookCfg;
	HWND BackLog;
};
extern _hWnds hWnds;
extern HANDLE AneHeap;
extern bool IsActive;
extern bool bClipIgnore;

// 자석 모드
struct _MagnetWnd
{
	HANDLE hThread;
	bool IsMagnet = false;
	bool IsMinimize = false;
	int IsFirst = 1;
	HWND hWnd;
	int res_x;
	int res_y;
	int res_c;
	int diff_x;
	int diff_y;
	int rect_x;
	int rect_y;
	bool IsTopMost;
	DWORD IgnoreTick;
};
extern _MagnetWnd MagnetWnd;

// 클래스 모음
struct _Class
{
	CTextRenderer *TextRenderer = 0;
	CTransEngine *TransEngine = 0;
	CTextProcess *TextProcess = 0;
	CHotkey *Hotkey = 0;
	CConfig *Config = 0;
	CFileWatch *FileWatch = 0;
	CRemocon *Remocon = 0;
};
extern _Class Cl;

struct render_attr
{
	FontFamily *Font;
	int Size_A;
	int Size_B;
	int Size_C;
	int Size_S;
};

struct _key_map
{
	bool Ctrl = false;
	bool Alt = false;
	bool Shift = false;
	DWORD Code;
	DWORD func;
	bool Always = false;
};

extern std::vector<_key_map> key_map;

static render_attr *rdName;
static render_attr *rdOriginal;
static render_attr *rdTrans;

INT IsWritableMemory(LPVOID pMemoryAddr);
INT IsReadableMemory(LPVOID pMemoryAddr);

typedef struct {
	DWORD pid;
	HWND hwnd;
} WINDOWPROCESSINFO;

UINT CALLBACK CCHookProc(HWND, UINT, WPARAM, LPARAM);
UINT CALLBACK CFHookProc(HWND, UINT, WPARAM, LPARAM);

extern int Elapsed_Prepare;
extern int Elapsed_Translate;

// WININET
typedef struct
{
	HINTERNET hInternet;
	HINTERNET hURL;
	HINTERNET hRequest;
	char *strHeaders;
	char *szPostData;
} PARM;

typedef struct
{
	wchar_t lpszInputFileName[260];
	wchar_t lpszOutputFileName[260];
	int WriteType;
	bool NoTransLineFeed;
	int StartTickCount;
} FILETRANS;
/*
struct dicStruct
{
	wchar_t *lpwszJpn;
	wchar_t *lpwszKor;
	char *lpszJpn;
	char *lpszKor;
	char *lpszPart;
	char *lpszAttr;
	bool bHidden;
};
extern std::vector<dicStruct> DICT;
*/
struct aneDicStruct
{
	int line;
	int type;
	wchar_t wjpn[31];
	wchar_t wkor[31];
	bool uc_vaild;
	char hidden;
	char jpn[31];
	char kor[31];
	char part[5];
	char attr[42];
	int operator<(aneDicStruct rhs) {
		if (wcslen(wjpn) > wcslen(rhs.wjpn) || (wcslen(wjpn) == wcslen(rhs.wjpn) && (wcscmp(wjpn, rhs.wjpn) > 0)) || ((wcslen(wjpn) == wcslen(rhs.wjpn) && (wcscmp(wjpn, rhs.wjpn) == 0)) && line < rhs.line))
			return true;
		//return ((wcslen(wjpn) > wcslen(rhs.wjpn)) || (wcslen(wjpn) <= wcslen(rhs.wjpn)) && (wcscmp(wjpn, rhs.wjpn) > 0)); }
		return false;
	}
};
extern std::vector<aneDicStruct> AneDic;

typedef struct
{
	int filesize;
	void *m_loc;
} OLDFILEINFO;

extern NOTIFYICONDATA niData;
extern UINT WM_TASKBARCHANGED;
extern int nMenuType;

class _viewLog
{
public:
	_viewLog(const _viewLog &vL)
	{
		__viewLog(vL.Name, vL.NameT, vL.Text, vL.TextT);
	}
	_viewLog(const wchar_t *name, const wchar_t *namet, const wchar_t *text, const wchar_t *textt)
	{
		__viewLog(name, namet, text, textt);
	}
	void __viewLog(const wchar_t *name, const wchar_t *namet, const wchar_t *text, const wchar_t *textt)
	{
		int nLen;

		nLen = wcslen(name);
		Name = (wchar_t *)malloc((nLen + 1) * 2);
		if (nLen > 1000) nLen = 1000;
		memcpy(Name, name, nLen * 2);
		Name[nLen] = 0x00;

		nLen = wcslen(namet);
		NameT = (wchar_t *)malloc((nLen + 1) * 2);
		if (nLen > 1000) nLen = 1000;
		memcpy(NameT, namet, nLen * 2);
		NameT[nLen] = 0x00;

		nLen = wcslen(text);
		Text = (wchar_t *)malloc((nLen + 1) * 2);
		if (nLen > 1000) nLen = 1000;
		memcpy(Text, text, nLen * 2);
		Text[nLen] = 0x00;

		nLen = wcslen(textt);
		TextT = (wchar_t *)malloc((nLen + 1) * 2);
		if (nLen > 1000) nLen = 1000;
		memcpy(TextT, textt, nLen * 2);
		TextT[nLen] = 0x00;
	}
	~_viewLog()
	{
		free(Name);
		free(NameT);
		free(Text);
		free(TextT);
	}
	/*
	wchar_t Name[1024];
	wchar_t NameT[1024];
	wchar_t Text[1024];
	wchar_t TextT[1024];*/
	wchar_t *Name;
	wchar_t *NameT;
	wchar_t *Text;
	wchar_t *TextT;
};

extern std::list<_viewLog> viewLog;
extern int viewLogNum;

struct _wndinfo
{
	int res_x;
	int res_y;
	int x;
	int y;
	int cx;
	int cy;
};
extern std::vector<_wndinfo> WndInfo;

extern CRITICAL_SECTION cs_trans;
extern CRITICAL_SECTION cs_ezdic;

extern int CCColorType;
extern int CCColorN;