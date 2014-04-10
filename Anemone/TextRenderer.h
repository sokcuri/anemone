#pragma once
class CTextRenderer
{
public:
	CTextRenderer();
	~CTextRenderer();
	bool Init();
	bool Paint();
	std::wstring *szNameOrg;
	std::wstring *szNameTrans;
	std::wstring *szContextOrg;
	std::wstring *szContextTrans;
private:
	ULONG_PTR m_gpToken;
	//HDC hMemDC;

};

