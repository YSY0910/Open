#pragma once
#include "stdafx.h"
#include "libpq-fe.h"
#include "Data_Structure.h"

//DB �� �����Լ�. 
PGconn *ConnectDB();

//DB ���� ����
void CloseDB(PGconn *connection);

//#######################################################
// ���α׷� ���۽�.
int Delete_All_ss_status();
int InsertBasic_ss_status();

// read ss_status table 
//void Read_ss_status(CString &strSRun, CString &strVRun, CString &strODetect );

// update ss_status table.
//void Update_ss_status(CString strSRun, CString strVRun, CString strODetect);
BOOL Update_ss_status(const CD_ss_status* pSD);


//#######################################################
int Delete_All_vsettings();
int InsertBasic_vsettings(const CD_vsettings* pVS);
BOOL Update_vsettings(const CD_vsettings* pVS);
BOOL Read_vsettings(CD_vsettings &oVS);

//#######################################################
BOOL Read_s_sensors(CD_s_sensors &oSS);

int Insert_DetectImageInfo(const CString& myPath, const CString& myImage);