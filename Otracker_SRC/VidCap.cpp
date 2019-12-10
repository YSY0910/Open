// VidCap.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "VidCap.h"
#include "VidCapDlg.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CVidCapApp
HANDLE m_hMutex; //���α׷� �ߺ����� ������ ����.

CD_ss_status *gDss_status = NULL;
CD_s_sensors *gDs_sensors = NULL;
CD_vsettings *gDvsettings = NULL;
//CString g_strRunningPath = _T("");				// ���α׷��� ���۵Ǵ� ���.
CString g_strImagePath = _T("");			//ħ���̹��� ������.

BEGIN_MESSAGE_MAP(CVidCapApp, CWinApp)
        ON_COMMAND(ID_HELP, &CWinApp::OnHelp)
END_MESSAGE_MAP()


// CVidCapApp construction

CVidCapApp::CVidCapApp()
{
        // TODO: add construction code here,
        // Place all significant initialization in InitInstance
}


// The one and only CVidCapApp object

CVidCapApp theApp;


// CVidCapApp initialization

BOOL CVidCapApp::InitInstance()
{
        // InitCommonControlsEx() is required on Windows XP if an application
        // manifest specifies use of ComCtl32.dll version 6 or later to enable
        // visual styles.  Otherwise, any window creation will fail.
        INITCOMMONCONTROLSEX InitCtrls;
        InitCtrls.dwSize = sizeof(InitCtrls);
        // Set this to include all the common control classes you want to use
        // in your application.
        InitCtrls.dwICC = ICC_WIN95_CLASSES;
        InitCommonControlsEx(&InitCtrls);

        CWinApp::InitInstance();

		////############## ���α׷� �ߺ����� üũ ###############
		if (!checkDuplicateExecute()) { /// FALSE �� �̹� ����ǰ� �ִٴ� ��.
			return FALSE;
		}

		//###################### �������� ��� ����
		TCHAR buffer[MAX_PATH];
		GetCurrentDirectory(MAX_PATH, buffer); // buffer�� ���� �������α׷��� ����Ǵ� ��θ� ���´�.
		g_strImagePath.Format(L"%s\\_dimages", buffer);
		
		///#### make global variables.
		gDss_status = new CD_ss_status();
		gDs_sensors = new CD_s_sensors();
		gDvsettings = new CD_vsettings();

	// ###################### DDDDD ����. �ܼ��Ҵ�.####################################
#ifdef _DEBUG
#ifndef FILE_DDDDD
	AllocConsole();///alloc DDDDD consol debugging window.
#endif
#endif

		DDDDD(_T("Detect Image Saving Path: %s\r\n"), g_strImagePath);
		if (!IsDirectoryExist(g_strImagePath)) CreateDirectory(g_strImagePath, NULL);

        // Standard initialization
        // If you are not using these features and wish to reduce the size
        // of your final executable, you should remove from the following
        // the specific initialization routines you do not need
        // Change the registry key under which our settings are stored
        // TODO: You should modify this string to be something appropriate
        // such as the name of your company or organization
        //SetRegistryKey(_T("Local AppWizard-Generated Applications"));

        Gdiplus::GdiplusStartupInput gdiplusStartupInput;
        ULONG_PTR gdiplusToken;
        GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);

        CVidCapDlg dlg;
        m_pMainWnd = &dlg;
        INT_PTR nResponse = dlg.DoModal();
        if (nResponse == IDOK) {
                // TODO: Place code here to handle when the dialog is
                //  dismissed with OK
        } else if (nResponse == IDCANCEL) {
                // TODO: Place code here to handle when the dialog is
                //  dismissed with Cancel
        }

        Gdiplus::GdiplusShutdown(gdiplusToken);

		//####
		delete gDss_status; gDss_status = NULL;
		delete gDs_sensors; gDss_status = NULL;
		delete gDvsettings; gDvsettings = NULL;


		//########################## DDDDD ����. console ��ȯ 
#ifdef _DEBUG
#ifdef FILE_DDDDD
		if (f_ddd) { fclose(f_ddd); }
#else
		FreeConsole();
#endif
#endif
		checkReleaseMutex();//######���ؽ� ����.

        // Since the dialog has been closed, return FALSE so that we exit the
        //  application, rather than start the application's message pump.
        return FALSE;
}

// ###########################################################################################
/// ���ؽ��� �̿��Ͽ� ���α׷� �ߺ��������.
// ###########################################################################################
BOOL CVidCapApp::checkDuplicateExecute()
{
	DWORD dwError;

	m_hMutex = ::CreateMutex(NULL, TRUE, STR_THIS_REG_KEY_NAME);
	dwError = ::GetLastError();

	if (m_hMutex == NULL)
		return FALSE;

	if (dwError == ERROR_ALREADY_EXISTS){
		::ReleaseMutex(m_hMutex);
		::CloseHandle(m_hMutex);
		return FALSE;
	}

	return TRUE;

}

// ###########################################################################################
/// ���α׷� �ߺ����� ������ ���� ������ ���ؽ� ����.
// ###########################################################################################
void CVidCapApp::checkReleaseMutex()
{
	::ReleaseMutex(m_hMutex);
	::CloseHandle(m_hMutex);
	m_hMutex = NULL;
}

// ###########################################################################################
BOOL CVidCapApp::IsDirectoryExist(const CString &dirPath)
{
	if (_T("") == dirPath){
		return FALSE;
	}
	DWORD dwAttrib = GetFileAttributes(dirPath);

	// �������� �ʰų� Folder�� �ƴϸ� 
	if (dwAttrib == 0xffffffff || (dwAttrib & FILE_ATTRIBUTE_DIRECTORY) != FILE_ATTRIBUTE_DIRECTORY){
		return FALSE;
	}		

	return TRUE;

}
 


