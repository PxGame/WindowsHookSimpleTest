#include "MainFrame.h"

BEGIN_MESSAGE_MAP(CMainFrame, CFrameWnd)
	ON_BN_CLICKED(IDB_BUTTON, btnClicked)
END_MESSAGE_MAP()


typedef void (*REPLACEIDTENTRYINONEMOD)(
	PCSTR pszCalleeModName,
	PROC pfnCurrent,
	PROC pfnNew,
	HMODULE hmodCaller);

typedef int(WINAPI *MESSAGEBOXA)(//必须加上WINAPI，应为函数原型有，如果不加入，则会在运行时报错。
  HWND hWnd,
  LPCSTR lpText,
  LPCTSTR lpCaption,
  UINT uType
);
MESSAGEBOXA messageboxa = NULL;

int WINAPI MyMessageBoxA(
  HWND hWnd,
  LPCSTR lpText,
  LPCTSTR lpCaption,
  UINT uType
)
{
	//return messageboxa(NULL, "已修改", "Test", MB_OK);
	return MessageBoxW(NULL, L"已修改", L"Test", MB_OK);
}

void FixFuntion()
{
	HMODULE hModule = LoadLibrary("APIHookDll.dll");
	if (hModule == NULL)
	{
		MessageBoxA(NULL, "APIHookDll.dll is load error !" , "Error", MB_OK);
		return ;
	}
	REPLACEIDTENTRYINONEMOD ReplaceIDTEntryInOneMod = (REPLACEIDTENTRYINONEMOD)
		GetProcAddress(hModule, "ReplaceIDTEntryInOneMod");
	if (ReplaceIDTEntryInOneMod == NULL)
	{
		MessageBoxA(NULL, "ReplaceIDTEntryInOneMod is get error !" , "Error", MB_OK);
		return ;
	}

	PCSTR pszCalleeModName = "USER32.dll";
	PROC pfnCurrent = (PROC)GetProcAddress(GetModuleHandle("USER32.dll"), "MessageBoxA");
	::messageboxa = (MESSAGEBOXA)pfnCurrent;
	PROC pfnNew = (PROC)MyMessageBoxA;
	HMODULE hmodCaller = GetModuleHandle(NULL);
	if (hmodCaller == NULL)
	{
		MessageBoxA(NULL, "hmodCaller is get error !" , "Error", MB_OK);
		return ;
	}
	ReplaceIDTEntryInOneMod(pszCalleeModName, (PROC)pfnCurrent, pfnNew, hmodCaller);

	FreeLibrary(hModule);
}

CMainFrame::CMainFrame(void)
{
	this->Create(NULL, _T("Test"), WS_POPUPWINDOW | WS_CAPTION, CRect(0,0,300,300));
	btn.Create(_T("Clicked"), WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON, CRect(0,0,200,200), this, IDB_BUTTON);
	this->CenterWindow();
	btn.CenterWindow();

	FixFuntion();
}


CMainFrame::~CMainFrame(void)
{
}

void CMainFrame::btnClicked()
{
	MessageBoxA("haha", "haha");
}