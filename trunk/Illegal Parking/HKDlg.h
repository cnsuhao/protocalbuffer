// HKDlg.h : main header file for the HKDLG application
//

#if !defined(AFX_HKDLG_H__4885C3FA_ED8A_4180_B0F7_B8AD6A44287D__INCLUDED_)
#define AFX_HKDLG_H__4885C3FA_ED8A_4180_B0F7_B8AD6A44287D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols
#include "GeneralDef.h"
#include "HKDlgDlg.h"

extern CHKDlgDlg *g_pMainDlg;

/////////////////////////////////////////////////////////////////////////////
// CHKDlgApp:
// See HKDlg.cpp for the implementation of this class
//

class CHKDlgApp : public CWinApp
{
public:
	CHKDlgApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CHKDlgApp)
	public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CHKDlgApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_HKDLG_H__4885C3FA_ED8A_4180_B0F7_B8AD6A44287D__INCLUDED_)
