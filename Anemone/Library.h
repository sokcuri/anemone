#pragma once

void GetLoadPath(std::wstring &dir);
bool GetEZTPathFromREG(std::wstring &dir);
bool ReadINI_Str(const wchar_t *key, const wchar_t *section, wchar_t *buf, wchar_t *file);
bool WriteINI_Str(const wchar_t *key, const wchar_t *section, wchar_t *buf, wchar_t *file);