// VidCapDlg.h : header file
//

#pragma once
#include "afxwin.h"

#include "lib\motiondetector.h"
#include "lib\imageblobs.h"

// CVidCapDlg dialog
class CVidCapDlg : public CDialog
{
// Construction
public:
        CVidCapDlg(CWnd* pParent = NULL);	// standard constructor        

// Dialog Data
        enum { IDD = IDD_VIDCAP_DIALOG };

protected:
        virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support


// Implementation
protected:
        HICON m_hIcon;

        // Generated message map functions
        virtual BOOL OnInitDialog();
        afx_msg void OnPaint();
        afx_msg HCURSOR OnQueryDragIcon();
        DECLARE_MESSAGE_MAP()

private:
        enum CaptureState {HALT, GET_BACKGROUND, BLOB_TRACKER} m_CaptureState;
        unsigned int* m_Background;
        unsigned int m_BackgroundFramesNumber;

        float m_Zoom;
        unsigned int m_MinBlobElements;
        MotionDetector m_MotionDetector;
        ImageBlobs m_Blobs;

        unsigned int m_Width;
        unsigned int m_Height;
        unsigned int m_Channels;

        UINT_PTR m_nTimer;
        double m_FpsRate;
        unsigned int m_MsPerFrame;
        unsigned int m_TotalFrames;
        unsigned int m_FramesProcessed;
        unsigned int m_Ms;

		int m_NowAlarmCount;
		int m_StartAlarmCount;//		

		int m_NowSenAlarmCount;
		int m_StartSenAlarmCount;
		int m_TemMin;
		int m_TemMax;
		int m_GasMax;
		int m_FlarMax;

		int m_ImgSaveInt;


        CStatic m_PrvStatic;
        CStatic m_CapImgStatic;
        CComboBox m_AdapterCombo;
        UINT m_TimerInterval;
        CButton m_RunButton;
        CStatic m_VideoFormat;                
        CString m_TrackingStatus;
        bool m_TakeSnapshot;
        SHELLEXECUTEINFO m_sLatteTestInfo;

        CLSID pBmpEncoder;
        int GetEncoderClsid(const WCHAR* format, CLSID* pClsid);
        
        void DrawData(unsigned char *pData);        

		BOOL GetProcess(CString name);
		void Wait(DWORD dwMillisecond);
		void EnableSettings();
		void DisableSettings();
		void SetVSettingValues(const CD_vsettings* pVS);

        afx_msg LRESULT OnGraphMessage(WPARAM wParam, LPARAM lParam);
        afx_msg void OnBnClickedEnumadaptorsButton();
        afx_msg void OnBnClickedRunButton();
        afx_msg void OnTimer(UINT_PTR nIDEvent);
        afx_msg void OnClose();
        afx_msg void OnWindowPosChanged(WINDOWPOS* lpwndpos);
        afx_msg void OnBnClickedFdetectCheck();       
        afx_msg void OnStnDblclickCapimgStatic();
        afx_msg void OnBnClickedBackgroundRadio();
        afx_msg void OnBnClickedTrackobjectsRadio();

		// //대학로고 bitmap image
		CStatic m_PicSLogo;
public:
	afx_msg void OnStartSecureClicked();
	CButton m_StartSecure;
	CString m_StrSensingInterval;
	CComboBox m_ComboODSize;
	CString m_StrTempMin;
	CString m_StrTempMax;
	CString m_StrGasMax;
	CString m_StrFlarMax;
	CString m_StrSImageInt;
	CStatic m_GRBoxSettings;
	CString m_StrSMailInt;
	CString m_SMailMax;
	CString m_StrGmailAccount;
	CString m_StrGmailPWD;
	CButton m_BtnInitSettings;
	CButton m_BtnApplySettings;
	afx_msg void OnBnClickedBtnInitSettings();
	afx_msg void OnBnClickedBtnSaveSettings();
	CString m_StrODCountMax;
	CString m_StrSensorAlarmCountMax;
	CString m_StrTemperature;
	CString m_StrGas;
	CString m_StrFlar;
};
