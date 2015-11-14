/*
	Copyright (c) 2013-2014 EasyDarwin.ORG.  All rights reserved.
	Github: https://github.com/EasyDarwin
	WEChat: EasyDarwin
	Website: http://www.EasyDarwin.org
*/
// EasyClientDlg.cpp : implementation file
//

#include "stdafx.h"
#include "EasyClient.h"
#include "EasyClientDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CEasyClientDlg dialog

CEasyClientDlg::CEasyClientDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CEasyClientDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);

	memset(&easyVideoPanelObj, 0x00, sizeof(EASY_VIDEO_PANEL_OBJ_T));
	m_pManager = NULL;
}

void CEasyClientDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CEasyClientDlg, CDialogEx)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_CBN_SELCHANGE(IDC_COMBO_SOURCE, &CEasyClientDlg::OnCbnSelchangeComboSource)
	ON_BN_CLICKED(IDC_BTN_CAPTURE, &CEasyClientDlg::OnBnClickedBtnCapture)
	ON_BN_CLICKED(IDC_BTN_PUSH, &CEasyClientDlg::OnBnClickedBtnPush)
	ON_BN_CLICKED(IDC_BTN_PLAY, &CEasyClientDlg::OnBnClickedBtnPlay)
	ON_MESSAGE(MSG_LOG, &CEasyClientDlg::OnLog)
	ON_WM_GETMINMAXINFO()

END_MESSAGE_MAP()


// CEasyClientDlg message handlers

BOOL CEasyClientDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	// TODO: Add extra initialization here

	easyVideoPanelObj.pDlgLocalPanel = new CDlgLocalPanel();
	easyVideoPanelObj.pDlgLocalPanel->Create(IDD_DIALOG_LOCAL_PANEL, this);
	easyVideoPanelObj.pDlgLocalPanel->ShowWindow(SW_SHOW);

	easyVideoPanelObj.pDlgRemotePanel = new CDlgRemotePanel();
	easyVideoPanelObj.pDlgRemotePanel->Create(IDD_DIALOG_REMOTE_PANEL, this);
	easyVideoPanelObj.pDlgRemotePanel->ShowWindow(SW_SHOW);

	m_pManager = CSourceManager::Instance();
	m_pManager->SetMainDlg(this);
	CWnd* pVideoCombo = GetDlgItem(IDC_COMBO_CAMERA) ;
	CWnd* pAudioCombo = GetDlgItem(IDC_COMBO_MIC) ;
	CEdit* pRtspURL = (CEdit*)GetDlgItem(IDC_EDIT_SREAM_URL);

	if (NULL != pRtspURL)
	{
		pRtspURL->SetWindowTextW(TEXT("rtsp://127.0.0.1:554/stream.sdp"));
	}
	CEdit* pIP = (CEdit*)GetDlgItem(IDC_EDIT_SERVER_IP);
	if (pIP)
	{
		pIP->SetWindowTextW(TEXT("easyclient.easydarwin.org"));
	}
	CEdit* pPort = (CEdit*)GetDlgItem(IDC_EDIT_SERVER_PORT);
	if (pPort)
	{
		pPort->SetWindowTextW(TEXT("554"));
	}
	CEdit* pName = (CEdit*)GetDlgItem(IDC_EDIT_PUSH_NAME);
	if (pName)
	{
		pName->SetWindowTextW(TEXT("stream.sdp"));
	}
	
	if (m_pManager)
	{
		m_pManager->EnumLocalAVDevInfo(pVideoCombo, pAudioCombo);
	}
	CComboBox* pSouceCombo = (CComboBox*)GetDlgItem(IDC_COMBO_SOURCE);
	if (pSouceCombo)
	{
		pSouceCombo->AddString(_T("本地音视频采集"));
		pSouceCombo->AddString(_T("网络RTSP流采集"));
	//	pSouceCombo->AddString(_T("网络Onvif流采集"));
		pSouceCombo->SetCurSel(0);
	}
	int nSel  = 	pSouceCombo->GetCurSel();
	if (nSel == 0)
	{
		pVideoCombo->EnableWindow(TRUE);
		pAudioCombo->EnableWindow(TRUE);
		pRtspURL->SetReadOnly(TRUE);
	} 
	else
	{
		pVideoCombo->EnableWindow(FALSE);

		pAudioCombo->EnableWindow(FALSE);
		pRtspURL->SetReadOnly(FALSE);
	}
	MoveWindow(0, 0, 926, 727);

	return TRUE;  // return TRUE  unless you set the focus to a control
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CEasyClientDlg::OnPaint()
{
	if (IsIconic())
	{
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
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CEasyClientDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

LRESULT CEasyClientDlg::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	if (/*WM_PAINT == message ||*/ WM_SIZE == message /*|| WM_MOVE == message*/)
	{
		UpdateComponents();
	}

	return CDialogEx::WindowProc(message, wParam, lParam);
}

//窗口挪动位置
void	CEasyClientDlg::UpdateComponents()
{

	CRect	rcClient;
	GetClientRect(&rcClient);
	if (rcClient.IsRectEmpty())		
		return;	
	
	CComboBox* pComboxMediaSource = (CComboBox*)GetDlgItem(IDC_COMBO_SOURCE);
	CComboBox* pVideoCombo = (CComboBox*)GetDlgItem(IDC_COMBO_CAMERA) ;
	CComboBox* pComboxAudioSource = (CComboBox*)GetDlgItem(IDC_COMBO_MIC) ;
	CEdit* pEdtRtspSource = (CEdit*)GetDlgItem(IDC_EDIT_SREAM_URL);



	CStatic* pStcServerIp = (CStatic*)GetDlgItem(IDC_STC_SERVER_IP);
	CStatic* pStcServerPort = (CStatic*)GetDlgItem(IDC_STC_SERVER_PORT);
	CStatic* pStcStreanName = (CStatic*)GetDlgItem(IDC_STC_STREAM_NAME);
	CStatic* pStcDebugInfo = (CStatic*)GetDlgItem(IDC_STC_DEBUG_INFO);
	CStatic* pStcVersionInfo = (CStatic*)GetDlgItem(IDC_STC_VERSION);

	
	
	CEdit* pIP = (CEdit*)GetDlgItem(IDC_EDIT_SERVER_IP);
	CEdit* pPort = (CEdit*)GetDlgItem(IDC_EDIT_SERVER_PORT);
	CEdit* pName = (CEdit*)GetDlgItem(IDC_EDIT_PUSH_NAME);

	CButton* pBtnCapture = (CButton*)GetDlgItem(IDC_BTN_CAPTURE);
	CButton* pBtnPush = (CButton*)GetDlgItem(IDC_BTN_PUSH);
	CButton* pBtnPlay = (CButton*)GetDlgItem(IDC_BTN_PLAY);

	CEdit* pEdtShowLog = (CEdit*)GetDlgItem(IDC_EDIT_SHOWLOG);


	CRect rcPosition;

	int nWidth = rcClient.Width();
	int nHeight = rcClient.Height();
	int nStartX = (nWidth-210*3-140*2)/2;


	rcPosition.SetRect(nStartX , nHeight-280+15, nStartX + 100, nHeight-280+15+24 );
	__MOVE_WINDOW(pStcServerIp, rcPosition);
	rcPosition.SetRect(nStartX + 100 + 5, nHeight-280+15, nStartX + 210, nHeight-280+15+24 );
	__MOVE_WINDOW(pIP, rcPosition);

	rcPosition.SetRect(nStartX +210 +140, nHeight-280+15, nStartX + 210+140+100, nHeight-280+15+24 );
	__MOVE_WINDOW(pStcServerPort, rcPosition);
	rcPosition.SetRect(nStartX +210 +140 + 90 +5, nHeight-280+15, nStartX +210+140 + 210, nHeight-280+15+24 );
	__MOVE_WINDOW(pPort, rcPosition);

	rcPosition.SetRect(nStartX + (210 +140)*2+40, nHeight-280+15, nStartX + (210 +140)*2+100, nHeight-280+15+24 );
	__MOVE_WINDOW(pStcStreanName, rcPosition);
	rcPosition.SetRect(nStartX + (210 +140)*2 + 90+5, nHeight-280+15, nStartX + (210 +140)*2 + 210, nHeight-280+15+24 );
	__MOVE_WINDOW(pName, rcPosition);

	int nBtnStartX =  (rcClient.Width()-74*3-200*2)/2;
	rcPosition.SetRect(nBtnStartX , nHeight-280+24+25, nBtnStartX + 74, nHeight-280+25+24+24 );
	__MOVE_WINDOW(pBtnCapture, rcPosition);

	rcPosition.SetRect(nBtnStartX+74+200 , nHeight-280+24+25, nBtnStartX+74+200 + 74, nHeight-280+25+24+24 );
	__MOVE_WINDOW(pBtnPush, rcPosition);
	
	rcPosition.SetRect(nBtnStartX+(74+200)*2 , nHeight-280+24+25, nBtnStartX+(74+200)*2  + 74, nHeight-280+25+24+24 );
	__MOVE_WINDOW(pBtnPlay, rcPosition);


	rcPosition.SetRect(10, nHeight-280+24+25+20, 100, nHeight-280+24+25+20+24 );
	__MOVE_WINDOW(pStcDebugInfo, rcPosition);
	
	rcPosition.SetRect(10 , nHeight-280+24+25+44,  nWidth-10, nHeight-30 );
	__MOVE_WINDOW(pEdtShowLog, rcPosition);

	
	rcPosition.SetRect(nWidth-420 , nHeight-25,  nWidth, nHeight );
	__MOVE_WINDOW(pStcVersionInfo, rcPosition);

	int iPanelWidth = ((rcClient.Width()) / 2);


	CRect	rcLocalPanel;
	rcLocalPanel.SetRect(rcClient.left, rcClient.top+65, rcClient.left+iPanelWidth, rcClient.bottom-280);
	__MOVE_WINDOW(easyVideoPanelObj.pDlgLocalPanel, rcLocalPanel);

	CRect	rcRemotePanel;
	rcRemotePanel.SetRect(rcLocalPanel.right, rcLocalPanel.top, rcClient.right, rcLocalPanel.bottom);
	__MOVE_WINDOW(easyVideoPanelObj.pDlgRemotePanel, rcRemotePanel);
	Invalidate();
}

void CEasyClientDlg::OnGetMinMaxInfo(MINMAXINFO* lpMMI)
{
	lpMMI->ptMinTrackSize.x = 926;//954, 780
	lpMMI->ptMinTrackSize.y = 727;

	CDialogEx::OnGetMinMaxInfo(lpMMI);
}

BOOL CEasyClientDlg::DestroyWindow()
{
// 	EasyPlayer_Release();

	if (m_pManager)
	{
		m_pManager->UnInstance();
		m_pManager = NULL;
	}
	__DESTROY_WINDOW(easyVideoPanelObj.pDlgLocalPanel);
	__DESTROY_WINDOW(easyVideoPanelObj.pDlgRemotePanel);

	return CDialogEx::DestroyWindow();
}


void CEasyClientDlg::OnCbnSelchangeComboSource()
{
	CComboBox* pComboxMediaSource = (CComboBox*)GetDlgItem(IDC_COMBO_SOURCE);
	CComboBox* pVideoCombo = (CComboBox*)GetDlgItem(IDC_COMBO_CAMERA) ;
	CComboBox* pComboxAudioSource = (CComboBox*)GetDlgItem(IDC_COMBO_MIC) ;
	CEdit* pEdtRtspSource = (CEdit*)GetDlgItem(IDC_EDIT_SREAM_URL);

	if (NULL == pComboxMediaSource)		return;

	int iCount = pComboxMediaSource->GetCount();
	int iSel = pComboxMediaSource->GetCurSel();
	if (iSel == 0)
	{
		pVideoCombo->EnableWindow(TRUE);
		pComboxAudioSource->EnableWindow(TRUE);
		pEdtRtspSource->SetReadOnly(TRUE);
	} 
	else
	{
		pVideoCombo->EnableWindow(FALSE);
		pComboxAudioSource->EnableWindow(FALSE);
		pEdtRtspSource->SetReadOnly(FALSE);
	}

// 	if (NULL != pComboxAudioSource)	pComboxAudioSource->ShowWindow(iSel == iCount-1?SW_HIDE:SW_SHOW);
// 	if (NULL != pEdtRtspSource)	pEdtRtspSource->ShowWindow(iSel == iCount-1?SW_SHOW:SW_HIDE);
// 
}


void CEasyClientDlg::OnBnClickedBtnCapture()
{
	if (m_pManager)
	{
		CWnd* pCapWnd = easyVideoPanelObj.pDlgLocalPanel->GetDlgVideoHwnd();
		BOOL bInCap = m_pManager->IsInCapture();
		CButton* pBtnStartCapture = (CButton*)GetDlgItem(IDC_BTN_CAPTURE) ;
		if (!bInCap)
		{
			CComboBox* pComboxMediaSource = (CComboBox*)GetDlgItem(IDC_COMBO_SOURCE);
			CComboBox* pVideoCombo = (CComboBox*)GetDlgItem(IDC_COMBO_CAMERA) ;
			CEdit* pEdtRtspSource = (CEdit*)GetDlgItem(IDC_EDIT_SREAM_URL);

			SOURCE_TYPE eType = (SOURCE_TYPE)pComboxMediaSource->GetCurSel();
			int nCamId = 0;
			char szURL[128] = {0,};
			CString strTemp = _T("");
			if (eType == SOURCE_LOCAL_CAMERA)
			{
				nCamId = pVideoCombo->GetCurSel();
				strTemp = _T("本地音视频采集");
			} 
			else
			{
				//Start
				wchar_t wszURL[128] = {0,};
				if (NULL != pEdtRtspSource)
					pEdtRtspSource->GetWindowTextW(wszURL, sizeof(wszURL));
				if (wcslen(wszURL) < 1)		return;

				CString strURL = wszURL;
				CString strRTSP = strURL.Mid(0,4);
				if (strRTSP!=_T("rtsp")&&strRTSP!=_T("RTSP"))
				{
					strURL = _T("rtsp://")+strURL;
				}
				
				__WCharToMByte(strURL, szURL, sizeof(szURL)/sizeof(szURL[0]));
				strTemp = _T("网络音视频流采集");

			}
			int nRet = m_pManager->StartCapture( eType,  nCamId, pCapWnd->GetSafeHwnd(), szURL);
			if (nRet>0)
			{
				strTemp +=_T("成功！"); 
			} 
			else
			{
				strTemp +=_T("失败！"); 
			}
			m_pManager->LogErr(strTemp);
			if (NULL != pBtnStartCapture)		pBtnStartCapture->SetWindowText(TEXT("Stop"));

		}
		else
		{
			m_pManager->StopCapture();
			if (NULL != pBtnStartCapture)		pBtnStartCapture->SetWindowText(TEXT("本地预览"));
			m_pManager->LogErr(_T("本地预览停止"));

			pCapWnd->Invalidate();	
		}
	}
}


void CEasyClientDlg::OnBnClickedBtnPush()
{
	if (m_pManager)
	{
		BOOL bInPush = m_pManager->IsInPushing();
		CEdit* pIP = (CEdit*)GetDlgItem(IDC_EDIT_SERVER_IP);
		CEdit* pPort = (CEdit*)GetDlgItem(IDC_EDIT_SERVER_PORT);
		CEdit* pName = (CEdit*)GetDlgItem(IDC_EDIT_PUSH_NAME);
		
		CButton* pBtnStartPush = (CButton*)GetDlgItem(IDC_BTN_PUSH) ;
		if (!bInPush)
		{
			char szIp[128] = {0,};
			char szPort[128] = {0,};
			char szName[128] = {0,};
			wchar_t wszIp[128] = {0,};
			wchar_t wszPort[128] = {0,};
			wchar_t wszName[128] = {0,};
			if (NULL != pIP)	
				pIP->GetWindowTextW(wszIp, sizeof(wszIp));
			if (wcslen(wszIp) < 1)		
				return;
			__WCharToMByte(wszIp, szIp, sizeof(szIp)/sizeof(szIp[0]));

			if (NULL != pPort)	
				pPort->GetWindowTextW(wszPort, sizeof(wszPort));
			if (wcslen(wszPort) < 1)		
				return;
			__WCharToMByte(wszPort, szPort, sizeof(szPort)/sizeof(szPort[0]));
			int nPort = atoi(szPort);

			if (NULL != pName)	
				pName->GetWindowTextW(wszName, sizeof(wszName));
			if (wcslen(wszName) < 1)		
				return;
			__WCharToMByte(wszName, szName, sizeof(szName)/sizeof(szName[0]));
			
			int nRet = m_pManager->StartPush(szIp , nPort,  szName);
			CString strMsg = _T("");
			if (nRet>=0)
			{
				strMsg.Format(_T("推送EasyDarwin服务器URL：rtsp://%s:%d/%s 成功！"), wszIp, nPort, wszName);
				if (NULL != pBtnStartPush)		pBtnStartPush->SetWindowText(TEXT("Stop"));
			} 
			else
			{
				strMsg.Format(_T("推送EasyDarwin服务器URL：rtsp://%s:%d/%s 失败！"), wszIp, nPort, wszName);

			}
			m_pManager->LogErr(strMsg);
	
		}
		else
		{
			m_pManager->LogErr(_T("停止推送！"));
			m_pManager->StopPush();
			if (NULL != pBtnStartPush)		pBtnStartPush->SetWindowText(TEXT("推送->"));
		}
	}

}

void CEasyClientDlg::OnBnClickedBtnPlay()
{
	if (m_pManager)
	{
		CWnd* pPlayWnd = easyVideoPanelObj.pDlgRemotePanel->GetDlgVideo();
		CEdit* pIP = (CEdit*)GetDlgItem(IDC_EDIT_SERVER_IP);
		CEdit* pPort = (CEdit*)GetDlgItem(IDC_EDIT_SERVER_PORT);
		CEdit* pName = (CEdit*)GetDlgItem(IDC_EDIT_PUSH_NAME);
		CWnd* pCapWnd = easyVideoPanelObj.pDlgLocalPanel->GetDlgVideoHwnd();
		BOOL bInPlay = m_pManager->IsInPlaying();
		CButton* pBtnStartPlay= (CButton*)GetDlgItem(IDC_BTN_PLAY) ;
		if (!bInPlay)
		{
			char szIp[128] = {0,};
			char szPort[128] = {0,};
			char szName[128] = {0,};
			wchar_t wszIp[128] = {0,};
			wchar_t wszPort[128] = {0,};
			wchar_t wszName[128] = {0,};
			if (NULL != pIP)	
				pIP->GetWindowTextW(wszIp, sizeof(wszIp));
			if (wcslen(wszIp) < 1)		
				return;
			__WCharToMByte(wszIp, szIp, sizeof(szIp)/sizeof(szIp[0]));

			if (NULL != pPort)	
				pPort->GetWindowTextW(wszPort, sizeof(wszPort));
			if (wcslen(wszPort) < 1)		
				return;
			__WCharToMByte(wszPort, szPort, sizeof(szPort)/sizeof(szPort[0]));
			int nPort = atoi(szPort);

			if (NULL != pName)	
				pName->GetWindowTextW(wszName, sizeof(wszName));
			if (wcslen(wszName) < 1)		
				return;
			__WCharToMByte(wszName, szName, sizeof(szName)/sizeof(szName[0]));

			char szURL[128]= {0,};
			sprintf(szURL, "rtsp://%s:%d/%s", szIp,  nPort, szName );
			int nRet = m_pManager->StartPlay(szURL, pPlayWnd->GetSafeHwnd());
			if (NULL != pBtnStartPlay)		pBtnStartPlay->SetWindowText(TEXT("Stop"));
			CString strMsg = _T("");
			if (nRet>0)
			{
				strMsg.Format(_T("直播预览URL：rtsp://%s:%d/%s 成功！"), wszIp, nPort, wszName);
			} 
			else
			{
				strMsg.Format(_T("直播预览URL：rtsp://%s:%d/%s 失败！"), wszIp, nPort, wszName);

			}
			m_pManager->LogErr(strMsg);
		}
		else
		{
			m_pManager->StopPlay();
			if (NULL != pBtnStartPlay)		pBtnStartPlay->SetWindowText(TEXT("直播预览"));
			pPlayWnd->Invalidate();	
			m_pManager->LogErr(_T("停止直播"));
		}
	}
}

LRESULT CEasyClientDlg::OnLog(WPARAM wParam, LPARAM lParam)
{
	CEdit* pLog = (CEdit*)GetDlgItem(IDC_EDIT_SHOWLOG);
	if (pLog)
	{
		CString strLog = (TCHAR*)lParam;
		CString strTime = _T("");
		CTime CurrentTime=CTime::GetCurrentTime(); 
		strTime.Format(_T("%04d/%02d/%02d %02d:%02d:%02d   "),CurrentTime.GetYear(),CurrentTime.GetMonth(),
			CurrentTime.GetDay(),CurrentTime.GetHour(),  CurrentTime.GetMinute(),
			CurrentTime.GetSecond());
		strLog = strTime + strLog + _T("\r\n");
		int nLength  =  pLog->SendMessage(WM_GETTEXTLENGTH);  
		pLog->SetSel(nLength,  nLength);  
		pLog->ReplaceSel(strLog); 

	}

	return 0;
}

