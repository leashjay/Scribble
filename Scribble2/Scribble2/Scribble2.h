
// Scribble2.h : main header file for the Scribble2 application
//
#pragma once

#ifndef __AFXWIN_H__
	#error "include 'pch.h' before including this file for PCH"
#endif

#include "resource.h"       // main symbols


// CScribble2App:
// See Scribble2.cpp for the implementation of this class
//

class CScribble2App : public CWinApp
{
public:
	CScribble2App() noexcept;


// Overrides
public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();

// Implementation
	afx_msg void OnAppAbout();
	DECLARE_MESSAGE_MAP()
};

extern CScribble2App theApp;
