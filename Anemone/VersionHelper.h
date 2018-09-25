#pragma once
#include <Windows.h>

#define _DEF_WIN32_WINNT_NT4                    0x0400
#define _DEF_WIN32_WINNT_WIN2K                  0x0500
#define _DEF_WIN32_WINNT_WINXP                  0x0501
#define _DEF_WIN32_WINNT_WS03                   0x0502
#define _DEF_WIN32_WINNT_WIN6                   0x0600
#define _DEF_WIN32_WINNT_VISTA                  0x0600
#define _DEF_WIN32_WINNT_WS08                   0x0600
#define _DEF_WIN32_WINNT_LONGHORN               0x0600
#define _DEF_WIN32_WINNT_WIN7                   0x0601
#define _DEF_WIN32_WINNT_WIN8                   0x0602
#define _DEF_WIN32_WINNT_WINBLUE                0x0603

BOOL DEF_IsWindowsVersionOrGreater(WORD wMajorVersion, WORD wMinorVersion, WORD wServicePackMajor)
{
	OSVERSIONINFOEXW osvi = { sizeof(osvi), 0, 0, 0, 0,{ 0 }, 0, 0 };
	DWORDLONG        const dwlConditionMask = VerSetConditionMask(
		VerSetConditionMask(
			VerSetConditionMask(
				0, VER_MAJORVERSION, VER_GREATER_EQUAL),
			VER_MINORVERSION, VER_GREATER_EQUAL),
		VER_SERVICEPACKMAJOR, VER_GREATER_EQUAL);

	osvi.dwMajorVersion = wMajorVersion;
	osvi.dwMinorVersion = wMinorVersion;
	osvi.wServicePackMajor = wServicePackMajor;

	return VerifyVersionInfoW(&osvi, VER_MAJORVERSION | VER_MINORVERSION | VER_SERVICEPACKMAJOR, dwlConditionMask) != FALSE;
}

BOOL DEF_IsWindowsXPOrGreater()
{
	return DEF_IsWindowsVersionOrGreater(HIBYTE(_DEF_WIN32_WINNT_WINXP), LOBYTE(_DEF_WIN32_WINNT_WINXP), 0);
}

BOOL DEF_IsWindowsXPSP1OrGreater()
{
	return DEF_IsWindowsVersionOrGreater(HIBYTE(_DEF_WIN32_WINNT_WINXP), LOBYTE(_DEF_WIN32_WINNT_WINXP), 1);
}

BOOL DEF_IsWindowsXPSP2OrGreater()
{
	return DEF_IsWindowsVersionOrGreater(HIBYTE(_DEF_WIN32_WINNT_WINXP), LOBYTE(_DEF_WIN32_WINNT_WINXP), 2);
}

BOOL DEF_IsWindowsXPSP3OrGreater()
{
	return DEF_IsWindowsVersionOrGreater(HIBYTE(_DEF_WIN32_WINNT_WINXP), LOBYTE(_DEF_WIN32_WINNT_WINXP), 3);
}

BOOL DEF_IsWindowsVistaOrGreater()
{
	return DEF_IsWindowsVersionOrGreater(HIBYTE(_DEF_WIN32_WINNT_VISTA), LOBYTE(_DEF_WIN32_WINNT_VISTA), 0);
}

BOOL DEF_IsWindowsVistaSP1OrGreater()
{
	return DEF_IsWindowsVersionOrGreater(HIBYTE(_DEF_WIN32_WINNT_VISTA), LOBYTE(_DEF_WIN32_WINNT_VISTA), 1);
}

BOOL DEF_IsWindowsVistaSP2OrGreater()
{
	return DEF_IsWindowsVersionOrGreater(HIBYTE(_DEF_WIN32_WINNT_VISTA), LOBYTE(_DEF_WIN32_WINNT_VISTA), 2);
}

BOOL DEF_IsWindows7OrGreater()
{
	return DEF_IsWindowsVersionOrGreater(HIBYTE(_DEF_WIN32_WINNT_WIN7), LOBYTE(_DEF_WIN32_WINNT_WIN7), 0);
}

BOOL DEF_IsWindows7SP1OrGreater()
{
	return DEF_IsWindowsVersionOrGreater(HIBYTE(_DEF_WIN32_WINNT_WIN7), LOBYTE(_DEF_WIN32_WINNT_WIN7), 1);
}

BOOL DEF_IsWindows8OrGreater()
{
	return DEF_IsWindowsVersionOrGreater(HIBYTE(_DEF_WIN32_WINNT_WIN8), LOBYTE(_DEF_WIN32_WINNT_WIN8), 0);
}

BOOL DEF_IsWindows8Point1OrGreater()
{
	return DEF_IsWindowsVersionOrGreater(HIBYTE(_DEF_WIN32_WINNT_WINBLUE), LOBYTE(_DEF_WIN32_WINNT_WINBLUE), 0);
}
