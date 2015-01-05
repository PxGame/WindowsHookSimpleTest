#include "WinMain.h"

CWinMain app;

BOOL CWinMain::InitInstance()
{
	m_pMainWnd = new CMainFrame();
	m_pMainWnd->ShowWindow(m_nCmdShow);
	m_pMainWnd->UpdateWindow();
	return TRUE;
}