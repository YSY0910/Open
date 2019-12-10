#include "stdafx.h"
#include "VidCap.h"
#include "PGUtils.h"


// ###########################################################################################
// 
// ###########################################################################################
PGconn* ConnectDB()
{		
	//if (!G_STATUS_ST100CONNECTION) return NULL;
	//if (CSTR_IP_NOFOUND == g_strST100IP) return NULL;

	PGconn *conn = NULL;
	const char* pghost = "127.0.0.1"; // ConvertUnicodeToMultybyte(g_strST100IP);
	const char *pgport = "9603";
	const char *pdbName = "xsecure";
	const char *pdbUser = "postgres";
	const char *pdbPassword = "admin0"; 

	/* Make a connection to the database */
	conn = PQsetdbLogin(pghost, pgport, NULL, NULL, pdbName, pdbUser, pdbPassword);

	//free((char*)pghost); /// see ConvertUnicodeToMultybyte().
	//delete [] pghost;

	/* Check to see that the backend connection was successfully made */
	if (PQstatus(conn) != CONNECTION_OK){
		/* error */
		DDDDD(_T("ERR DATABASECONNECT FAILED: 0x%x \r\n"), conn);
		return conn;
	}
	//DDDDD(_T("DATABASECONNECT Success!!: 0x%x \r\n"), conn);

	return conn;

}


// ###########################################################################################
// 
// ###########################################################################################
void CloseDB(PGconn *connection)
{
	ASSERT(connection); // only valid in debug-mode.
	if (!connection) {
		DDDDD(_T("Invalid] DBConnection is NULL. Don't need to close connection.\r\n"));
		return;
	}

	PQfinish(connection);
	connection = NULL;

}

//#######################################################
// 프로그램 시작시.. ss_status 테이블에 값이 있으면 삭제.(모든열 삭제).
int Delete_All_ss_status()
{
	PGconn *conn = NULL;

	conn = ConnectDB();
	if (conn == NULL) { return -1; }

	std::string sSQL;

	sSQL.append("DELETE FROM ss_status;"); //모든 아이템 삭제. 
	PGresult *res= PQexec(conn, sSQL.c_str());
	if (PQresultStatus(res) != PGRES_COMMAND_OK){	//쿼리의 성공 여부를 판단.
		DDDDD(_T("[failed]query Delete from ss_status failed!\r\n"));
		PQclear(res);
		PQfinish(conn);
		return -1;
	}

	PQclear(res);
	PQfinish(conn);
	DDDDD(_T("[success] all items in ss_table are deleted!\r\n"));
	return 0;
}

//#######################################################
// 프로그램 시작시.
// ss_status table 값을 초기값으로 리셋한 열 삽입.
int  InsertBasic_ss_status() 
{
	PGconn *conn = NULL;

	conn = ConnectDB();
	if (conn == NULL) { return -1; }

	std::string sSQL;

	sSQL.append("INSERT INTO ss_status(srun,vrun,odetect) VALUES('0','0','0');"); //초기값을 모두 0으로 준다. 
	PGresult *res= PQexec(conn, sSQL.c_str());
	if (PQresultStatus(res) != PGRES_COMMAND_OK){	//쿼리의 성공 여부를 판단.
		DDDDD(_T("[failed]query NEW ITEM INSERT ss_status failed!\r\n"));
		PQclear(res);
		PQfinish(conn);
		return -1;
	}

	PQclear(res);
	PQfinish(conn);
	DDDDD(_T("[success] insert new item to ss_status.\r\n"));

	return 0;
}


//#######################################################
BOOL Update_ss_status(const CD_ss_status *pSD) 
{
	PGconn *conn = NULL;
	conn = ConnectDB();

	if (NULL == conn) {
		return FALSE;
	}	

	CD_ss_status sV;
	sV = *pSD; // 값을 복사.

	CW2A my_srun(sV.sSRun, CP_UTF8);
	CW2A my_vrun(sV.sVRun, CP_UTF8);
	CW2A my_odetect(sV.sODetect, CP_UTF8);

	std::string sSQL;
	sSQL.append("UPDATE ss_status SET srun = '");
	sSQL.append(my_srun);		sSQL.append("',vrun = '");
	sSQL.append(my_vrun);		sSQL.append("',odetect = '");
	sSQL.append(my_odetect);		sSQL.append("'");

	//DDDDD(_T("%S\r\n"), sSQL.c_str());
	PGresult *resPI = PQexec(conn, sSQL.c_str());
	if (PQresultStatus(resPI) != PGRES_COMMAND_OK)	//쿼리의 성공 여부를 판단.
	{
		DDDDD(_T("%S\r\n"), sSQL.c_str());
		DDDDD(_T("[failed] update ss_status item.\r\n"));
		PQclear(resPI);
		PQfinish(conn);
		return FALSE;
	}

	PQclear(resPI);
	PQfinish(conn);
	DDDDD(_T("[success] update ss_status item.\r\n"));
	return TRUE;
}


//#######################################################
// vsettings 테이블의 모든 열을 삭제
int Delete_All_vsettings() 
{
	PGconn *conn = NULL;

	conn = ConnectDB();
	if (conn == NULL) { return -1; }

	std::string sSQL;

	sSQL.append("DELETE FROM vsettings;"); //모든 아이템 삭제. 
	PGresult *res= PQexec(conn, sSQL.c_str());
	if (PQresultStatus(res) != PGRES_COMMAND_OK){	//쿼리의 성공 여부를 판단.
		DDDDD(_T("[failed]query Delete from vsettings failed!\r\n"));
		PQclear(res);
		PQfinish(conn);
		return -1;
	}

	PQclear(res);
	PQfinish(conn);
	DDDDD(_T("[success] all items in vsettings are deleted!\r\n"));
	return 0;
}

//#######################################################
// 이 함수를 호출하기전 pVS->SetToDefault()를 실행할것.
int InsertBasic_vsettings(const CD_vsettings* pVS)
{
	PGconn *conn = NULL;
	conn = ConnectDB();

	if (NULL == conn) {
		return FALSE;
	}

	CD_vsettings sV;
	sV = *pVS; // 값을 복사.

	CW2A sDTSize(sV.sDTSize, CP_UTF8);
	CW2A sSInt(sV.sSInt, CP_UTF8);
	CW2A sSTemMin(sV.sSTemMin, CP_UTF8);
	CW2A sSTemMax(sV.sSTemMax, CP_UTF8);
	CW2A sSGasMax(sV.sSGasMax, CP_UTF8);
	CW2A sSFlarMax(sV.sSFlarMax, CP_UTF8);
	CW2A sSODCnt(sV.sODCnt, CP_UTF8);
	CW2A sSSDCnt(sV.sSDCnt, CP_UTF8);
	CW2A sSISaveInt(sV.sSISaveInt, CP_UTF8);
	CW2A sSmailInt(sV.sSmailInt, CP_UTF8);
	CW2A sSmailMax(sV.sSmailMax, CP_UTF8);
	CW2A sSmailAddr(sV.sSmailAddr, CP_UTF8);
	CW2A sSMailPwd(sV.sSMailPwd, CP_UTF8);

	std::string sSQL;

	sSQL.append("INSERT INTO vsettings(dt_size,s_int,s_tem_min,s_tem_max,s_gas_max,s_flar_max,odcnt,sdcnt,isave_int,smail_int,smail_max,smail_addr,smail_pwd)"); 
	sSQL.append(" VALUES ('");
	sSQL.append(sDTSize);	sSQL.append("','");
	sSQL.append(sSInt);	sSQL.append("','");
	sSQL.append(sSTemMin);	sSQL.append("','");
	sSQL.append(sSTemMax);	sSQL.append("','");
	sSQL.append(sSGasMax);	sSQL.append("','");
	sSQL.append(sSFlarMax);	sSQL.append("','");
	sSQL.append(sSODCnt);	sSQL.append("','");
	sSQL.append(sSSDCnt);	sSQL.append("','");
	sSQL.append(sSISaveInt);	sSQL.append("','");
	sSQL.append(sSmailInt);	sSQL.append("','");
	sSQL.append(sSmailMax);	sSQL.append("','");
	sSQL.append(sSmailAddr);	sSQL.append("','");
	sSQL.append(sSMailPwd);	sSQL.append("')");

	PGresult *res= PQexec(conn, sSQL.c_str());
	if (PQresultStatus(res) != PGRES_COMMAND_OK){	//쿼리의 성공 여부를 판단.
		DDDDD(_T("[failed]query NEW ITEM INSERT vsettings failed!\r\n"));
		PQclear(res);
		PQfinish(conn);
		return -1;
	}

	PQclear(res);
	PQfinish(conn);
	DDDDD(_T("[success] insert new item to vsettings.\r\n"));
	return 0;
}

//#######################################################
BOOL Update_vsettings(const CD_vsettings* pVS)
{
	PGconn *conn = NULL;
	conn = ConnectDB();

	if (NULL == conn) {
		return FALSE;
	}	

	CD_vsettings sV;
	sV = *pVS; // 값을 복사.

	CW2A sDTSize(sV.sDTSize, CP_UTF8);
	CW2A sSInt(sV.sSInt, CP_UTF8);
	CW2A sSTemMin(sV.sSTemMin, CP_UTF8);
	CW2A sSTemMax(sV.sSTemMax, CP_UTF8);
	CW2A sSGasMax(sV.sSGasMax, CP_UTF8);
	CW2A sSFlarMax(sV.sSFlarMax, CP_UTF8);
	CW2A sSODCnt(sV.sODCnt, CP_UTF8);
	CW2A sSSDCnt(sV.sSDCnt, CP_UTF8);
	CW2A sSISaveInt(sV.sSISaveInt, CP_UTF8);
	CW2A sSmailInt(sV.sSmailInt, CP_UTF8);
	CW2A sSmailMax(sV.sSmailMax, CP_UTF8);
	CW2A sSmailAddr(sV.sSmailAddr, CP_UTF8);
	CW2A sSMailPwd(sV.sSMailPwd, CP_UTF8);

	std::string sSQL;
	sSQL.append("UPDATE vsettings SET dt_size = '");
	sSQL.append(sDTSize);		sSQL.append("',s_int = '");
	sSQL.append(sSInt);		sSQL.append("',s_tem_min = '");
	sSQL.append(sSTemMin);		sSQL.append("',s_tem_max = '");
	sSQL.append(sSTemMax);		sSQL.append("',s_gas_max = '");
	sSQL.append(sSGasMax);		sSQL.append("',s_flar_max = '");
	sSQL.append(sSFlarMax);		sSQL.append("',odcnt = '");
	sSQL.append(sSODCnt);		sSQL.append("',sdcnt = '");
	sSQL.append(sSSDCnt);		sSQL.append("',isave_int = '");
	sSQL.append(sSISaveInt);		sSQL.append("',smail_int = '");
	sSQL.append(sSmailInt);		sSQL.append("',smail_max = '");
	sSQL.append(sSmailMax);		sSQL.append("',smail_addr = '");
	sSQL.append(sSmailAddr);		sSQL.append("',smail_pwd = '");
	sSQL.append(sSMailPwd);		sSQL.append("'");

	//DDDDD(_T("%S\r\n"), sSQL.c_str());
	PGresult *resPI = PQexec(conn, sSQL.c_str());
	if (PQresultStatus(resPI) != PGRES_COMMAND_OK)	//쿼리의 성공 여부를 판단.
	{
		DDDDD(_T("%S\r\n"), sSQL.c_str());
		DDDDD(_T("[failed] update vsettings item.\r\n"));
		PQclear(resPI);
		PQfinish(conn);
		return FALSE;
	}

	PQclear(resPI);
	PQfinish(conn);
	DDDDD(_T("[success] update vsettings item.\r\n"));

	return TRUE;
}


BOOL Read_vsettings(CD_vsettings &oVS) 
{
	PGconn *conn = NULL;
	conn = ConnectDB();
	if (NULL == conn) { return FALSE; }

	std::string sSQL;

	sSQL.append("SELECT * FROM vsettings ORDER BY index DESC LIMIT 1");

	// #######Execute with sql statement
	PGresult *res = PQexec(conn, sSQL.c_str());

	//쿼리의 성공 여부를 판단. 데이터가 0개 여도 여기는 통과 하게 되어 있다.
	if ((PQresultStatus(res) != PGRES_TUPLES_OK)) {
		PQclear(res);	PQfinish(conn);
		// message box need.
		DDDDD(_T("[failed] Select * vsettings Query is failed.\r\n"));
		return FALSE;
	}
	else if ((PQntuples(res) == 0)) {//쿼리로 받아온 데이터의 개수 0 개면 데이터가 없는것이다.
		PQclear(res);	PQfinish(conn);
		// message box need.
		DDDDD(_T("[failed] Select * vsettings SQL Result is 0.\r\n"));
		return FALSE;
	}

	// Get the field name
	int nFields = PQnfields(res);

	char *recvFieldName = NULL;
	char *recvFieldData = NULL;
	//CString recvData;

	for (int i = 0; i < PQntuples(res); i++) {
		for (int j = 0; j < nFields; j++) {

			recvFieldName = PQfname(res, j);
			recvFieldData = PQgetvalue(res, i, j);

			ASSERT(recvFieldName && recvFieldData);

			// CA2W를 2번 연속쓰니.. 안된다???=>//DDDDD(_T("[%d] %s  /  %s \r\n"), i, CA2W(recvFieldName, CP_UTF8), CA2W(recvFieldData, CP_UTF8)); 
			//DDDDD(_T("[%d] %s  /  %s \r\n"),i,(CString)recvFieldName,CA2W(recvFieldData, CP_UTF8));

			if (!(strcmp(recvFieldName, "dt_size"))) {
				oVS.sDTSize = CA2W(recvFieldData, CP_UTF8);
			}
			else if (!(strcmp(recvFieldName, "s_int"))) {
				oVS.sSInt = CA2W(recvFieldData, CP_UTF8);
			}
			else if (!(strcmp(recvFieldName, "s_tem_min"))) {
				oVS.sSTemMin = CA2W(recvFieldData, CP_UTF8);
			}
			else if (!(strcmp(recvFieldName, "s_tem_max"))) { 
				oVS.sSTemMax = CA2W(recvFieldData, CP_UTF8);
			}
			else if (!(strcmp(recvFieldName, "s_gas_max"))) {
				oVS.sSGasMax = CA2W(recvFieldData, CP_UTF8);
			}
			else if (!(strcmp(recvFieldName, "s_flar_max"))) {
				oVS.sSFlarMax = CA2W(recvFieldData, CP_UTF8);
			}
			else if (!(strcmp(recvFieldName, "odcnt"))) {
				oVS.sODCnt = CA2W(recvFieldData, CP_UTF8);
			}
			else if (!(strcmp(recvFieldName, "sdcnt"))) {
				oVS.sSDCnt = CA2W(recvFieldData, CP_UTF8);
			}
			else if (!(strcmp(recvFieldName, "isave_int"))) {
				oVS.sSISaveInt = CA2W(recvFieldData, CP_UTF8);
			}
			else if (!(strcmp(recvFieldName, "smail_int"))) {
				oVS.sSmailInt = CA2W(recvFieldData, CP_UTF8);
			}
			else if (!(strcmp(recvFieldName, "smail_max"))) {
				oVS.sSmailMax = CA2W(recvFieldData, CP_UTF8);
			}
			else if (!(strcmp(recvFieldName, "smail_addr"))) {
				oVS.sSmailAddr = CA2W(recvFieldData, CP_UTF8);
			}
			else if (!(strcmp(recvFieldName, "smail_pwd"))) {
				oVS.sSMailPwd = CA2W(recvFieldData, CP_UTF8);
			}

		}//end of inner for()

	}//end of for()

	//######DB close.	
	PQclear(res);
	PQfinish(conn);
	DDDDD(_T("[success] select * from vsettings.\r\n"));

	return TRUE;
}


//#######################################################
BOOL Read_s_sensors(CD_s_sensors& oSS) 
{
	PGconn *conn = NULL;
	conn = ConnectDB();
	if (NULL == conn) { return FALSE; }

	std::string sSQL;

	sSQL.append("SELECT * FROM s_sensors ORDER BY index DESC LIMIT 1");

	// #######Execute with sql statement
	PGresult *res = PQexec(conn, sSQL.c_str());

	//쿼리의 성공 여부를 판단. 데이터가 0개 여도 여기는 통과 하게 되어 있다.
	if ((PQresultStatus(res) != PGRES_TUPLES_OK)) {
		PQclear(res);	PQfinish(conn);
		// message box need.
		DDDDD(_T("[failed] Select * s_sensors Query is failed.\r\n"));
		return FALSE;
	}
	else if ((PQntuples(res) == 0)) {//쿼리로 받아온 데이터의 개수 0 개면 데이터가 없는것이다.
		PQclear(res);	PQfinish(conn);
		// message box need.
		DDDDD(_T("[failed] Select * s_sensors SQL Result is 0.\r\n"));
		return FALSE;
	}

	// Get the field name
	int nFields = PQnfields(res);

	char *recvFieldName = NULL;
	char *recvFieldData = NULL;
	//CString recvData;

	for (int i = 0; i < PQntuples(res); i++) {
		for (int j = 0; j < nFields; j++) {

			recvFieldName = PQfname(res, j);
			recvFieldData = PQgetvalue(res, i, j);

			ASSERT(recvFieldName && recvFieldData);

			// CA2W를 2번 연속쓰니.. 안된다???=>//DDDDD(_T("[%d] %s  /  %s \r\n"), i, CA2W(recvFieldName, CP_UTF8), CA2W(recvFieldData, CP_UTF8)); 
			//DDDDD(_T("[%d] %s  /  %s \r\n"),i,(CString)recvFieldName,CA2W(recvFieldData, CP_UTF8));

			if (!(strcmp(recvFieldName, "s_tem"))) {
				oSS.sSTem = CA2W(recvFieldData, CP_UTF8);
			}
			else if (!(strcmp(recvFieldName, "s_gas"))) {
				oSS.sSGas = CA2W(recvFieldData, CP_UTF8);
			}
			else if (!(strcmp(recvFieldName, "s_flar"))) {
				oSS.sSFlar = CA2W(recvFieldData, CP_UTF8);
			}

		}//end of inner for()

	}//end of for()

	//######DB close.	
	PQclear(res);
	PQfinish(conn);
	//DDDDD(_T("[success] select * from s_sensors.\r\n"));

	return TRUE;
}


//#######################################################
// 감지된 물체가 있을 경우, 영상이미지와, 경로를 s_odimages table에 저장하기 위함.
int Insert_DetectImageInfo(const CString &myPath, const CString &myImage)
{
	PGconn *conn = NULL;
	conn = ConnectDB();

	if (NULL == conn) {
		return FALSE;
	}

	CW2A sPath(myPath, CP_UTF8);
	CW2A sImage(myImage, CP_UTF8);

	std::string sSQL;

	sSQL.append("INSERT INTO s_odimages(spath,dimg)"); 
	sSQL.append(" VALUES ('");
	sSQL.append(sPath);	sSQL.append("','");
	sSQL.append(sImage);	sSQL.append("')");

	PGresult *res= PQexec(conn, sSQL.c_str());
	if (PQresultStatus(res) != PGRES_COMMAND_OK){	//쿼리의 성공 여부를 판단.
		DDDDD(_T("[failed]query NEW ITEM INSERT s_odimages failed!\r\n"));
		PQclear(res);
		PQfinish(conn);
		return -1;
	}

	PQclear(res);
	PQfinish(conn);
	//DDDDD(_T("[success] Detected image info save: %s, %s\r\n"), myPath, myImage);
	return 0;
}