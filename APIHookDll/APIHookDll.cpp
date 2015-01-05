// APIHookDll.cpp : 定义 DLL 应用程序的导出函数。
//

#include "stdafx.h"

// Handle unexpected exceptions if the module is unloaded   
LONG WINAPI InvalidReadExceptionFilter(PEXCEPTION_POINTERS pep) {   
   
   // handle all unexpected exceptions because we simply don't patch   
   // any module in that case   
   LONG lDisposition = EXCEPTION_EXECUTE_HANDLER;   
   
   // Note: pep->ExceptionRecord->ExceptionCode has 0xc0000005 as a value   
   
   return(lDisposition);   
}

extern "C" __declspec(dllexport) void ReplaceIDTEntryInOneMod(
	PCSTR pszCalleeModName,
	PROC pfnCurrent,
	PROC pfnNew,
	HMODULE hmodCaller)
{
	ULONG ulSize;
	PIMAGE_IMPORT_DESCRIPTOR pImportDesc = NULL;
	__try
	{
		pImportDesc = (PIMAGE_IMPORT_DESCRIPTOR)
			ImageDirectoryEntryToData(hmodCaller, TRUE, IMAGE_DIRECTORY_ENTRY_IMPORT,&ulSize); 
	}
	__except (InvalidReadExceptionFilter(GetExceptionInformation()))
	{
	}
	if (pImportDesc == NULL)
	{
		return;
	}
	for (; pImportDesc->Name; pImportDesc++)
	{
		PSTR pszModName = (PSTR)((PBYTE)hmodCaller + pImportDesc->Name);
		if (lstrcmpiA(pszModName, pszCalleeModName) == 0)
		{
			PIMAGE_THUNK_DATA pThunk = (PIMAGE_THUNK_DATA)
				((PBYTE)hmodCaller + pImportDesc->FirstThunk);
			for (; pThunk->u1.Function; pThunk++)
			{
				PROC *ppfn = (PROC *)&pThunk->u1.Function;
				BOOL bFound = (*ppfn == pfnCurrent);
				if (bFound)
				{
					if (! WriteProcessMemory(GetCurrentProcess(), ppfn, &pfnNew, sizeof(pfnNew), NULL) &&//修改函数地址
						(ERROR_NOACCESS == GetLastError()))
					{
						DWORD dwOldProtext;
						if (VirtualProtect(ppfn, sizeof(pfnNew), PAGE_WRITECOPY, &dwOldProtext))//修改页面保护属性
						{
							WriteProcessMemory(GetCurrentProcess(), ppfn, &pfnNew, sizeof(pfnNew), NULL);
							VirtualProtect(ppfn, sizeof(pfnNew), dwOldProtext, &dwOldProtext);//还原页面保护属性
						}
					}
					return; //修改完成
				}
			}
		}
	}
}
