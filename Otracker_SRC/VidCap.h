// VidCap.h : main header file for the PROJECT_NAME application
//

#pragma once

#ifndef __AFXWIN_H__
#error "include 'stdafx.h' before including this file for PCH"
#endif

#include "resource.h"		// main symbols


// CVidCapApp:
// See VidCap.cpp for the implementation of this class
//

class CVidCapApp : public CWinApp
{
public:
        CVidCapApp();

		void checkReleaseMutex();		
		BOOL checkDuplicateExecute();
		BOOL CVidCapApp::IsDirectoryExist(const CString& dirPath);

// Overrides
public:
        virtual BOOL InitInstance();

// Implementation

        DECLARE_MESSAGE_MAP()
};

extern CVidCapApp theApp;