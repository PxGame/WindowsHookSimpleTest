#pragma once
#include "afxwin.h"
# define IDB_BUTTON 1000
class CMainFrame :
	public CFrameWnd
{
public:
	CMainFrame(void);
	~CMainFrame(void);

protected:
	CButton btn;
	afx_msg void btnClicked();

	DECLARE_MESSAGE_MAP()
};

