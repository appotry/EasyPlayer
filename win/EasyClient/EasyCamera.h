
// EasyCamera.h : main header file for the PROJECT_NAME application
//

#pragma once

#ifndef __AFXWIN_H__
	#error "include 'stdafx.h' before including this file for PCH"
#endif

#include "resource.h"		// main symbols


// CEasyCameraApp:
// See EasyCamera.cpp for the implementation of this class
//

class CEasyCameraApp : public CWinApp
{
public:
	CEasyCameraApp();

// Overrides
public:
	virtual BOOL InitInstance();

// Implementation

	DECLARE_MESSAGE_MAP()
	virtual BOOL PreTranslateMessage(MSG* pMsg);
};

extern CEasyCameraApp theApp;