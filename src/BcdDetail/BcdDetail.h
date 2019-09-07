
// BcdDetail.h : main header file for the PROJECT_NAME application
//

#pragma once

#ifndef __AFXWIN_H__
	#error "include 'stdafx.h' before including this file for PCH"
#endif

#include "resource.h"		// main symbols

#include "BcdStore.h"
// CBcdDetailApp:
// See BcdDetail.cpp for the implementation of this class
//

class CBcdDetailApp : public CWinApp
{
public:
	CBcdDetailApp();

// Overrides
public:
	virtual BOOL InitInstance();

// Implementation

	DECLARE_MESSAGE_MAP()
};

extern CBcdDetailApp theApp;
