#include "stdafx.h"
#include "TransEngine.h"
static CRITICAL_SECTION CriticalSection;
int CTransEngine::ezt_addr[20];

CTransEngine::CTransEngine()
{
	//InitializeCriticalSection(&CriticalSection);
}

void CTransEngine::GetEnginePath(std::wstring szEnginePath)
{
	szEnginePath = EnginePath;
}

bool CTransEngine::Init(std::wstring &szEnginePath)
{
	// Load ezTrans Engine
	std::wstring szEngineDLL = szEnginePath + L"\\J2KEngine.dll";
	HMODULE hDLL = LoadLibrary(szEngineDLL.c_str());
	if (!hDLL) MessageBox(0, L"이지트랜스 번역 엔진 초기화 실패\r\n: LoadLibrary Failed", 0, MB_ICONERROR);

	wcscpy_s(EnginePath, szEngineDLL.c_str());

	// Load ezTrans Function
	ezt_addr[0] = (int)GetProcAddress(hDLL, "J2K_FreeMem");
	ezt_addr[1] = (int)GetProcAddress(hDLL, "J2K_GetPriorDict");
	ezt_addr[2] = (int)GetProcAddress(hDLL, "J2K_GetProperty");
	ezt_addr[3] = (int)GetProcAddress(hDLL, "J2K_Initialize");
	ezt_addr[4] = (int)GetProcAddress(hDLL, "J2K_InitializeEx");
	ezt_addr[5] = (int)GetProcAddress(hDLL, "J2K_ReloadUserDict");
	ezt_addr[6] = (int)GetProcAddress(hDLL, "J2K_SetDelJPN");
	ezt_addr[7] = (int)GetProcAddress(hDLL, "J2K_SetField");
	ezt_addr[8] = (int)GetProcAddress(hDLL, "J2K_SetHnj2han");
	ezt_addr[9] = (int)GetProcAddress(hDLL, "J2K_SetJWin");
	ezt_addr[10] = (int)GetProcAddress(hDLL, "J2K_SetPriorDict");
	ezt_addr[11] = (int)GetProcAddress(hDLL, "J2K_SetProperty");
	ezt_addr[12] = (int)GetProcAddress(hDLL, "J2K_StopTranslation");
	ezt_addr[13] = (int)GetProcAddress(hDLL, "J2K_Terminate");
	ezt_addr[14] = (int)GetProcAddress(hDLL, "J2K_TranslateChat");
	ezt_addr[15] = (int)GetProcAddress(hDLL, "J2K_TranslateFM");
	ezt_addr[16] = (int)GetProcAddress(hDLL, "J2K_TranslateMM");
	ezt_addr[17] = (int)GetProcAddress(hDLL, "J2K_TranslateMMEx");
	ezt_addr[18] = (int)GetProcAddress(hDLL, "J2K_TranslateMMNT");
	ezt_addr[19] = (int)GetProcAddress(hDLL, "J2K_TranslateMMNTW");

	for (int i = 0; i <= 18; i++)
	if (!ezt_addr[i]) 
	{
		MessageBox(0, L"이지트랜스 번역 엔진 초기화 실패\r\n: 함수 정보 불러오기 실패", 0, MB_ICONERROR);
		return false;
	}

	if (ezt_addr[19]) ehndSupport = true;

	Sleep(50);

	std::wstring _szEngineDAT = szEnginePath + L"\\Dat";
	std::string szEngineDAT(_szEngineDAT.length(), ' ');
	std::copy(_szEngineDAT.begin(), _szEngineDAT.end(), szEngineDAT.begin());
	if (J2K_InitializeEx("CSUSER123455", szEngineDAT.c_str())) return true;
	return false;
}

__declspec(naked) void CTransEngine::J2K_FreeMem(void *addr)
{
	__asm JMP ezt_addr + (4 * 0)
}
__declspec(naked) void CTransEngine::J2K_GetPriorDict(void)
{
	__asm JMP ezt_addr + (4 * 1)
}
__declspec(naked) void CTransEngine::J2K_GetProperty(void)
{
	__asm JMP ezt_addr + (4 * 2)
}
__declspec(naked) void CTransEngine::J2K_Initialize(void)
{
	__asm JMP ezt_addr + (4 * 3)
}
__declspec(naked) bool CTransEngine::J2K_InitializeEx(char *data0, const char *key)
{
	__asm JMP ezt_addr + (4 * 4)
}
__declspec(naked) void CTransEngine::J2K_ReloadUserDict()
{
	__asm JMP ezt_addr + (4 * 5)
}
__declspec(naked) void CTransEngine::J2K_SetDelJPN()
{
	__asm JMP ezt_addr + (4 * 6)
}
__declspec(naked) void CTransEngine::J2K_SetField()
{
	__asm JMP ezt_addr + (4 * 7)
}
__declspec(naked) void CTransEngine::J2K_SetHnj2han()
{
	__asm JMP ezt_addr + (4 * 8)
}
__declspec(naked) void CTransEngine::J2K_SetJWin()
{
	__asm JMP ezt_addr + (4 * 9)
}
__declspec(naked) void CTransEngine::J2K_SetPriorDict()
{
	__asm JMP ezt_addr + (4 * 10)
}
__declspec(naked) void CTransEngine::J2K_SetProperty()
{
	__asm JMP ezt_addr + (4 * 11)
}
__declspec(naked) void CTransEngine::J2K_StopTranslation()
{
	__asm JMP ezt_addr + (4 * 12)
}
__declspec(naked) void CTransEngine::J2K_Terminate()
{
	__asm JMP ezt_addr + (4 * 13)
}
__declspec(naked) void CTransEngine::J2K_TranslateChat()
{
	__asm JMP ezt_addr + (4 * 14)
}
__declspec(naked) void CTransEngine::J2K_TranslateFM()
{
	__asm JMP ezt_addr + (4 * 15)
}
__declspec(naked) void CTransEngine::J2K_TranslateMM()
{
	__asm JMP ezt_addr + (4 * 16)
}
__declspec(naked) void CTransEngine::J2K_TranslateMMEx()
{
	__asm JMP ezt_addr + (4 * 17)
}
__declspec(naked) int CTransEngine::J2K_TranslateMMNT(int data0, char *krStr)
{
	__asm JMP ezt_addr + (4 * 18)
}

__declspec(naked) int CTransEngine::J2K_TranslateMMNTW(int data0, wchar_t *krStr)
{
	__asm JMP ezt_addr + (4 * 19)
}

CTransEngine::~CTransEngine()
{
	//DeleteCriticalSection(&CriticalSection);
	Cl.TransEngine = 0;
}
