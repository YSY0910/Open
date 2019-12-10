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


extern CD_ss_status *gDss_status; //ss_status 데이터 저장소
extern CD_s_sensors *gDs_sensors; // s_sensors 데이터 저장소.
extern CD_vsettings *gDvsettings;//vsettings 데이터 저장소. 

//extern CString g_strRunningPath;			// 프로그램이 시작되는 경로.
extern CString g_strImagePath;			//침입이미지 저장경로.