#pragma once

#include "stdafx.h"
#include <atlstr.h>
#include <VECTOR>
#include <algorithm>

using namespace std;
#define CSTR_DATA_NULL	_T("")
#define CSTR_DATA_ZERO _T("0")

#define CSTR_ALPHANUMERIC_VALID		_T("ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789 !@#$%^&()~`-*?<>.,") // 유효한 alphanumeric character.
#define CSTR_INTEGER_VALID			_T("0123456789")				// 0~9 정수 문자 유효한 값.
#define CSTR_FLAG_INTEGER_VALID		_T("0123456789-")				// 부호있는 정수
#define CSTR_DECIMAL_VALID			_T("0123456789.")				// 실수 표현.

//#############[ Data-Structure ]#############################################################

// ###########################################################################################
/// 
// ###########################################################################################
class CD_ss_status
{
public:
	CD_ss_status() { InitData(); }
	~CD_ss_status() { EmptyData(); }

	void Clear() {
		InitData();	
		//EmptyData();
	}

	void InitData() {
		//sSRun = CSTR_DATA_NULL; sVRun = CSTR_DATA_NULL;  sODetect = CSTR_DATA_NULL;
		sSRun = CSTR_DATA_ZERO; sVRun = CSTR_DATA_ZERO; sODetect = CSTR_DATA_ZERO;
	}

	void EmptyData() {
		sSRun.Empty();		sVRun.Empty();			sODetect.Empty();
	}

	CString sSRun; //
	CString sVRun; //
	CString sODetect; //

	// redefine operator '='
	CD_ss_status& operator= (const CD_ss_status& t)
	{
		sSRun = t.sSRun;			sVRun = t.sVRun;			sODetect = t.sODetect;
		return *this; // caution.
	}
};

// ###########################################################################################
/// 
// ###########################################################################################
class CD_ss_odimages
{
public:
	CD_ss_odimages() { InitData(); }
	~CD_ss_odimages() { EmptyData(); }

	void Clear() {
		InitData();	
		//EmptyData();
	}

	void InitData() {
		sPath = CSTR_DATA_NULL; sFName = CSTR_DATA_NULL; 
		//sSRun = CSTR_DATA_ZERO; sVRun = CSTR_DATA_ZERO;
	}

	void EmptyData() {
		sPath.Empty();		sFName.Empty();
	}

	CString sPath; //
	CString sFName; //

	// redefine operator '='
	CD_ss_odimages& operator= (const CD_ss_odimages& t)
	{
		sPath = t.sPath;			sFName = t.sFName;
		return *this; // caution.
	}
};
// ###########################################################################################
/// 
// ###########################################################################################
class CD_s_sensors
{
public:
	CD_s_sensors() { InitData(); }
	~CD_s_sensors() { EmptyData(); }

	void Clear() {
		InitData();	
		//EmptyData();
	}

	void InitData() {
		//sSTem = CSTR_DATA_NULL; sSGas = CSTR_DATA_NULL;  sSFlar = CSTR_DATA_NULL;
		sSTem = CSTR_DATA_ZERO; sSGas = CSTR_DATA_ZERO; sSFlar = CSTR_DATA_ZERO;
	}

	void EmptyData() {
		sSTem.Empty();		sSGas.Empty();			sSFlar.Empty();
	}

	CString sSTem; //
	CString sSGas; // 
	CString sSFlar; // 

	// redefine operator '='
	CD_s_sensors& operator= (const CD_s_sensors& t)
	{
		sSTem = t.sSTem;			sSGas = t.sSGas;			sSFlar = t.sSFlar;

		return *this; // caution.
	}
};

// ###########################################################################################
/// 
// ###########################################################################################
class CD_vsettings
{
public:
	CD_vsettings() { InitData(); }
	~CD_vsettings() { EmptyData(); }

	void Clear() {
		InitData();	
		//EmptyData();
	}

	void InitData() {
#if false
		sDTSize = CSTR_DATA_NULL; sSInt = CSTR_DATA_NULL;  sSTemMin = CSTR_DATA_NULL;
		sSTemMax = CSTR_DATA_NULL; sSGasMax = CSTR_DATA_NULL;  sSFlarMax = CSTR_DATA_NULL;
		sSISaveInt = CSTR_DATA_NULL; sSmailInt = CSTR_DATA_NULL;  sSmailMax = CSTR_DATA_NULL;
		sSmailAddr = CSTR_DATA_NULL; sSMailPwd = CSTR_DATA_NULL;
#else
		sDTSize = CSTR_DATA_ZERO; sSInt = CSTR_DATA_ZERO;  sSTemMin = CSTR_DATA_ZERO;
		sSTemMax = CSTR_DATA_ZERO; sSGasMax = CSTR_DATA_ZERO;  sSFlarMax = CSTR_DATA_ZERO;
		sSISaveInt = CSTR_DATA_ZERO; sSmailInt = CSTR_DATA_ZERO;  sSmailMax = CSTR_DATA_ZERO;
		sSmailAddr = CSTR_DATA_ZERO; sSMailPwd = CSTR_DATA_ZERO;
#endif
	}

	void SetToDefault() {
		sDTSize = _T("0"); sSInt = _T("1000");  sSTemMin = _T("0");
		sSTemMax = _T("42"); sSGasMax = _T("120");  sSFlarMax = _T("80");
		sODCnt = _T("10");     sSDCnt = _T("10");
		sSISaveInt = _T("5"); sSmailInt = _T("10");  sSmailMax = _T("50");
		sSmailAddr = _T("Your_Gmail_ address"); sSMailPwd = _T("Gmail_Password");
	}

	void EmptyData() {
		sDTSize.Empty();		sSInt.Empty();			sSTemMin.Empty();
		sSTemMax.Empty();		sSGasMax.Empty();			sSFlarMax.Empty();
		sODCnt.Empty();			sSDCnt.Empty();
		sSISaveInt.Empty();		sSmailInt.Empty();			sSmailMax.Empty();
		sSmailAddr.Empty();		sSMailPwd.Empty();
	}

	CString sDTSize; // 물체인식 최소사이즈 0: 최소사이즈 제한없음, 1: 중간사이즈 이상, 2: 큰사이즈 이상.
	CString sSInt; // 센서값 읽는 주기. 
	CString sSTemMin; // 저온알림 기준값
	CString sSTemMax;	// 고온알림 기준값
	CString sSGasMax; // 가스이상 감지 기준값
	CString sSFlarMax; // 불꽃감지 기준값
	CString sODCnt; //  물체침입 N회 이상시 칩입으로 설정.
	CString sSDCnt; // 온도.가스.불꽃 알림기준 N회 이상시 이상감지로 설정
	CString sSISaveInt; // 침입 또는 온도.가스.불꽃 이상감지시 이미지 저장 주기
	CString sSmailInt; // 침입 또는 온조.가스.불꽃 이상감지시 메일전송주기
	CString sSmailMax; // 침입 또는 온조.가스.불꽃 이상감지시 메일전송 최대회수.
	CString sSmailAddr; // 메일 송.수신을 위한 gmail 계정
	CString sSMailPwd; // 위 메일계정의 비밀번호.

	// redefine operator '='
	CD_vsettings& operator= (const CD_vsettings& t)
	{
		sDTSize = t.sDTSize;			sSInt = t.sSInt;			sSTemMin = t.sSTemMin;
		sSTemMax = t.sSTemMax;				sSGasMax = t.sSGasMax;		sSFlarMax = t.sSFlarMax;
		sODCnt = t.sODCnt;	sSDCnt = t.sSDCnt;
		sSISaveInt = t.sSISaveInt;	sSmailInt = t.sSmailInt;	sSmailMax = t.sSmailMax;
		sSmailAddr = t.sSmailAddr;			sSMailPwd = t.sSMailPwd;

		return *this; // caution.
	}
};