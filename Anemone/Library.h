#pragma once

void GetLoadPath(std::wstring &dir);
bool GetEZTPathFromREG(std::wstring &dir);
bool ReadINI_Str(const wchar_t *key, const wchar_t *section, wchar_t *buf, wchar_t *file);
bool WriteINI_Str(const wchar_t *key, const wchar_t *section, wchar_t *buf, wchar_t *file);
int UCS2toUTF8(char *dst, wchar_t *ucs2, int count);
int UTF8toUCS2(wchar_t *ucs2, char *src);
std::wstring replaceAll(const std::wstring &str, const std::wstring &pattern, const std::wstring &replace);