// VidCapDlg.cpp : implementation file
//

#include "stdafx.h"
#include "VidCap.h"
#include "VidCapDlg.h"

#include "capture.h"
#include "samplegrab.h"

#include "lib\vec2D.h"
#include "lib\vec2Dc.h"
#include "libpq-fe.h"
#include "PGUtils.h"

#include <Windows.h>
#include <shellapi.h>
#include <tlhelp32.h> //�������� ���μ��� Ȯ���� ����.

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


#pragma


LARGE_INTEGER m_nFreq;
LARGE_INTEGER m_nBeginTime;

void tic()
{
        QueryPerformanceFrequency(&m_nFreq);
        QueryPerformanceCounter(&m_nBeginTime);
}
__int64 toc()
{
        LARGE_INTEGER nEndTime;
        __int64 nCalcTime;

        QueryPerformanceCounter(&nEndTime);
        nCalcTime = (nEndTime.QuadPart - m_nBeginTime.QuadPart) * 1000 / m_nFreq.QuadPart;

        return nCalcTime;
}


// CVidCapDlg dialog
CVidCapDlg::CVidCapDlg(CWnd* pParent /*=NULL*/)
                : CDialog(CVidCapDlg::IDD, pParent)
                , m_Width(640), m_Height(480), m_Channels(3)
                , m_Zoom(0.125f)
                , m_nTimer(0), m_TimerInterval(1000)                                
                , m_TakeSnapshot(false)
                , pBmpEncoder(GUID_NULL)
                , m_CaptureState(HALT)
                , m_Background(0)
                , m_BackgroundFramesNumber(0)                
                , m_MinBlobElements(10)
                , m_TrackingStatus(_T(""))
				, m_NowAlarmCount(0)
				,m_StartAlarmCount(0)
				,m_NowSenAlarmCount(0)
				,m_StartSenAlarmCount(0)
				,m_TemMin(0)
				,m_TemMax(0)
				,m_GasMax(0)
				,m_FlarMax(0)
				,m_ImgSaveInt(5)
	, m_StrSensingInterval(_T("")), m_StrTempMin(_T("")), m_StrTempMax(_T("")), m_StrGasMax(_T("")), m_StrFlarMax(_T("")), m_StrSImageInt(_T("")), m_StrSMailInt(_T("")), m_SMailMax(_T("")), m_StrGmailAccount(_T("")), m_StrGmailPWD(_T("")), m_StrODCountMax(_T("")), m_StrSensorAlarmCountMax(_T("")), m_StrTemperature(_T("")), m_StrGas(_T("")), m_StrFlar(_T("")) {
        m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
		ZeroMemory(&m_sLatteTestInfo, sizeof(SHELLEXECUTEINFO));
}

void CVidCapDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_PRV_STATIC, m_PrvStatic);
	DDX_Control(pDX, IDC_ADAPTORS_COMBO, m_AdapterCombo);
	DDX_Text(pDX, IDC_SAMPLEINTERVAL_EDIT, m_TimerInterval);
	DDV_MinMaxUInt(pDX, m_TimerInterval, 10, 10000);
	DDX_Control(pDX, IDC_RUN_BUTTON, m_RunButton);
	DDX_Control(pDX, IDC_CAPIMG_STATIC, m_CapImgStatic);
	DDX_Control(pDX, IDC_VIDINFO_STATIC, m_VideoFormat);
	DDX_Text(pDX, IDC_STATUS_STATIC, m_TrackingStatus);
	DDX_Control(pDX, IDC_PIC_SLOGO, m_PicSLogo);
	DDX_Control(pDX, IDC_BTN_START_SECURE, m_StartSecure);
	DDX_Text(pDX, IDC_EDIT_SINT, m_StrSensingInterval);
	DDX_Control(pDX, IDC_COMBO_ODSIZE, m_ComboODSize);
	DDX_Text(pDX, IDC_EDIT_TEMP_MIN, m_StrTempMin);
	DDX_Text(pDX, IDC_EDIT_TEMP_MAX, m_StrTempMax);
	DDX_Text(pDX, IDC_EDIT_GAS_MAX, m_StrGasMax);
	DDX_Text(pDX, IDC_EDIT_FLAR_MAX, m_StrFlarMax);
	DDX_Text(pDX, IDC_EDIT_SIMG_INT, m_StrSImageInt);
	DDX_Control(pDX, IDC_STATIC_SETTINGS, m_GRBoxSettings);
	DDX_Text(pDX, IDC_EDIT_SMAIL_INT, m_StrSMailInt);
	DDX_Text(pDX, IDC_EDIT_SMAIL_MAX, m_SMailMax);
	DDX_Text(pDX, IDC_EDIT_GMAIL_ACCOUNT, m_StrGmailAccount);
	DDX_Text(pDX, IDC_EDIT_GMAIL_PWD, m_StrGmailPWD);
	DDX_Control(pDX, IDC_BTN_INIT_SETTINGS, m_BtnInitSettings);
	DDX_Control(pDX, IDC_BTN_SAVE_SETTINGS, m_BtnApplySettings);
	DDX_Text(pDX, IDC_EDIT_COUNT_ODETECT, m_StrODCountMax);
	DDX_Text(pDX, IDC_EDIT_COUNT_SENSOR_DETECT, m_StrSensorAlarmCountMax);
	DDX_Text(pDX, IDC_EDIT_TEMP_INFO, m_StrTemperature);
	DDX_Text(pDX, IDC_EDIT_GAS_INFO, m_StrGas);
	DDX_Text(pDX, IDC_EDIT_FLAR_INFO, m_StrFlar);
}

BEGIN_MESSAGE_MAP(CVidCapDlg, CDialog)
        ON_MESSAGE(WM_GRAPHNOTIFY, OnGraphMessage)
        ON_WM_PAINT()
        ON_WM_QUERYDRAGICON()
        //}}AFX_MSG_MAP
        ON_BN_CLICKED(IDC_ENUMADAPTORS_BUTTON, &CVidCapDlg::OnBnClickedEnumadaptorsButton)
        ON_BN_CLICKED(IDC_RUN_BUTTON, &CVidCapDlg::OnBnClickedRunButton)
        ON_WM_TIMER()
        ON_WM_CLOSE()
        ON_WM_WINDOWPOSCHANGED()        
        ON_STN_DBLCLK(IDC_CAPIMG_STATIC, &CVidCapDlg::OnStnDblclickCapimgStatic)
        ON_BN_CLICKED(IDC_BACKGROUND_RADIO, &CVidCapDlg::OnBnClickedBackgroundRadio)
        ON_BN_CLICKED(IDC_TRACKOBJECTS_RADIO, &CVidCapDlg::OnBnClickedTrackobjectsRadio)
	ON_BN_CLICKED(IDC_BTN_START_SECURE, &CVidCapDlg::OnStartSecureClicked)
	ON_BN_CLICKED(IDC_BTN_INIT_SETTINGS, &CVidCapDlg::OnBnClickedBtnInitSettings)
	ON_BN_CLICKED(IDC_BTN_SAVE_SETTINGS, &CVidCapDlg::OnBnClickedBtnSaveSettings)
END_MESSAGE_MAP()


// CVidCapDlg message handlers
LRESULT CVidCapDlg::OnGraphMessage(WPARAM wParam, LPARAM lParam)
{
        HRESULT hr = vcHandleGraphEvent();
        TRACE(L" WM_GRAPH 0x%X\n", hr);
        return 0;
}

BOOL CVidCapDlg::OnInitDialog()
{
        CDialog::OnInitDialog();

        // Set the icon for this dialog.  The framework does this automatically
        //  when the application's main window is not a dialog
        SetIcon(m_hIcon, TRUE);			// Set big icon
        SetIcon(m_hIcon, FALSE);		// Set small icon

        // TODO: Add extra initialization here

        // Initialize COM
        if (FAILED(CoInitializeEx(NULL, COINIT_APARTMENTTHREADED))) {
                MessageBox(L"CoInitialize Failed!", L"COM error");
                m_RunButton.EnableWindow(FALSE);
                return TRUE;
        }

        if (GetEncoderClsid(L"image/jpeg", &pBmpEncoder) < 0) {
                MessageBox(L"Failed to get image/jpeg encoder", L"warning");
        }

#if TRUE //UI picture control�� ���зΰ� �׷��ش�.
		HBITMAP hBitmap=NULL;
		hBitmap=(HBITMAP)LoadImage(0,L"slogo.bmp",IMAGE_BITMAP,0,0,LR_LOADFROMFILE);
		m_PicSLogo.SetBitmap(hBitmap);
		if(!hBitmap)::CloseHandle(hBitmap);
#endif
				
		//######################################################
		Delete_All_ss_status();//���ο� ������ ���� ss_status table�� ��� ������ ����.
		InsertBasic_ss_status(); //���ο� ������ ���� ss_status table �� ���ο� ������ �߰�.
		if (!Read_vsettings(*gDvsettings)) { //vsettings �� ���� ���������� �ε�. 
			// �������ε忡 ������ ���. �Ʒ��� ���� ����Ʈ �������� ����.
			gDvsettings->SetToDefault();
			InsertBasic_vsettings(gDvsettings);
		}
		//######################################################
#if false //for test.
		//reset global ss_status.
		gDss_status->sSRun = _T("1");
		gDss_status->sVRun = _T("2");
		gDss_status->sODetect = _T("3");
		Update_ss_status(gDss_status);
		CD_s_sensors* oSS = new CD_s_sensors();
		Read_s_sensors(*oSS);
		delete oSS; oSS = NULL;
#endif
		//#### ���Ȱ��� �������·� ����.
		m_StartSecure.EnableWindow(FALSE);

		//#### ��ü�ν� ������ ���� �޺��ڽ� �ʱ�ȭ.
		m_ComboODSize.AddString(_T("0:Tiny"));
		m_ComboODSize.AddString(_T("1:Medium"));
		m_ComboODSize.AddString(_T("2:Big"));
		m_ComboODSize.SetCurSel(0);//0�� �׸���.

		//##### ������ �׸�鿡 �� ����.###########
		SetVSettingValues(gDvsettings);
		//Insert_DetectImageInfo(g_strImagePath, _T("12222.99.99.99.jpg")); for test..
	

        return TRUE;  // return TRUE  unless you set the focus to a control
}

// MFC Sleep�� ������ ������ ����Լ�. 
// DWORD dwMillisecond : �и���
void CVidCapDlg::Wait(DWORD dwMillisecond)
{
    MSG msg;
    DWORD t0, t1, diff; 

    t0 = GetTickCount();
    while(TRUE){
        t1 = GetTickCount();

        if( t0 <= t1 )				diff = t1 - t0;
        else						diff = 0xFFFFFFFF - t0 + t1;
		
        if( diff > dwMillisecond )  break;
		
        while( PeekMessage(&msg, NULL, 0, 0, PM_REMOVE) ){
			TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
        Sleep(1);
    }
}

void CVidCapDlg::OnClose()
{
        // TODO: Add your message handler code here and/or call default
        KillTimer(m_nTimer);
        vcStopCaptureVideo();
        CoUninitialize();

        if (m_Background != 0)
                delete[] m_Background;

        CDialog::OnClose();
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CVidCapDlg::OnPaint()
{
        if (IsIconic()) {
                CPaintDC dc(this); // device context for painting

                SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

                // Center icon in client rectangle
                int cxIcon = GetSystemMetrics(SM_CXICON);
                int cyIcon = GetSystemMetrics(SM_CYICON);
                CRect rect;
                GetClientRect(&rect);
                int x = (rect.Width() - cxIcon + 1) / 2;
                int y = (rect.Height() - cyIcon + 1) / 2;

                // Draw the icon
                dc.DrawIcon(x, y, m_hIcon);
        } else {

                CDialog::OnPaint();
        }
}

void CVidCapDlg::EnableSettings()
{
	GetDlgItem(IDC_EDIT_SINT)->EnableWindow(TRUE);
	GetDlgItem(IDC_COMBO_ODSIZE)->EnableWindow(TRUE);
	GetDlgItem(IDC_EDIT_TEMP_MIN)->EnableWindow(TRUE);
	GetDlgItem(IDC_EDIT_TEMP_MAX)->EnableWindow(TRUE);
	GetDlgItem(IDC_EDIT_GAS_MAX)->EnableWindow(TRUE);
	GetDlgItem(IDC_EDIT_FLAR_MAX)->EnableWindow(TRUE);
	GetDlgItem(IDC_EDIT_SIMG_INT)->EnableWindow(TRUE);
	GetDlgItem(IDC_EDIT_SMAIL_INT)->EnableWindow(TRUE);
	GetDlgItem(IDC_EDIT_SMAIL_MAX)->EnableWindow(TRUE);
	GetDlgItem(IDC_EDIT_GMAIL_ACCOUNT)->EnableWindow(TRUE);
	GetDlgItem(IDC_EDIT_GMAIL_PWD)->EnableWindow(TRUE);
	GetDlgItem(IDC_EDIT_COUNT_ODETECT)->EnableWindow(TRUE);
	GetDlgItem(IDC_EDIT_COUNT_SENSOR_DETECT)->EnableWindow(TRUE);
	m_BtnInitSettings.EnableWindow(TRUE);
	m_BtnApplySettings.EnableWindow(TRUE);
}

void CVidCapDlg::DisableSettings()
{
	GetDlgItem(IDC_EDIT_SINT)->EnableWindow(FALSE);
	GetDlgItem(IDC_COMBO_ODSIZE)->EnableWindow(FALSE);
	GetDlgItem(IDC_EDIT_TEMP_MIN)->EnableWindow(FALSE);
	GetDlgItem(IDC_EDIT_TEMP_MAX)->EnableWindow(FALSE);
	GetDlgItem(IDC_EDIT_GAS_MAX)->EnableWindow(FALSE);
	GetDlgItem(IDC_EDIT_FLAR_MAX)->EnableWindow(FALSE);
	GetDlgItem(IDC_EDIT_SIMG_INT)->EnableWindow(FALSE);
	GetDlgItem(IDC_EDIT_SMAIL_INT)->EnableWindow(FALSE);
	GetDlgItem(IDC_EDIT_SMAIL_MAX)->EnableWindow(FALSE);
	GetDlgItem(IDC_EDIT_GMAIL_ACCOUNT)->EnableWindow(FALSE);
	GetDlgItem(IDC_EDIT_GMAIL_PWD)->EnableWindow(FALSE);
	GetDlgItem(IDC_EDIT_COUNT_ODETECT)->EnableWindow(FALSE);
	GetDlgItem(IDC_EDIT_COUNT_SENSOR_DETECT)->EnableWindow(FALSE);
	m_BtnInitSettings.EnableWindow(FALSE);
	m_BtnApplySettings.EnableWindow(FALSE);
}

int CVidCapDlg::GetEncoderClsid(const WCHAR* format, CLSID* pClsid)
{
        UINT  num = 0;        // number of image encoders
        UINT  size = 0;         // size of the image encoder array in bytes

        Gdiplus::ImageCodecInfo* pImageCodecInfo = NULL;

        Gdiplus::GetImageEncodersSize(&num, &size);
        if (size == 0)
                return -1;  // Failure

        pImageCodecInfo = (Gdiplus::ImageCodecInfo*)(malloc(size));
        if (pImageCodecInfo == NULL)
                return -1;  // Failure

        Gdiplus::GetImageEncoders(num, size, pImageCodecInfo);

        for (UINT j = 0; j < num; ++j) {
                if (wcscmp(pImageCodecInfo[j].MimeType, format) == 0) {
                        *pClsid = pImageCodecInfo[j].Clsid;
                        free(pImageCodecInfo);
                        return j;  // Success
                }
        }

        free(pImageCodecInfo);
        return -1;  // Failure
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CVidCapDlg::OnQueryDragIcon()
{
        return static_cast<HCURSOR>(m_hIcon);
}


void CVidCapDlg::OnBnClickedEnumadaptorsButton()
{
        //Enum adaptors
        vcGetCaptureDevices(m_AdapterCombo);
}

//##############################################
void CVidCapDlg::OnBnClickedRunButton()
{
        UpdateData();

        HRESULT hr;
        if (m_nTimer == 0) {
                //Run capture
                hr = vcCaptureVideo(m_hWnd, m_PrvStatic.m_hWnd, m_AdapterCombo.GetCurSel() + 1);
                if (hr != S_OK) {
                        vcStopCaptureVideo();
                        return;
                }
                
                CString str;
                str.Format(L"Video output: %dx%d %dbpp", sgGetDataWidth(), sgGetDataHeight(), 8 * sgGetDataChannels()); 
                m_VideoFormat.SetWindowTextW(str);

                //Setup Timer
                if (sgGetDataWidth() == m_Width && sgGetDataHeight() == m_Height && sgGetDataChannels() == m_Channels) {
                        m_nTimer = SetTimer(1, m_TimerInterval, 0);
                        m_FpsRate = 0.0;
                        m_Ms = 0;
                        m_MsPerFrame = 0;
                        m_FramesProcessed = 0;
                        m_TotalFrames = 1000 / m_TimerInterval;
                        if (m_TotalFrames == 0)
                                m_TotalFrames = 1;
                }

                m_RunButton.SetWindowTextW(L"Stop");
				//##########################################
				m_StartSecure.SetWindowTextW(L"���Ȱ��� ����");
				m_StartSecure.EnableWindow(TRUE);// enable ���Ȱ��ý��� button.
				gDss_status->sSRun = _T("1");						
				gDss_status->sVRun = _T("0");						
				gDss_status->sODetect = _T("0"); //## ħ�Կ��� �ʱ�ȭ.
				Update_ss_status(gDss_status);// ss_status ������Ʈ.
				m_NowAlarmCount = 0; //
				m_NowSenAlarmCount = 0;
				m_StartAlarmCount = _ttoi(gDvsettings->sODCnt);// N�� ħ���� Ž���ؾ� ħ������ ����.
				m_StartSenAlarmCount = _ttoi(gDvsettings->sSDCnt); //N�� �������� ������ �Ѿ�� ������� ����.
				m_TemMin = _ttoi(gDvsettings->sSTemMin);
				m_TemMax = _ttoi(gDvsettings->sSTemMax);
				m_GasMax = _ttoi(gDvsettings->sSGasMax);
				m_FlarMax = _ttoi(gDvsettings->sSFlarMax);
				m_ImgSaveInt = _ttoi(gDvsettings->sSISaveInt); //�̹��� �����ֱ�.

				//### check.
				if (m_GasMax < 10) { m_GasMax = 10; }
				if (m_FlarMax < 10) { m_FlarMax = 10; }
				if (m_ImgSaveInt < 5) { m_ImgSaveInt = 5; }

                m_Background = new unsigned int[sgGetDataWidth() * sgGetDataHeight() * sgGetDataChannels()];
                memset(m_Background, 0, sgGetDataWidth() * sgGetDataHeight() * sgGetDataChannels() * sizeof(unsigned int));
                m_BackgroundFramesNumber = 0;
                m_CaptureState = HALT;

                m_MotionDetector.init(sgGetDataWidth(), sgGetDataHeight(), m_Zoom);
                m_Blobs.init((unsigned int)(m_Zoom * (float)sgGetDataWidth()), (unsigned int)(m_Zoom * (float)sgGetDataHeight()));

				DisableSettings();//### ������ �Է� disable.

        } else {
                //Close Timer
                KillTimer(m_nTimer);
                m_nTimer = 0;
                m_RunButton.SetWindowTextW(L"Run");
				//##########################################
				m_StartSecure.SetWindowTextW(L"���Ȱ��� ����");
				m_StartSecure.EnableWindow(FALSE);// enable ���Ȱ��ý��� Disable.
				// ���Ȱ��ø� �����Ѵ�. (�������� ���ư��� �ʴ� �����)
				gDss_status->sSRun = _T("0");
				gDss_status->sVRun = _T("0");
				gDss_status->sODetect = _T("0"); //## ħ�Կ��� �ʱ�ȭ.
				Update_ss_status(gDss_status);// ss_status ������Ʈ.
				m_NowAlarmCount = 0;
				m_NowSenAlarmCount = 0;


                m_VideoFormat.SetWindowTextW(L"Video output");
                //Close Capture
                vcStopCaptureVideo();

                delete[] m_Background;
                m_Background = 0;
                m_CaptureState = HALT;

				EnableSettings();//### ������ �Է� enable.
        }
}

//##############################################
void CVidCapDlg::OnTimer(UINT_PTR nIDEvent)
{
        // TODO: Add your message handler code here and/or call default
        SYSTEMTIME SystemTime;
        GetLocalTime(&SystemTime);
        TRACE(L" %d:%d:%d\n", SystemTime.wHour, SystemTime.wMinute, SystemTime.wSecond);

        unsigned char* pData = sgGrabData();
        if (pData != 0) 
                DrawData(pData);                

        CDialog::OnTimer(nIDEvent);
}

//##############################################
void CVidCapDlg::OnWindowPosChanged(WINDOWPOS* lpwndpos)
{
        CDialog::OnWindowPosChanged(lpwndpos);

        // TODO: Add your message handler code here
        vcChangePreviewState(!IsIconic());
}

//##############################################
void CVidCapDlg::DrawData(unsigned char *pData)
{                
        Gdiplus::Bitmap* pBitmap = sgGetBitmap();
        if (pBitmap == 0)
                return;        

#if false
		//���콺 ����Ŭ���� �̹��� �����.(jpg�� ����)
		//���콺 ����Ŭ���� m_TakeSnapshot �� true �� �ȴ�. 
        if (m_TakeSnapshot == true) {
                m_TakeSnapshot = false;
                sndPlaySound(L"snap.wav", SND_ASYNC);
                if (pBmpEncoder != GUID_NULL) {
                        wchar_t FileName[_MAX_PATH] = L"";
                        for (unsigned int i = 1; i < 0xFFFFFFFF; i++) {
                                swprintf_s(FileName, _MAX_PATH, L"Snapshot %04d.jpg", i);
                                FILE* fp = _wfopen(FileName, L"rb");
                                if (fp == 0) {
                                        pBitmap->Save(FileName, &pBmpEncoder);
                                        break;
                                }
                                else
                                        fclose(fp);

                        }                        
                }
        }
#else
        if (m_TakeSnapshot == true) {
            m_TakeSnapshot = false;
			//���� �̹��� ����. ########
			if (pBmpEncoder != GUID_NULL) {
				CString strImgName = L"";
				CTime curTime;
				curTime = CTime::GetCurrentTime();

				wchar_t FileName[_MAX_PATH] = L"";
				for (unsigned int i = 1; i < 0xFFFFFFFF; i++) {
					strImgName.Format(L"dimg%04d_%02d_%02d_%02d_%02d_%02d_%04d.jpg",
						curTime.GetYear(), curTime.GetMonth(),curTime.GetDay(),
						curTime.GetHour(), curTime.GetMinute(), curTime.GetSecond(), i);

					swprintf_s(FileName, _MAX_PATH, L"%s\\%s", g_strImagePath, strImgName);
					FILE* fp = _wfopen(FileName, L"rb"); //�׽�Ʈ�� �����. ��������Ȯ��.
					if (fp == 0) {//������ �̹� �������� ������.. �̹��������� ����� ����Ϸ� �� DB�� �������.
							pBitmap->Save(FileName, &pBmpEncoder);
							//## DB�� �̹��� ��� ����.
							Insert_DetectImageInfo(g_strImagePath, strImgName);
							break;
					} else {
						fclose(fp);//������ �̹������ϸ� ������ �ݴ´�.												
					}

				}                        
			}
        }
#endif

        RECT r;
        m_CapImgStatic.GetClientRect(&r);
        Gdiplus::Graphics g(m_CapImgStatic.GetDC()->m_hDC);
        

        //draw on Bitmap
        Gdiplus::Graphics mem_g(pBitmap);
        Gdiplus::SolidBrush GreenBrush(Gdiplus::Color(100, 0, 255, 0));
        Gdiplus::Pen RedPen(Gdiplus::Color(255, 255, 0, 0), 2.5f);

        switch (m_CaptureState) {
        default:
        case HALT:
                break;

        case GET_BACKGROUND:                
                m_BackgroundFramesNumber++;
                for (long i = 0; i < sgGetBufferSize(); i++)
                        m_Background[i] += (unsigned int)pData[i];
                break;

        case BLOB_TRACKER:
			{
				BOOL b_odetect = FALSE;
				BOOL b_sensorDetect = FALSE;
				int oDsize = m_ComboODSize.GetCurSel();
				unsigned int nBlobs = 0;
				tic();
				//mark motion regions
				const vec2Dc* mvec = m_MotionDetector.detect(pData);
				m_Ms += (unsigned int)toc();
				//DDDDD(L"[mark motion regions] heitht:%u, width%u\r\n", mvec->height(), mvec->width());

				for (unsigned int y = 0; y < mvec->height(); y++) {
					for (unsigned int x = 0; x < mvec->width(); x++) {
						if ((*mvec)(y, x) == 1)
							mem_g.FillEllipse(&GreenBrush, x * 8, y * 8, 8, 8);
					}
				}

				tic();
				//estimate blobs                
				m_Blobs.find_blobs(*mvec, m_MinBlobElements);
				m_Ms += (unsigned int)toc();
				nBlobs = m_Blobs.get_blobs_number();

				if ( nBlobs > 0) {
					// m_Blobs.get_blobs_number() �� ���� ��� �� ��ü�� �����Ǿ����� Ȯ�ΰ���.
					//DDDDD(L" found %u blobs\n", m_Blobs.get_blobs_number()); 
					m_Blobs.find_bounding_boxes();
					for (unsigned int i = 0; i < nBlobs; i++) {
						const RECT r = m_Blobs.get_blob(i)->bounding_box;
						INT x = int((float)r.left / m_Zoom);
						INT y = int((float)r.top / m_Zoom);
						INT width = int((float)r.right / m_Zoom) - x;
						INT height = int((float)r.bottom / m_Zoom) - y;
						mem_g.DrawRectangle(&RedPen, x, y, width, height);
						//###########################################
						// �̹�ü�� ������ �׸��� rectangle�� �ִ� width x height �� 640x480��.
						// DB�� vsettings ��(0, 1, 2)�� ���� width���� �о� ħ�Կ��θ� �Ǻ�.
						// 0:  rectangle �� width�� 60���� ���� ���.
						// 1: rectangle�� width�� 61~ 200
						// 2: rectangle�� width�� 200�̻��� ���
						// run�ϰ� ���ý����� ������ ��츸..
						if (_T("1") == gDss_status->sSRun && _T("1") == gDss_status->sVRun) {
							//DDDDD(L"[blobs.drawrect] x:%u, y:%u, width:%u, height:%u, odsize:%d, nBlobs:%u\r\n",	x, y, width, height, oDsize,nBlobs);
							if (0 == oDsize) {
								if (width >= 5) { b_odetect = TRUE; }
							} else if (1 == oDsize) {
								if (width >= 200) { b_odetect = TRUE; }
							} else {
								if (width >= 400) { b_odetect = TRUE; }
							}
						}//if (_T("1") == gDss_status->sSRun && _T("1") == gDss_status->sVRun) {
					}//end of for()

					m_Blobs.delete_blobs();
				}//if ( nBlobs > 0) {
				else {
					//## ��ü�� �����ߴٰ� �ٽ� �ʱ�ȭ������ ���ƿ� ��� ��ü�ν�ī��Ʈ�� 0���� ����.
					//## ī�޶� ������� �ϹǷ�, �����̳�.. ������ � ���� ��ü�ν�(ħ��)���� �����Ǵ� ��찡 �ִ�
					//�̷��� ��츦 �ϴ� ��ü���� Ƚ�� ī��Ʈ�� ���� ���´�. 
					m_NowAlarmCount = 0; 
				}

				if (_T("1") == gDss_status->sSRun && _T("1") == gDss_status->sVRun) {
					//############ ������ ���� #################
					if (Read_s_sensors(*gDs_sensors)) {
						int nTem = _ttoi(gDs_sensors->sSTem);
						int nGas = _ttoi(gDs_sensors->sSGas);
						int nFlar = _ttoi(gDs_sensors->sSFlar);

						
						//nTem = 0.56 * (nTem - (int)32);//ȭ���� ������. C#���� ���ְ�����.
						//UI�� �µ�.����.ȭ������ ���÷���.
						//m_StrTemperature.Format(_T("%d"), nTem);
						m_StrTemperature = gDs_sensors->sSTem;
						m_StrGas = gDs_sensors->sSGas;
						m_StrFlar = gDs_sensors->sSFlar;

						if ((nTem < m_TemMin || nTem > m_TemMax) || nGas > m_GasMax || nFlar > m_FlarMax) {
							if (m_StartSenAlarmCount < m_NowSenAlarmCount++) {
								b_sensorDetect = TRUE;
							}
						} else {
							m_NowSenAlarmCount = 0; //������ֿ��� ��� ���. �ʱ�ȭ�Ѵ�. 
						}
					}

					DDDDD(L"[res] odetect:%d, osensor:%d, odcount:(%d/%d), oscount:(%d/%d)\r\n",
						b_odetect, b_sensorDetect, m_NowAlarmCount, m_StartAlarmCount, m_NowSenAlarmCount, m_StartSenAlarmCount);

					if (b_odetect || b_sensorDetect) {
						if ((m_StartAlarmCount < m_NowAlarmCount++ || b_sensorDetect) && _T("0") == gDss_status->sODetect) {
							gDss_status->sODetect = _T("1");// ħ�԰��� �Ͽ���.(�Ǵ� ������ ���� )
							//ss_status table ���� ########
							Update_ss_status(gDss_status);
						}
						//��ü�� ������ ��� ������ �̹����� �����ϵ���.
						if ((b_odetect && 0 == m_NowAlarmCount % m_ImgSaveInt) || (b_sensorDetect && 0 == m_NowSenAlarmCount % m_ImgSaveInt)) {
							m_TakeSnapshot = TRUE; // ��� �׸��带 �׸��� ���� ȭ���� ������ ���ֵ��� �Ѵ�. 
						}

					}//end of if (b_odetect) {
				}

				m_FramesProcessed++;
				if (m_FramesProcessed >= m_TotalFrames) {
					m_MsPerFrame = m_Ms / m_TotalFrames;
					m_FpsRate = 1000.0 / double(m_MsPerFrame);
					m_FramesProcessed = 0;
					m_Ms = 0;
				}

				m_TrackingStatus.Format(L"Object tracking: %dms (%.2lffps)", m_MsPerFrame, m_FpsRate);

#if false
				//#### read sensor data and refresh ui ##############
				if (_T("1") == gDss_status->sSRun && _T("1") == gDss_status->sVRun) {
					if (Read_s_sensors(*gDs_sensors)) {
						m_StrTemperature = gDs_sensors->sSTem;
						m_StrGas = gDs_sensors->sSGas;
						m_StrFlar = gDs_sensors->sSFlar;
					}
				}
#endif
				break;
			}//case BLOB_TRACKER:
        }                


        g.DrawImage(pBitmap, Gdiplus::Rect(0, 0, r.right, r.bottom));

        UpdateData(FALSE);
}

//##############################################
void CVidCapDlg::OnStnDblclickCapimgStatic()
{
        //m_TakeSnapshot = true;
}

//##############################################
void CVidCapDlg::OnBnClickedBackgroundRadio()
{
        m_CaptureState = GET_BACKGROUND;

        m_BackgroundFramesNumber = 0;
        if (m_Background != 0)
                memset(m_Background, 0, sgGetDataWidth() * sgGetDataHeight() * sgGetDataChannels() * sizeof(unsigned int));
}

//##############################################
void CVidCapDlg::OnBnClickedTrackobjectsRadio()
{
        if (m_CaptureState == HALT)
                return;

        m_CaptureState = BLOB_TRACKER;

        if (m_Background != 0) {
                unsigned char* tmp = new unsigned char[sgGetDataWidth() * sgGetDataHeight() * sgGetDataChannels()];
                for (unsigned int i = 0; i < sgGetDataWidth() * sgGetDataHeight() * sgGetDataChannels(); i++)                 
                        tmp[i] = unsigned char(m_Background[i] / m_BackgroundFramesNumber);                
                
                m_MotionDetector.set_background(tmp);

                Gdiplus::Bitmap pBitmap(sgGetDataWidth(), sgGetDataHeight(), 
                                        sgGetDataChannels() * sgGetDataWidth(), PixelFormat24bppRGB, tmp);
                pBitmap.Save(L"background.jpg", &pBmpEncoder);

                delete[] tmp;
        }   
}

//##############################################
// ȯ�漳������ �׸��� �����Ѵ�.
void CVidCapDlg::SetVSettingValues(const CD_vsettings *pVS) 
{
	CString strDtSize = NULL;
	strDtSize = pVS->sDTSize;
	if (_T("0") == strDtSize) { m_ComboODSize.SetCurSel(0); }
	else if (_T("1") == strDtSize) {m_ComboODSize.SetCurSel(1);}
	else {m_ComboODSize.SetCurSel(2);}

	m_StrSensingInterval = pVS->sSInt;// ������ �д� �ֱ�.
	m_StrTempMin = pVS->sSTemMin; // �µ� ���� �Ǵܰ�.
	m_StrTempMax = pVS->sSTemMax;//�µ� ���� �Ǵܰ�.
	m_StrGasMax = pVS->sSGasMax; //���� ���� �̻��Ǵܰ�
	m_StrFlarMax = pVS->sSFlarMax; //ȭ�� �̻��Ǵܰ�
	m_StrODCountMax = pVS->sODCnt;//��ü�ν��� ����̻� �����Ǿ�� ������� ���⳪.
	m_StrSensorAlarmCountMax = pVS->sSDCnt; //���� �̻��� ����̻� �����Ǿ�� ������� ���⳪.
	m_StrSImageInt = pVS->sSISaveInt; //�̹��� ���� �ֱ�.(ħ�Խ�, ������ �̻� ������ )
	m_StrSMailInt = pVS->sSmailInt; // ���� �۽� �ֱ�.
	m_SMailMax = pVS->sSmailMax; // ���� ���� �۽� �ִ�ȸ��. (�ֱ⸶�� 1ȸ�� ����� ��������..)
	m_StrGmailAccount = pVS->sSmailAddr; // gmail account
	m_StrGmailPWD = pVS->sSMailPwd; //gmail password.
	
	UpdateData(FALSE);//UI�� ����.
}


//##############################################
// ���Ȱ��ý��� ��ư ���� ���.
void CVidCapDlg::OnStartSecureClicked() {
	if (_T("1") == gDss_status->sSRun) {
		m_NowAlarmCount = 0;
		m_NowSenAlarmCount = 0;
		gDss_status->sODetect = _T("0"); //## ħ�Կ��� �ʱ�ȭ.

		if (_T("0") == gDss_status->sVRun) { //���Ȱ��� ���� �ƴ϶��..		
			TCHAR buffer[MAX_PATH];
			CString sLattPath;
			gDss_status->sVRun = _T("1"); //## ���� ���Ȱ�����.			
			Update_ss_status(gDss_status);// ss_status ������Ʈ.
			m_StartSecure.SetWindowTextW(L"���Ȱ��� ����");
			
			Wait(1000);
#if true			

			GetCurrentDirectory(MAX_PATH, buffer); // buffer�� ���� �������α׷��� ����Ǵ� ��θ� ���´�.
			sLattPath.Format(L"%s\\LatteTest\\LatteTest.exe", buffer);
			//SHELLEXECUTEINFO m_sLatteTestInfo;
			ZeroMemory(&m_sLatteTestInfo, sizeof(SHELLEXECUTEINFO));
			m_sLatteTestInfo.cbSize  = sizeof(SHELLEXECUTEINFO);
			m_sLatteTestInfo.lpFile  = sLattPath; //���μ��� ���
			m_sLatteTestInfo.lpParameters =  _T("-a");
			m_sLatteTestInfo.nShow  = SW_HIDE; //����� �Ⱥ��̰�
			m_sLatteTestInfo.lpVerb  = _T("runas"); //������ ���� ����
			m_sLatteTestInfo.fMask  = SEE_MASK_NOCLOSEPROCESS; //���μ��� �ڵ鰪 ���

			if (ShellExecuteEx(&m_sLatteTestInfo)) {
				DDDDD(_T("[success] execute LatteTest.\r\n"));
			} else {
				DDDDD(_T("[failed] failed to execute LatteTest.\r\n"));
			}
#else
			C#���α׷� ����( ������ ����, �������� ó����). "-a"�ɼ�: â�� ����.
			::ShellExecute(NULL, _T("open"), _T("LatteTest"), _T("-a"), NULL, SW_HIDE);
#endif
		} else { //���Ȱ������̶��.
			gDss_status->sVRun = _T("0"); //## ���� ���Ȱ��� ����.			
			Update_ss_status(gDss_status);// ss_status ������Ʈ.

			m_StartSecure.EnableWindow(FALSE);// enable ���Ȱ��ý��� button.
			Wait(5000); // C#���α׷��� ������ ���� 5�� ��ٸ���.
			// �̷��� ��ٸ� ���� C# ���α׷�(LatteTest)�� sVrun ���� DB���� �о� 0���� Ȯ���� ������ ����
			// �Ʒ����� ���࿡ ������� �ʾ��� ��츦 ����Ͽ� �������� ����.
			//if (GetProcess(_T("LatteTest") )) {
				//DDDDD(_T("[FIND] find LatteTest exsistance.\r\n"));
				if (TerminateProcess(m_sLatteTestInfo.hProcess, 0)) {
					DDDDD(_T("[success] terminate LatteTest. by Force.\r\n"));
				} else {
					DDDDD(_T("[failed] failed to terminate LatteTest. by Force.\r\n"));
				}
			//}

			m_StartSecure.EnableWindow(TRUE);// enable ���Ȱ��ý��� button.
			m_StartSecure.SetWindowTextW(L"���Ȱ��� ����");
			
		}		

	} else {
		AfxMessageBox(_T("You Have to Click RUN button First!"));
	}

}

//##############################################
// �����ʱ�ȭ ��ư �������.
void CVidCapDlg::OnBnClickedBtnInitSettings() {
	gDvsettings->SetToDefault();//������ �ʱ�ȭ.
	InsertBasic_vsettings(gDvsettings);	// vsettings table�� ����.
	SetVSettingValues(gDvsettings); //UI�� ����.
}

//##############################################
// ���������ư���� ���.
void CVidCapDlg::OnBnClickedBtnSaveSettings() {
	UpdateData(TRUE);//UI�� ���� ���� �о��.
	int nDtSize = m_ComboODSize.GetCurSel();
	
	if (0 == nDtSize) { gDvsettings->sDTSize = _T("0"); }
	else if ( 1 == nDtSize) { gDvsettings->sDTSize = _T("1"); }
	else { gDvsettings->sDTSize = _T("2"); }

	 gDvsettings->sSInt = m_StrSensingInterval;// ������ �д� �ֱ�.
	 gDvsettings->sSTemMin = m_StrTempMin; // �µ� ���� �Ǵܰ�.
	 gDvsettings->sSTemMax = m_StrTempMax;//�µ� ���� �Ǵܰ�.
	 gDvsettings->sSGasMax = m_StrGasMax; //���� ���� �̻��Ǵܰ�
	 gDvsettings->sSFlarMax = m_StrFlarMax; //ȭ�� �̻��Ǵܰ�
	 gDvsettings->sODCnt = m_StrODCountMax;//��ü�ν��� ����̻� �����Ǿ�� ������� ���⳪.
	 gDvsettings->sSDCnt = m_StrSensorAlarmCountMax; //���� �̻��� ����̻� �����Ǿ�� ������� ���⳪.
	 gDvsettings->sSISaveInt = m_StrSImageInt; //�̹��� ���� �ֱ�.(ħ�Խ�, ������ �̻� ������ )
	 gDvsettings->sSmailInt = m_StrSMailInt; // ���� �۽� �ֱ�.
	 gDvsettings->sSmailMax = m_SMailMax; // ���� ���� �۽� �ִ�ȸ��. (�ֱ⸶�� 1ȸ�� ����� ��������..)
	 gDvsettings->sSmailAddr = m_StrGmailAccount; // gmail account
	 gDvsettings->sSMailPwd = m_StrGmailPWD; //gmail password.

	 Update_vsettings(gDvsettings); // update vsettings table values.
}

/*
        @brief          ���μ��� ���� ���θ� �����Ѵ�.
        @param       [IN] name: ex) Run.exe
        @return       ���μ��� ���� FALSE
                          ���μ��� ���� TRUE
*/
BOOL GetProcess(CString name)
{
		//CStringW myName(name);
        HANDLE hProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS,0); //���� ��� ���μ������� ���¸� ����
        PROCESSENTRY32 pe32;
 
         if(hProcessSnap == INVALID_HANDLE_VALUE){                
			 exit(EXIT_FAILURE);
         }

         pe32.dwSize = sizeof(PROCESSENTRY32);

         if( !Process32First(hProcessSnap, &pe32)) {
                CloseHandle(hProcessSnap);
                exit(EXIT_FAILURE);  
         }		 	 

         do{
			      if(name.Compare(pe32.szExeFile) == 0)     //�������� ���μ����� �ִٸ� 
                  {
                           //MessageBox(pe32.szExeFile);
                           return TRUE; 
                  }
         }while(Process32Next(hProcessSnap,&pe32));
         return FALSE;
}