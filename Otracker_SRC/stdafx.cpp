// stdafx.cpp : source file that includes just the standard includes
// VidCap.pch will be the pre-compiled header
// stdafx.obj will contain the pre-compiled type information

#include "stdafx.h"


#ifdef _DEBUG //--------------------------->
//// #define FILE_DDDDD	/// DDDDD 메시지의 파일 출력.

#ifdef FILE_DDDDD
FILE *f_ddd = NULL;
#endif

void DDDDD(CString fmt, ...){
	va_list args;
	va_start(args, fmt);

	CString s;
	if (args){ //  null check. 
		s.FormatV(fmt, args);
	}
	va_end(args);
	//m_dwLengthDStr = s.GetLength();
	//WriteConsole(GetStdHandle(STD_OUTPUT_HANDLE),s,s.GetLength(),&m_dwLengthDStr,NULL);
	//WriteConsole(GetStdHandle(STD_OUTPUT_HANDLE),s,s.GetLength()/sizeof(TCHAR)-1,NULL,NULL);
#ifdef FILE_DDDDD
	CTime t = CTime::GetCurrentTime();
	struct _timeb tstruct;
	_ftime(&tstruct);
	CString tmS;
	// 날짜+밀리초 로 파일명 작성. 
	if (s.GetLength() > 2){
		tmS.Format(L"%02d.%02d.%02d:%s", t.GetHour(), t.GetMinute(), t.GetSecond(), s);
	}
	else{
		tmS = s;
	}

	if (!f_ddd){
		f_ddd = fopen("msg_log.txt", "wb");
		fwrite((LPSTR)(LPCTSTR)tmS, 2, tmS.GetLength(), f_ddd);
	}
	else{
		fwrite((LPSTR)(LPCTSTR)tmS, 2, tmS.GetLength(), f_ddd);
	}

#else
	WriteConsole(GetStdHandle(STD_OUTPUT_HANDLE), s, s.GetLength(), NULL, NULL);
#endif	
};
#endif // <---------------------------------------------------------------------