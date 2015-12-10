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

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CEasyClientDlg dialog

CEasyClientDlg::CEasyClientDlg(CWnd* pParent /*=NULL*/)
	: CEasySkinManager(CEasyClientDlg::IDD, pParent, en_Wnd_Normal)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);

	m_pManager = NULL;
	m_pVideoWindow = NULL;
}

void CEasyClientDlg::DoDataExchange(CDataExchange* pDX)
{
	CEasySkinManager::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_BTN_CAPTURE, m_btnLocalView);
	DDX_Control(pDX, IDC_BTN_PUSH, m_btnPush);
	DDX_Control(pDX, IDC_BTN_PLAY, m_btnLiveView);
	DDX_Control(pDX, IDC_SYSLINK_EASYDARWIN, m_linkEasyDarwinWeb);
	DDX_Control(pDX, IDC_SYSLINK_EASYCLIENT_GITHUB, m_linkEasyClientGethub);
	DDX_Control(pDX, IDC_EDIT_VIDOE_WIDTH, m_edtVdieoWidth);
	DDX_Control(pDX, IDC_EDIT_VIDEO_HEIGHT, m_edtVideoHeight);
	DDX_Control(pDX, IDC_EDIT_VIDEO_FPS, m_edtFPS);
	DDX_Control(pDX, IDC_EDIT_VIDEO_BITRATE, m_edtVideoBitrate);
	DDX_Control(pDX, IDC_EDIT_PUSH_BUFFER, m_edtPushBuffer);
}

BEGIN_MESSAGE_MAP(CEasyClientDlg, CEasySkinManager)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_CBN_SELCHANGE(IDC_COMBO_SOURCE, &CEasyClientDlg::OnCbnSelchangeComboSource)
	ON_BN_CLICKED(IDC_BTN_CAPTURE, &CEasyClientDlg::OnBnClickedBtnCapture)
	ON_BN_CLICKED(IDC_BTN_PUSH, &CEasyClientDlg::OnBnClickedBtnPush)
	ON_BN_CLICKED(IDC_BTN_PLAY, &CEasyClientDlg::OnBnClickedBtnPlay)
	ON_MESSAGE(MSG_LOG, &CEasyClientDlg::OnLog)
	ON_WM_GETMINMAXINFO()

	ON_WM_SIZE()
	ON_WM_CTLCOLOR()
	ON_NOTIFY(NM_CLICK, IDC_SYSLINK_EASYDARWIN, &CEasyClientDlg::OnNMClickSyslinkEasydarwin)
	ON_NOTIFY(NM_CLICK, IDC_SYSLINK_EASYCLIENT_GITHUB, &CEasyClientDlg::OnNMClickSyslinkEasyclientGithub)
	ON_WM_ERASEBKGND()
	ON_CBN_SELCHANGE(IDC_COMBO_WND_MODE, &CEasyClientDlg::OnCbnSelchangeComboWndMode)
END_MESSAGE_MAP()


// CEasyClientDlg message handlers

BOOL CEasyClientDlg::OnInitDialog()
{
	CEasySkinManager::OnInitDialog();

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	this->SetWindowText(_T("EasyClient"));

	//更新皮肤
	UpdataResource();

	CComboBox* pComboxWndMode = (CComboBox*)GetDlgItem(IDC_COMBO_WND_MODE);
	if (pComboxWndMode)
	{
		pComboxWndMode->AddString(_T("四画面"));
		pComboxWndMode->AddString(_T("八画面"));
		pComboxWndMode->AddString(_T("九画面"));
		pComboxWndMode->AddString(_T("16画面"));
		pComboxWndMode->SetCurSel(0);
	}

	//创建视频窗口组
	CreateVideoComponents();

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

	m_edtVdieoWidth.SetWindowText(_T("640"));
	m_edtVideoHeight.SetWindowText(_T("480"));
	m_edtFPS.SetWindowText(_T("25"));
	m_edtVideoBitrate.SetWindowText(_T("2048"));
	m_edtPushBuffer.SetWindowText(_T("1024"));
	
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
// 	int nSel  = 	pSouceCombo->GetCurSel();
// 	if (nSel == 0)
// 	{
// 		pVideoCombo->ShowWindow(SW_SHOW);
// 		pAudioCombo->ShowWindow(SW_SHOW);
// 		pRtspURL->ShowWindow(SW_HIDE);
// 		m_edtVdieoWidth.ShowWindow(SW_SHOW);
// 		m_edtVideoHeight.ShowWindow(SW_SHOW);
// 		m_edtFPS.ShowWindow(SW_SHOW);
// 		m_edtVideoBitrate.ShowWindow(SW_SHOW);
// 	} 
// 	else
// 	{
// 		pVideoCombo->ShowWindow(SW_HIDE);
// 		pAudioCombo->ShowWindow(SW_HIDE);
// 		pRtspURL->ShowWindow(SW_SHOW);
// 		m_edtVdieoWidth.ShowWindow(SW_HIDE);
// 		m_edtVideoHeight.ShowWindow(SW_HIDE);
// 		m_edtFPS.ShowWindow(SW_HIDE);
// 		m_edtVideoBitrate.ShowWindow(SW_HIDE);
// 	}

	MoveWindow(0, 0, 1024,768 );//926, 727

	return TRUE;  // return TRUE  unless you set the focus to a control
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CEasyClientDlg::OnPaint()
{
	CEasySkinManager::OnPaint();
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CEasyClientDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

LRESULT CEasyClientDlg::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{

	return CEasySkinManager::WindowProc(message, wParam, lParam);
}

//窗口挪动位置
void	CEasyClientDlg::UpdateComponents()
{
	//移动准备
	HDWP hDwp=BeginDeferWindowPos(64);
	UINT uFlags=SWP_NOACTIVATE|SWP_NOCOPYBITS|SWP_NOZORDER;

	CRect	rcClient;
	GetClientRect(&rcClient);
	if (rcClient.IsRectEmpty())		
		return;	
	
	CEdit* pIP = (CEdit*)GetDlgItem(IDC_EDIT_SERVER_IP);
	CEdit* pPort = (CEdit*)GetDlgItem(IDC_EDIT_SERVER_PORT);
	CEdit* pName = (CEdit*)GetDlgItem(IDC_EDIT_PUSH_NAME);

	CButton* pBtnCapture = (CButton*)GetDlgItem(IDC_BTN_CAPTURE);
	CButton* pBtnPush = (CButton*)GetDlgItem(IDC_BTN_PUSH);
	CButton* pBtnPlay = (CButton*)GetDlgItem(IDC_BTN_PLAY);
	CEdit* pEdtShowLog = (CEdit*)GetDlgItem(IDC_EDIT_SHOWLOG);

	CComboBox* pComboxMediaSource = (CComboBox*)GetDlgItem(IDC_COMBO_SOURCE);
	CComboBox* pVideoCombo = (CComboBox*)GetDlgItem(IDC_COMBO_CAMERA) ;
	CComboBox* pComboxAudioSource = (CComboBox*)GetDlgItem(IDC_COMBO_MIC) ;
	CEdit* pEdtRtspSource = (CEdit*)GetDlgItem(IDC_EDIT_SREAM_URL);

	CComboBox* pComboxWndMode = (CComboBox*)GetDlgItem(IDC_COMBO_WND_MODE);
	
	CRect rcPosition;
	int nWidth = rcClient.Width();
	int nHeight = rcClient.Height();	
	
	rcPosition.SetRect(87, 39, 87 + 80, 58 );
	__MOVE_WINDOW(pComboxWndMode, rcPosition);
	if (pComboxWndMode)
	{
		pComboxWndMode->SetFocus();
	}

	int nStartX =420;
	rcPosition.SetRect(nStartX, 39, nStartX + 150, 58 );
	__MOVE_WINDOW(pIP, rcPosition);
	if (pIP)
	{
		pIP->SetFocus();
	}
	rcPosition.SetRect(nStartX+200, 39, nStartX+200+60, 58 );
	__MOVE_WINDOW(pPort, rcPosition);
	if (pPort)
	{
		pPort->SetFocus();
	}
	rcPosition.SetRect(nStartX+330, 39, nStartX+330 + 80, 58 );
	__MOVE_WINDOW(pName, rcPosition);
	if (pName)
	{
		pName->SetFocus();
	}

	rcPosition.SetRect(nStartX+510, 39, nStartX+510 + 70, 58 );
	if (m_edtPushBuffer.GetSafeHwnd())
	{
			m_edtPushBuffer.MoveWindow(rcPosition);
			m_edtPushBuffer.SetFocus();
	}

	//按钮位置
	int nBtnStartX =  (rcClient.Width()-62*3-212*2)/2;
	DeferWindowPos(hDwp,m_btnLocalView,NULL,nBtnStartX,nHeight-250+24+25,0,0,uFlags|SWP_NOSIZE);
	DeferWindowPos(hDwp,m_btnPush,NULL,nBtnStartX+62+212, nHeight-250+24+25,0,0,uFlags|SWP_NOSIZE);
	DeferWindowPos(hDwp,m_btnLiveView,NULL,nBtnStartX+(62+212)*2,nHeight-250+24+25,0,0,uFlags|SWP_NOSIZE);

	DeferWindowPos(hDwp,m_linkEasyDarwinWeb,NULL,nWidth-250,nHeight-23,0,0,uFlags|SWP_NOSIZE);
	if (m_linkEasyDarwinWeb.m_hWnd)
	{
		m_linkEasyDarwinWeb.SetFocus();
	}
	DeferWindowPos(hDwp,m_linkEasyClientGethub,NULL,nWidth-120,nHeight-23,0,0,uFlags|SWP_NOSIZE);
	if (m_linkEasyClientGethub.m_hWnd)
	{
		m_linkEasyClientGethub.SetFocus();
	}

	rcPosition.SetRect(10 , nHeight-280+24+25+44,  nWidth-10, nHeight-30 );

	//大小变动控件
	if (pEdtShowLog)
	{
		DeferWindowPos(hDwp, pEdtShowLog->GetSafeHwnd(), NULL, 10, nHeight-250+24+25+44, nWidth-20,127, uFlags);
		pEdtShowLog->SetFocus();
	}

		//结束调整
	LockWindowUpdate();
	EndDeferWindowPos(hDwp);
	UnlockWindowUpdate();

	int iPanelWidth = rcClient.Width();

	CRect	rcPanel;
	rcPanel.SetRect(rcClient.left+2, rcClient.top+65, rcClient.left+iPanelWidth-4, rcClient.bottom-180);//320
// 	__MOVE_WINDOW(easyVideoPanelObj.pDlgLocalPanel, rcLocalPanel);
// 	if (easyVideoPanelObj.pDlgLocalPanel)
// 	{
// 		easyVideoPanelObj.pDlgLocalPanel->Invalidate(FALSE);
// 	}
// 
// 	CRect	rcRemotePanel;
// 	rcRemotePanel.SetRect(rcLocalPanel.right+1, rcLocalPanel.top, rcClient.right-3, rcLocalPanel.bottom);
// 	__MOVE_WINDOW(easyVideoPanelObj.pDlgRemotePanel, rcRemotePanel);
// 	if (easyVideoPanelObj.pDlgRemotePanel)
// 	{
// 		easyVideoPanelObj.pDlgRemotePanel->Invalidate(FALSE);
// 	}
	//Update Video Wnd
	UpdateVideoComponents(&rcPanel);

}

void CEasyClientDlg::OnGetMinMaxInfo(MINMAXINFO* lpMMI)
{
	lpMMI->ptMinTrackSize.x = 1024;//926;//954, 780
	lpMMI->ptMinTrackSize.y = 768;//727;

	CEasySkinManager::OnGetMinMaxInfo(lpMMI);
}

BOOL CEasyClientDlg::DestroyWindow()
{
// 	EasyPlayer_Release();

	if (m_pManager)
	{
		m_pManager->UnInstance();
		m_pManager = NULL;
	}

	//Delete Video Wnd
	DeleteVideoComponents();

	return CEasySkinManager::DestroyWindow();
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
		pVideoCombo->ShowWindow(SW_SHOW);
		pComboxAudioSource->ShowWindow(SW_SHOW);
		pEdtRtspSource->ShowWindow(SW_HIDE);
		m_edtVdieoWidth.ShowWindow(SW_SHOW);
		m_edtVideoHeight.ShowWindow(SW_SHOW);
		m_edtFPS.ShowWindow(SW_SHOW);
		m_edtVideoBitrate.ShowWindow(SW_SHOW);
	} 
	else
	{
		pVideoCombo->ShowWindow(SW_HIDE);
		pComboxAudioSource->ShowWindow(SW_HIDE);
		pEdtRtspSource->ShowWindow(SW_SHOW);
		m_edtVdieoWidth.ShowWindow(SW_HIDE);
		m_edtVideoHeight.ShowWindow(SW_HIDE);
		m_edtFPS.ShowWindow(SW_HIDE);
		m_edtVideoBitrate.ShowWindow(SW_HIDE);
	}
	Invalidate();
}

void CEasyClientDlg::OnBnClickedBtnCapture()
{
	if (m_pManager)
	{
		CWnd* pCapWnd = NULL;//easyVideoPanelObj.pDlgLocalPanel->GetDlgVideo();
		BOOL bInCap = m_pManager->IsInCapture();
		CButton* pBtnStartCapture = (CButton*)GetDlgItem(IDC_BTN_CAPTURE) ;
		if (!bInCap)
		{
			CComboBox* pComboxMediaSource = (CComboBox*)GetDlgItem(IDC_COMBO_SOURCE);
			CComboBox* pVideoCombo = (CComboBox*)GetDlgItem(IDC_COMBO_CAMERA) ;
			CComboBox* pAudioCombo = (CComboBox*)GetDlgItem(IDC_COMBO_MIC) ;
			CEdit* pEdtRtspSource = (CEdit*)GetDlgItem(IDC_EDIT_SREAM_URL);

			SOURCE_TYPE eType = (SOURCE_TYPE)pComboxMediaSource->GetCurSel();
			int nCamId = 0;
			int nAudioId = 0;
			char szURL[128] = {0,};
			CString strTemp = _T("");
			int nWidth = 640;
			int nHeight = 480;
			int nFps = 25;
			int nBitrate = 2048;

			if (eType == SOURCE_LOCAL_CAMERA)
			{
				nCamId = pVideoCombo->GetCurSel();
				nAudioId = pAudioCombo->GetCurSel();
				strTemp = _T("本地音视频采集");

				//视频参数设置
				// 			char szIp[128] = {0,};
				char szWidth[128] = {0,};
				wchar_t wszWidth[128] = {0,};
				char szHeight[128] = {0,};
				wchar_t wszHeight[128] = {0,};
				char szFPS[128] = {0,};
				wchar_t wszFPS[128] = {0,};
				char szBitrate[128] = {0,};
				wchar_t wszBitrate[128] = {0,};

				m_edtVdieoWidth.GetWindowTextW(wszWidth, sizeof(wszWidth));
				if (wcslen(wszWidth) < 1)		
					return;
				__WCharToMByte(wszWidth, szWidth, sizeof(szWidth)/sizeof(szWidth[0]));
				nWidth = atoi(szWidth);

				m_edtVideoHeight.GetWindowTextW(wszHeight, sizeof(wszHeight));
				if (wcslen(wszHeight) < 1)		
					return;
				__WCharToMByte(wszHeight, szHeight, sizeof(szHeight)/sizeof(szHeight[0]));
				nHeight = atoi(szHeight);

				m_edtFPS.GetWindowTextW(wszFPS, sizeof(wszFPS));
				if (wcslen(wszFPS) < 1)		
					return;
				__WCharToMByte(wszFPS, szFPS, sizeof(szFPS)/sizeof(szFPS[0]));
				nFps = atoi(szFPS);

				m_edtVideoBitrate.GetWindowTextW(wszBitrate, sizeof(wszBitrate));
				if (wcslen(wszBitrate) < 1)		
					return;
				__WCharToMByte(wszBitrate, szBitrate, sizeof(szBitrate)/sizeof(szBitrate[0]));
				nBitrate = atoi(szBitrate);
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

			int nRet = m_pManager->StartCapture( eType,  nCamId, nAudioId, pCapWnd->GetSafeHwnd(), szURL, nWidth, nHeight, nFps,nBitrate );
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
			char szPushBufferSize[128]={0,};
			wchar_t wszIp[128] = {0,};
			wchar_t wszPort[128] = {0,};
			wchar_t wszName[128] = {0,};
			wchar_t wszPushBufferSize[128]={0,};

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

			m_edtPushBuffer.GetWindowTextW(wszPushBufferSize, sizeof(wszPushBufferSize));
			if (wcslen(wszPushBufferSize) < 1)		
				return;
			__WCharToMByte(wszPushBufferSize, szPushBufferSize, sizeof(szPushBufferSize)/sizeof(szPushBufferSize[0]));
			int nPushBufSize = atoi(szPushBufferSize);

			int nRet = m_pManager->StartPush(szIp , nPort,  szName, nPushBufSize);
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
		CWnd* pPlayWnd = NULL;//easyVideoPanelObj.pDlgRemotePanel->GetDlgVideo();
		CEdit* pIP = (CEdit*)GetDlgItem(IDC_EDIT_SERVER_IP);
		CEdit* pPort = (CEdit*)GetDlgItem(IDC_EDIT_SERVER_PORT);
		CEdit* pName = (CEdit*)GetDlgItem(IDC_EDIT_PUSH_NAME);
		CWnd* pCapWnd = NULL;//easyVideoPanelObj.pDlgLocalPanel->GetDlgVideo();
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
		pLog->SetFocus();
	}

	return 0;
}

// SkinUI 界面统一绘制函数
void CEasyClientDlg::DrawClientArea( CDC*pDC,int nWidth,int nHeight )
{
	CEasySkinManager::DrawClientArea(pDC,nWidth,nHeight);
	pDC->SetBkMode(TRANSPARENT);

	//绘制静态文本 （有没有更好的方法呢？？？--!）
	//左部分	
	CFont *pOldFont=pDC->SelectObject(&m_ftSaticDefault);

	CRect rcPosition;
	rcPosition.SetRect(20 , 39, 20 + 67, 58 );
	CString strWndMode = _T("窗口模式");
	pDC->DrawText(strWndMode,rcPosition,DT_CENTER| DT_VCENTER |DT_SINGLELINE|DT_END_ELLIPSIS);
	
	rcPosition.SetRect(187 , 39, 187 + 210, 58 );
	CString strServerIp = _T("EasyDarwin服务器推流地址参数设置: ");
	pDC->DrawText(strServerIp,rcPosition,DT_LEFT| DT_VCENTER |DT_SINGLELINE|DT_END_ELLIPSIS);

 	rcPosition.SetRect(400, 39, 400 + 30, 58 );
	CString strIp = _T("IP:");
 	pDC->DrawText(strIp,rcPosition,DT_LEFT| DT_VCENTER |DT_SINGLELINE|DT_END_ELLIPSIS);

 	rcPosition.SetRect(400+160, 39, 400 + 160+60, 58 );
	CString strPort = _T("Port:");
	pDC->DrawText(strPort,rcPosition,DT_CENTER| DT_VCENTER |DT_SINGLELINE|DT_END_ELLIPSIS);

 	rcPosition.SetRect(400+290, 39, 400 + 290+60, 58 );
	CString strStream = _T("Stream:");
	pDC->DrawText(strStream,rcPosition,DT_CENTER| DT_VCENTER |DT_SINGLELINE|DT_END_ELLIPSIS);

	//推送参数设置
 	rcPosition.SetRect(870, 39, 870+60, 58 );
	CString strPushParam = _T("推送缓存:");
	pDC->DrawText(strPushParam,rcPosition,DT_LEFT| DT_VCENTER |DT_SINGLELINE|DT_END_ELLIPSIS);


	//下部分
	CString strDebugInfo = _T("Trace:");
	rcPosition.SetRect(10, nHeight-250+24+25+20, 70, nHeight-250+24+25+20+24 );
	pDC->DrawText(strDebugInfo,rcPosition,DT_CENTER| DT_VCENTER |DT_SINGLELINE|DT_END_ELLIPSIS);

	CString strVersionInfo = EasyClent_VersionInfo;
	rcPosition.SetRect(nWidth-810 , nHeight-25,  nWidth, nHeight );
	pDC->DrawText(strVersionInfo,rcPosition,DT_CENTER| DT_VCENTER |DT_SINGLELINE|DT_END_ELLIPSIS);

	pDC->SelectObject(pOldFont);  
}

BOOL CEasyClientDlg::OnCommand(WPARAM wParam, LPARAM lParam)
{
	return CEasySkinManager::OnCommand(wParam, lParam);
}
void CEasyClientDlg::UpdataResource()
{
	//设置窗口可拖动
	SetExtrude(true);
	//显示Logo
	m_bShowLogo = true;

	HDC hParentDC = GetBackDC();

	CRect rcClient;
	GetClientRect(&rcClient);

	m_ftSaticDefault.CreateFont(18,0,0,0,FW_NORMAL,0,FALSE,0,0,0,0,0,0,TEXT("微软雅黑"));

// 	m_btnLocalView;
// 	m_btnPush;
// 	m_btnLiveView;
	//贴图
	m_btnLocalView.SetBackImage(TEXT("SkinUI\\图标\\按钮常规.png"),
		TEXT("SkinUI\\图标\\按钮选中.png"),TEXT("SkinUI\\图标\\按钮选中-2.png"),TEXT("SkinUI\\图标\\按钮常规.png"), &CRect(3,3,3,3));
	m_btnLocalView.SetButtonType(en_PushButton);//en_IconButton
	m_btnLocalView.SetParentBack(hParentDC);
	//m_btnLocalView.SetIconImage(TEXT("SkinUI\\Main\\Tools.png"));
	m_btnLocalView.SetWindowText(TEXT("本地预览"));
	m_btnLocalView.SetSize(62,33);//74,24

	m_btnPush.SetBackImage(TEXT("SkinUI\\图标\\按钮常规.png"),
		TEXT("SkinUI\\图标\\按钮选中.png"),TEXT("SkinUI\\图标\\按钮选中-2.png"),TEXT("SkinUI\\图标\\按钮常规.png"));
	m_btnPush.SetButtonType(en_PushButton);//en_IconButton
	m_btnPush.SetParentBack(hParentDC);
	//m_btnPush.SetIconImage(TEXT("SkinUI\\Main\\Tools.png"));
	m_btnPush.SetWindowText(TEXT("推送-->"));
	m_btnPush.SetSize(62,33);//74,24

	m_btnLiveView.SetBackImage(TEXT("SkinUI\\图标\\按钮常规.png"),
		TEXT("SkinUI\\图标\\按钮选中.png"),TEXT("SkinUI\\图标\\按钮选中-2.png"),TEXT("SkinUI\\图标\\按钮常规.png"));
	m_btnLiveView.SetButtonType(en_PushButton);//en_IconButton
	m_btnLiveView.SetParentBack(hParentDC);
	//m_btnLiveView.SetIconImage(TEXT("SkinUI\\Main\\Tools.png"));
	m_btnLiveView.SetWindowText(TEXT("直播预览"));
	m_btnLiveView.SetSize(62,33);//74,24
}


void CEasyClientDlg::OnSize(UINT nType, int cx, int cy)
{
	CEasySkinManager::OnSize(nType, cx, cy);

	UpdateComponents();
	if (m_pManager)
	{
		m_pManager->ResizeVideoWnd();
	}
}


HBRUSH CEasyClientDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CEasySkinManager::OnCtlColor(pDC, pWnd, nCtlColor);

	if(nCtlColor == CTLCOLOR_SCROLLBAR || //静态文本背景设置为透明
		nCtlColor == CTLCOLOR_STATIC )//||nCtlColor == CTLCOLOR_SCROLLBAR
	{	
		//设置透明背景
		pDC->SetTextColor(RGB(10, 15, 15)) ;
		pDC-> SetBkMode(TRANSPARENT); 
		return   (HBRUSH)GetStockObject(NULL_BRUSH); 
	}
	if(nCtlColor==CTLCOLOR_EDIT)// 对EDIT控件属性进行设定
	{
		//设置透明背景
		pDC->SetBkMode(TRANSPARENT);
		//pDC->SetBkColor(RGB(0,225,225)); //设定文本的背景色
		pDC->SetTextColor(RGB(0,0,255));   //设定文本颜色
	}
	
	return hbr;
}


void CEasyClientDlg::OnNMClickSyslinkEasydarwin(NMHDR *pNMHDR, LRESULT *pResult)
{
	PNMLINK pNMLink = (PNMLINK) pNMHDR;  
// 	if (wcscmp(pNMLink->item.szUrl, _T("http:\/\/")) == 0) 
// 	{ 
// 		// 主要执行语句 
// 		ShellExecuteW(NULL, L"open", pNMLink->item.szUrl, NULL, NULL, SW_SHOWNORMAL); 
// 	} 
	*pResult = 0;

	ShellExecute(NULL, NULL, _T("http://www.easydarwin.org/"), NULL,NULL, SW_SHOWNORMAL); 
}


void CEasyClientDlg::OnNMClickSyslinkEasyclientGithub(NMHDR *pNMHDR, LRESULT *pResult)
{
	*pResult = 0;
	ShellExecute(NULL, NULL, _T("https://github.com/EasyDarwin/EasyClient"), NULL,NULL, SW_SHOWNORMAL); 

}


BOOL CEasyClientDlg::OnEraseBkgnd(CDC* pDC)
{
	return CEasySkinManager::OnEraseBkgnd(pDC);
}

void CEasyClientDlg::CreateVideoComponents()
{
	if (NULL == m_pVideoWindow)
	{
		m_pVideoWindow = new VIDEO_NODE_T;
		m_pVideoWindow->fullscreen    = false;
		m_pVideoWindow->maximizedId	=	-1;
		m_pVideoWindow->selectedId	=	-1;
		m_pVideoWindow->channels		=	4;
		if (m_pVideoWindow->channels>MAX_VIDEO_WINDOW_NUM)	m_pVideoWindow->channels=MAX_VIDEO_WINDOW_NUM;
		m_pVideoWindow->pDlgVideo	=	new CDlgPanel[MAX_VIDEO_WINDOW_NUM];//gAppInfo.maxchannels
		for (int i=0; i<MAX_VIDEO_WINDOW_NUM; i++)
		{
			m_pVideoWindow->pDlgVideo[i].SetMainDlg(this, i);
			m_pVideoWindow->pDlgVideo[i].Create(IDD_DIALOG_PANEL, this);
			m_pVideoWindow->pDlgVideo[i].ShowWindow(SW_HIDE);
		}
	}
}
void CEasyClientDlg::UpdateVideoComponents(LPRECT lpRect)
{
		CRect rcClient;
	if (NULL == lpRect)
	{
		GetClientRect(&rcClient);
		lpRect = &rcClient;
	}

	if (NULL == m_pVideoWindow)		return;

	//CRect rcClient;
	rcClient.CopyRect(lpRect);

	CRect rcTmp;
	rcTmp.SetRect(rcClient.left, rcClient.top, rcClient.left+rcClient.Width()/2, rcClient.top+rcClient.Height()/2);

	//

	if (m_pVideoWindow->maximizedId==-1)
	{
		int nTimes = 2;
		int nLeft = lpRect->left;
		int nTop  = lpRect->top;

		for (int i=m_pVideoWindow->channels; i<MAX_VIDEO_WINDOW_NUM; i++)
		{
			if (m_pVideoWindow->pDlgVideo[i].IsWindowVisible())
				m_pVideoWindow->pDlgVideo[i].ShowWindow(SW_HIDE);
		}

		switch (m_pVideoWindow->channels)
		{
		case 4:
		case 9:
		case 16:
		case 25:
		case 36:
		case 64:
		default:
			{
				nTimes = 2;
				if (m_pVideoWindow->channels == 2)		nTimes	=	1;
				if (m_pVideoWindow->channels == 4)		nTimes	=	2;
				if (m_pVideoWindow->channels == 9)		nTimes	=	3;
				if (m_pVideoWindow->channels == 16)		nTimes	=	4;
				if (m_pVideoWindow->channels == 25)		nTimes	=	5;
				if (m_pVideoWindow->channels == 36)		nTimes	=	6;
				if (m_pVideoWindow->channels == 64)		nTimes	=	8;

				RECT rcTmp;
				SetRectEmpty(&rcTmp);

				int n = 0;//videoPatrol.patrolStartId;
				for (int i = 0; i < nTimes; i++)
				{
					for (int j = 0; j < nTimes; j ++)
					{
						//SetRect(&rcTmp, nLeft, nTop, nLeft + imgSize.cx / nTimes, nTop + imgSize.cy / nTimes);
						SetRect(&rcTmp, nLeft, nTop, nLeft + rcClient.Width() / nTimes, nTop + rcClient.Height() / nTimes);
						//CopyRect(&vidRenderHandle[n].drawvid.rect, &rcTmp);

						if (j+1==nTimes && rcTmp.right<rcClient.right)
						{
							rcTmp.right = rcClient.right;
						}
						if (i+1==nTimes && rcTmp.bottom<rcClient.bottom)
						{
							rcTmp.bottom = rcClient.bottom;
						}


						m_pVideoWindow->pDlgVideo[n].MoveWindow(&rcTmp);
						if (! m_pVideoWindow->pDlgVideo[n].IsWindowVisible())
							m_pVideoWindow->pDlgVideo[n].ShowWindow(SW_SHOW);


						n ++;

						nLeft += rcClient.Width() / nTimes;
					}
					nLeft = rcClient.left;
					nTop  += (rcClient.Height()) / nTimes;
				}
			}
			break;
		case 6:		//6·??á
			{
				int nWidth = rcClient.Width() / 3;
				int nHeight= rcClient.Height()/ 3;

				int nRight = 0;
				int nBottom= 0;
				if (rcClient.right > nWidth*3)	nRight = rcClient.Width()-nWidth*3;
				if (rcClient.bottom> nHeight*3)	nBottom= rcClient.Height()-nHeight*3;
			
				nLeft = rcClient.left;
				nTop  = rcClient.top+nHeight*2;
				for (int i=3; i<6; i++)
				{
					rcTmp.SetRect(nLeft, nTop, nLeft+nWidth, nTop+nHeight);
					if (i+1==6)			rcTmp.right += nRight;
					if (nBottom > 0)	rcTmp.bottom += nBottom;
					m_pVideoWindow->pDlgVideo[i].MoveWindow(&rcTmp);
					if (! m_pVideoWindow->pDlgVideo[i].IsWindowVisible())
						m_pVideoWindow->pDlgVideo[i].ShowWindow(SW_SHOW);
				
					nLeft += nWidth;
				}
				nLeft -= nWidth;
				nTop  = rcClient.top;
				for (int i=1; i<3; i++)
				{
					rcTmp.SetRect(nLeft, nTop, nLeft+nWidth, nTop+nHeight);
					if (nRight>0)	rcTmp.right += nRight;
					m_pVideoWindow->pDlgVideo[i].MoveWindow(&rcTmp);
					if (! m_pVideoWindow->pDlgVideo[i].IsWindowVisible())
						m_pVideoWindow->pDlgVideo[i].ShowWindow(SW_SHOW);
					nTop += nHeight;
				}
			
				rcTmp.SetRect(rcClient.left, rcClient.top, rcTmp.left, rcTmp.bottom);
				m_pVideoWindow->pDlgVideo[0].MoveWindow(&rcTmp);
				if (! m_pVideoWindow->pDlgVideo[0].IsWindowVisible())
					m_pVideoWindow->pDlgVideo[0].ShowWindow(SW_SHOW);
			}
			break;
		case 8:		//8分屏
			{

				int nWidth = rcClient.Width() / 4;
				int nHeight= rcClient.Height()/ 4;

				int nRight = 0;
				int nBottom= 0;
				if (rcClient.right > nWidth*4)	nRight = rcClient.Width()-nWidth*4;
				if (rcClient.bottom> nHeight*4)	nBottom= rcClient.Height()-nHeight*4;

				nLeft = rcClient.left;
				nTop  = rcClient.top+nHeight*3;
				for (int i=4; i<8; i++)
				{
					rcTmp.SetRect(nLeft, nTop, nLeft+nWidth, nTop+nHeight);
					if (i+1==8)			rcTmp.right += nRight;
					if (nBottom > 0)	rcTmp.bottom += nBottom;
					m_pVideoWindow->pDlgVideo[i].MoveWindow(&rcTmp);
					if (! m_pVideoWindow->pDlgVideo[i].IsWindowVisible())
							m_pVideoWindow->pDlgVideo[i].ShowWindow(SW_SHOW);

					nLeft += nWidth;
				}
				nLeft -= nWidth;
				nTop  = rcClient.top;
				for (int i=1; i<4; i++)
				{
					rcTmp.SetRect(nLeft, nTop, nLeft+nWidth, nTop+nHeight);
					if (nRight>0)	rcTmp.right += nRight;
					m_pVideoWindow->pDlgVideo[i].MoveWindow(&rcTmp);
					if (! m_pVideoWindow->pDlgVideo[i].IsWindowVisible())
						m_pVideoWindow->pDlgVideo[i].ShowWindow(SW_SHOW);
					nTop += nHeight;
				}

				rcTmp.SetRect(rcClient.left, rcClient.top, rcTmp.left, rcTmp.bottom);
				m_pVideoWindow->pDlgVideo[0].MoveWindow(&rcTmp);
				if (! m_pVideoWindow->pDlgVideo[0].IsWindowVisible())
					m_pVideoWindow->pDlgVideo[0].ShowWindow(SW_SHOW);

			}
			break;
		}

		for (int vid=0; vid<MAX_VIDEO_WINDOW_NUM; vid++)
		{
			//m_pVideoWindow->pDlgVideo[vid].SetSelectedChannel(m_pVideoWindow->selectedId==vid);
		}
	}
	else
	{
		for (int i=0; i<MAX_VIDEO_WINDOW_NUM; i++)
		{
			if (m_pVideoWindow->pDlgVideo[i].IsWindowVisible() && i!=m_pVideoWindow->maximizedId)
			{
				m_pVideoWindow->pDlgVideo[i].ShowWindow(SW_HIDE);
			}
		}
		rcTmp.SetRect(lpRect->left, lpRect->top, lpRect->right, lpRect->bottom);
		m_pVideoWindow->pDlgVideo[m_pVideoWindow->maximizedId].MoveWindow(&rcTmp);
		m_pVideoWindow->pDlgVideo[m_pVideoWindow->maximizedId].ShowWindow(SW_SHOW);
	}
}
void CEasyClientDlg::DeleteVideoComponents()
{
	if (NULL != m_pVideoWindow)
	{
		if (NULL != m_pVideoWindow->pDlgVideo)
		{
			for (int i=0; i<MAX_VIDEO_WINDOW_NUM; i++)
			{
				m_pVideoWindow->pDlgVideo[i].DestroyWindow();
			}
			delete []m_pVideoWindow->pDlgVideo;
			m_pVideoWindow->pDlgVideo = NULL;
		}
		delete m_pVideoWindow;
		m_pVideoWindow = NULL;
	}
}

void CEasyClientDlg::OnCbnSelchangeComboWndMode()
{
	if (NULL == m_pVideoWindow)		return;

	int nSplitWindow = 4;
	CComboBox* pComboxWndMode = (CComboBox*)GetDlgItem(IDC_COMBO_WND_MODE);
	if (pComboxWndMode)
	{
		int nIdx = pComboxWndMode->GetCurSel();
		if (nIdx == 0)	nSplitWindow = 4;
		else if (nIdx == 1)	nSplitWindow = 8;
		else if (nIdx == 2)	nSplitWindow = 9;
		else if (nIdx == 3)	nSplitWindow = 16;

		m_pVideoWindow->channels		=	nSplitWindow;
		UpdateComponents();
	}
}


void CEasyClientDlg::GetPushServerInfo(ServerURLInfo* pPushServerInfo)
{
	UpdateData(TRUE);
	CEdit* pIP = (CEdit*)GetDlgItem(IDC_EDIT_SERVER_IP);
	CEdit* pPort = (CEdit*)GetDlgItem(IDC_EDIT_SERVER_PORT);
	CEdit* pName = (CEdit*)GetDlgItem(IDC_EDIT_PUSH_NAME);

	char szIp[128] = {0,};
	char szPort[128] = {0,};
	char szName[128] = {0,};
	char szPushBufferSize[128]={0,};
	wchar_t wszIp[128] = {0,};
	wchar_t wszPort[128] = {0,};
	wchar_t wszName[128] = {0,};
	wchar_t wszPushBufferSize[128]={0,};

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

	m_edtPushBuffer.GetWindowTextW(wszPushBufferSize, sizeof(wszPushBufferSize));
	if (wcslen(wszPushBufferSize) < 1)		
		return;
	__WCharToMByte(wszPushBufferSize, szPushBufferSize, sizeof(szPushBufferSize)/sizeof(szPushBufferSize[0]));
	int nPushBufSize = atoi(szPushBufferSize);

	int nRet = m_pManager->StartPush(szIp , nPort,  szName, nPushBufSize);
	if (pPushServerInfo)
	{
		pPushServerInfo->pushServerPort = nPort;
		pPushServerInfo->nPushBufferLenth = nPushBufSize;
		strcpy_s(pPushServerInfo->pushServerAddr , 128,  szIp);
		pPushServerInfo->pushServerPort = nPort;
		strcpy_s(pPushServerInfo->sdpName , 64,  szName);
	}
}
