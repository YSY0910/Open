#pragma once
#include <string> /// for use std::string 
#include "Data_Structure.h"

//##########################################################################################
// Consol Debug function DDDDD 
//##########################################################################################
#ifdef _DEBUG // jslee. for debugging: consol mode.
extern void DDDDD(CString fmt, ...);
//	DWORD m_dwLengthDStr = 0;
//#define CDBGPRINT1(s) WriteConsole(GetStdHandle(STD_OUTPUT_HANDLE),s,s.GetLength(),&m_dwLengthDStr,NULL);
//#define CDBGPRINT2(s) WriteConsole(GetStdHandle(STD_OUTPUT_HANDLE),CString(CString(##s)+CString(_T("\r\n"))),CString(CString(##s)+CString(_T("\r\n"))).GetLength(),NULL,NULL)
#else
#define DDDDD(s,...)
//#define CDBGPRINT1(s)
//#define CDBGPRINT2(s)
#endif

#pragma warning(disable:4996)

#define STR_THIS_REG_KEY_NAME		_T("_VID_CAP_")	


extern CD_ss_status *gDss_status; //ss_status ������ �����
extern CD_s_sensors *gDs_sensors; // s_sensors ������ �����.
extern CD_vsettings *gDvsettings;//vsettings ������ �����. 

//extern CString g_strRunningPath;			// ���α׷��� ���۵Ǵ� ���.
extern CString g_strImagePath;			//ħ���̹��� ������.