// stdafx.cpp : 표준 포함 파일만 들어 있는 소스 파일입니다.
// Anemone.pch는 미리 컴파일된 헤더가 됩니다.
// stdafx.obj에는 미리 컴파일된 형식 정보가 포함됩니다.

#include "stdafx.h"

// TODO: 필요한 추가 헤더는
// 이 파일이 아닌 STDAFX.H에서 참조합니다.

/*
IsWritableMemory : 주어진 메모리 주소의 상태가 쓰기가능인지 체크하는 함수
Param :
LPVOID  pMemoryAddr : 검사하고자 하는 메모리의 주소
Return Value :
ERROR_SUCCESS : 인자로 주어진 주소의 메모리가 Read/Write 가능한 경우
기타 값 : Read/Write 가능한 메모리가 아니면 해당 주소의 Protect Mode 를
나타내는 0이 아닌 값을 리턴함.
http://msdn.microsoft.com/en-us/library/aa915370.aspx 참조
*/
INT IsWritableMemory(LPVOID pMemoryAddr)
{
	MEMORY_BASIC_INFORMATION    MemInfo = { 0, };
	SIZE_T  nResult = 0;

	nResult = VirtualQuery(pMemoryAddr, &MemInfo, sizeof(MemInfo));

	if (nResult == 0) // 커널 영역인 경우 VirtualQuery 자체가 Fail함.  
	{
		return -1;
	}
	else if (MemInfo.Protect & (PAGE_EXECUTE_READWRITE | PAGE_READWRITE))
	{
		return  ERROR_SUCCESS;
	}
	else
	{
		return  MemInfo.Protect;
	}
}


/*
IsReadableMemory : 주어진 메모리 주소의 상태가 참조(Read)가능인지 체크하는 함수
Param :
LPVOID  pMemoryAddr : 검사하고자 하는 메모리의 주소
Return Value :
ERROR_SUCCESS : 인자로 주어진 주소의 메모리가 Read 가능한 경우
기타 값 : Read 가능한 메모리가 아니면 해당 주소의 State 를
나타내는 0이 아닌 값을 리턴함.
http://msdn.microsoft.com/en-us/library/aa915370.aspx 참조
*/
INT IsReadableMemory(LPVOID pMemoryAddr)
{
	MEMORY_BASIC_INFORMATION    MemInfo = { 0, };
	SIZE_T  nResult = 0;

	nResult = VirtualQuery(pMemoryAddr, &MemInfo, sizeof(MemInfo));

	if (nResult == 0) // 커널 영역인 경우 VirtualQuery 자체가 Fail함.  
	{
		return -1;
	}
	else if (MemInfo.State & MEM_COMMIT)
	{
		return  ERROR_SUCCESS;
	}
	else
	{
		return  MemInfo.State;
	}
}