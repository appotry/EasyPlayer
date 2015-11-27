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

	memset(&easyVideoPanelObj, 0x00, sizeof(EASY_VIDEO_PANEL_OBJ_T));
	m_pManager = NULL;
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
	int nSel  = 	pSouceCombo->GetCurSel();
	if (nSel == 0)
	{
		pVideoCombo->ShowWindow(SW_SHOW);
		pAudioCombo->ShowWindow(SW_SHOW);
		pRtspURL->ShowWindow(SW_HIDE);
		m_edtVdieoWidth.ShowWindow(SW_SHOW);
		m_edtVideoHeight.ShowWindow(SW_SHOW);
		m_edtFPS.ShowWindow(SW_SHOW);
		m_edtVideoBitrate.ShowWindow(SW_SHOW);
	} 
	else
	{
		pVideoCombo->ShowWindow(SW_HIDE);
		pAudioCombo->ShowWindow(SW_HIDE);
		pRtspURL->ShowWindow(SW_SHOW);
		m_edtVdieoWidth.ShowWindow(SW_HIDE);
		m_edtVideoHeight.ShowWindow(SW_HIDE);
		m_edtFPS.ShowWindow(SW_HIDE);
		m_edtVideoBitrate.ShowWindow(SW_HIDE);

	}

	MoveWindow(0, 0, 926, 727);

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

	CRect rcPosition;
	int nWidth = rcClient.Width();
	int nHeight = rcClient.Height();	
	
	//左边部分	
	int nStartX = (nWidth/2-450)/2;
	rcPosition.SetRect(nStartX+70 , nHeight-320+15, nStartX+70+360, nHeight-320+15+25 );
	__MOVE_WINDOW(pComboxMediaSource, rcPosition);
	rcPosition.SetRect(nStartX+70 , nHeight-320+15+30, nStartX+70+360, nHeight-320+15+30+25 );
	__MOVE_WINDOW(pEdtRtspSource, rcPosition);
	if (pEdtRtspSource)
	{
		//设置焦点刷新
		pEdtRtspSource->SetFocus();
	}

	rcPosition.SetRect(nStartX+70 , nHeight-320+15+30, nStartX+70+140, nHeight-320+15+30+25 );
	__MOVE_WINDOW(pVideoCombo, rcPosition);
	rcPosition.SetRect(nStartX+70+220 , nHeight-320+15+30, nStartX+70+220+140, nHeight-320+15+30+25 );
	__MOVE_WINDOW(pComboxAudioSource, rcPosition);

// 	CEasySkinButton m_edtVdieoWidth;
// 	CEasySkinButton m_edtVideoHeight;
// 	CEasySkinButton m_edtFPS;
// 	CEasySkinButton m_edtVideoBitrate;
// 	CEasySkinButton m_edtPushBuffer;

	//视频宽度
	rcPosition.SetRect(nStartX+110 , nHeight-320+15+60, nStartX+110+50, nHeight-320+15+60+25 );
	if (m_edtVdieoWidth.GetSafeHwnd())
	{
		m_edtVdieoWidth.MoveWindow(rcPosition);
		m_edtVdieoWidth.SetFocus();
	}
	
	//视频高度
	rcPosition.SetRect(nStartX+190 , nHeight-320+15+60, nStartX+190+50, nHeight-320+15+60+25 );
		if (m_edtVideoHeight.GetSafeHwnd())
		{
			m_edtVideoHeight.MoveWindow(rcPosition);
			m_edtVideoHeight.SetFocus();
		}

	//视频FPS
	rcPosition.SetRect(nStartX+270 , nHeight-320+15+60, nStartX+270+50, nHeight-320+15+60+25 );
		if (m_edtFPS.GetSafeHwnd())
		{
			m_edtFPS.MoveWindow(rcPosition);
			m_edtFPS.SetFocus();
		}

	//视频比特率
	rcPosition.SetRect(nStartX+380 , nHeight-320+15+60, nStartX+380+50, nHeight-320+15+60+25 );
		if (m_edtVideoBitrate.GetSafeHwnd())
		{
			m_edtVideoBitrate.MoveWindow(rcPosition);
			m_edtVideoBitrate.SetFocus();
		}

	//右边部分控件位置处理
	nStartX = nWidth/2+(nWidth/2-450)/2;
	rcPosition.SetRect(nStartX+20, nHeight-320+15+30, nStartX + 20+180, nHeight-320+15+30+25 );
	__MOVE_WINDOW(pIP, rcPosition);
	if (pIP)
	{
		pIP->SetFocus();
	}
	rcPosition.SetRect(nStartX+240, nHeight-320+15+30, nStartX + 240+40, nHeight-320+15+30+25 );
	__MOVE_WINDOW(pPort, rcPosition);
	if (pPort)
	{
		pPort->SetFocus();
	}
	rcPosition.SetRect(nStartX+340, nHeight-320+15+30, nStartX + 340+100, nHeight-320+15+30+25 );
	__MOVE_WINDOW(pName, rcPosition);
	if (pName)
	{
		pName->SetFocus();
	}

	rcPosition.SetRect(nStartX+80, nHeight-320+15+60, nStartX + 80+100, nHeight-320+15+60+25 );
		if (m_edtPushBuffer.GetSafeHwnd())
		{
				m_edtPushBuffer.MoveWindow(rcPosition);
				m_edtPushBuffer.SetFocus();
		}

// 	CEdit* pPort = (CEdit*)GetDlgItem(IDC_EDIT_SERVER_PORT);
// 	CEdit* pName = (CEdit*)GetDlgItem(IDC_EDIT_PUSH_NAME);

	//按钮位置
	int nBtnStartX =  (rcClient.Width()-62*3-212*2)/2;
	DeferWindowPos(hDwp,m_btnLocalView,NULL,nBtnStartX,nHeight-250+24+25,0,0,uFlags|SWP_NOSIZE);
	DeferWindowPos(hDwp,m_btnPush,NULL,nBtnStartX+62+212, nHeight-250+24+25,0,0,uFlags|SWP_NOSIZE);
	DeferWindowPos(hDwp,m_btnLiveView,NULL,nBtnStartX+(62+212)*2,nHeight-250+24+25,0,0,uFlags|SWP_NOSIZE);


	DeferWindowPos(hDwp,m_linkEasyDarwinWeb,NULL,nWidth-250,nHeight-23,0,0,uFlags|SWP_NOSIZE);
	if (m_linkEasyDarwinWeb.m_hWnd)
	{
// 		CRect rcClient;
// 		m_linkEasyDarwinWeb.GetClientRect(&rcClient);
// 		m_linkEasyDarwinWeb.MoveWindow( nWidth-225,nHeight-23, rcClient.Width(), rcClient.Height() );
// 		m_linkEasyDarwinWeb.Invalidate();
		m_linkEasyDarwinWeb.SetFocus();
	}
	DeferWindowPos(hDwp,m_linkEasyClientGethub,NULL,nWidth-120,nHeight-23,0,0,uFlags|SWP_NOSIZE);
	if (m_linkEasyClientGethub.m_hWnd)
	{
		m_linkEasyClientGethub.SetFocus();
	}

	rcPosition.SetRect(10 , nHeight-280+24+25+44,  nWidth-10, nHeight-30 );
	//__MOVE_WINDOW(pEdtShowLog, rcPosition);
	//DeferWindowPos(hDwp,pEdtShowLog->GetSafeHwnd(),NULL,10,nHeight-280+24+25+44,0,0,uFlags|SWP_NOSIZE);
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

	int iPanelWidth = ((rcClient.Width()) / 2);

	CRect	rcLocalPanel;
	rcLocalPanel.SetRect(rcClient.left+2, rcClient.top+60, rcClient.left+iPanelWidth-2, rcClient.bottom-320);
	__MOVE_WINDOW(easyVideoPanelObj.pDlgLocalPanel, rcLocalPanel);
	if (easyVideoPanelObj.pDlgLocalPanel)
	{
		easyVideoPanelObj.pDlgLocalPanel->Invalidate(FALSE);
	}

	CRect	rcRemotePanel;
	rcRemotePanel.SetRect(rcLocalPanel.right+1, rcLocalPanel.top, rcClient.right-3, rcLocalPanel.bottom);
	__MOVE_WINDOW(easyVideoPanelObj.pDlgRemotePanel, rcRemotePanel);
	if (easyVideoPanelObj.pDlgRemotePanel)
	{
		easyVideoPanelObj.pDlgRemotePanel->Invalidate(FALSE);
	}
}

void CEasyClientDlg::OnGetMinMaxInfo(MINMAXINFO* lpMMI)
{
	lpMMI->ptMinTrackSize.x = 926;//954, 780
	lpMMI->ptMinTrackSize.y = 727;

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
	__DESTROY_WINDOW(easyVideoPanelObj.pDlgLocalPanel);
	__DESTROY_WINDOW(easyVideoPanelObj.pDlgRemotePanel);

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
		CWnd* pCapWnd = easyVideoPanelObj.pDlgLocalPanel->GetDlgVideoHwnd();
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
	int nStartX = (nWidth/2-450)/2;
	CFont *pOldFont=pDC->SelectObject(&m_ftSaticDefault);
	CString strSourceType = _T("选择源类型:");
	pDC->DrawText(strSourceType,CRect(nStartX,nHeight-320+15,nStartX+70,nHeight-320+15+24),DT_LEFT| DT_VCENTER |DT_SINGLELINE|DT_END_ELLIPSIS);

	CComboBox* pSouceCombo = (CComboBox*)GetDlgItem(IDC_COMBO_SOURCE);
	int nSel  = 	pSouceCombo->GetCurSel();
	if (nSel == 0)
	{
		CString strCamera = _T("摄像头:");
		pDC->DrawText(strCamera,CRect(nStartX,nHeight-320+15+30,nStartX+70,nHeight-320+15+30+24),DT_LEFT| DT_VCENTER |DT_SINGLELINE|DT_END_ELLIPSIS);
		CString strMicphone = _T("麦克风:");
		pDC->DrawText(strMicphone,CRect(nStartX+220,nHeight-320+15+30,nStartX+220+70,nHeight-320+15+30+24),DT_CENTER| DT_VCENTER |DT_SINGLELINE|DT_END_ELLIPSIS);
		//音视频参数设置
		CString strVideoParam = _T("视频参数设置:");
		pDC->DrawText(strVideoParam,CRect(nStartX,nHeight-320+15+60,nStartX+80,nHeight-320+15+60+24),DT_LEFT| DT_VCENTER |DT_SINGLELINE|DT_END_ELLIPSIS);
		//宽
		CString strVideoWidth = _T("宽:");
		pDC->DrawText(strVideoWidth,CRect(nStartX+80,nHeight-320+15+60,nStartX+80+30,nHeight-320+15+60+24),DT_CENTER| DT_VCENTER |DT_SINGLELINE|DT_END_ELLIPSIS);

		//	高
		CString strVideoHeight = _T("高:");
		pDC->DrawText(strVideoHeight,CRect(nStartX+160,nHeight-320+15+60,nStartX+160+30,nHeight-320+15+60+24),DT_CENTER| DT_VCENTER |DT_SINGLELINE|DT_END_ELLIPSIS);

		//	帧率
		CString strFPS = _T("FPS:");
		pDC->DrawText(strFPS,CRect(nStartX+240,nHeight-320+15+60,nStartX+240+30,nHeight-320+15+60+24),DT_CENTER| DT_VCENTER |DT_SINGLELINE|DT_END_ELLIPSIS);

		//	比特率
		CString strVideobitrate = _T("比特率:");
		pDC->DrawText(strVideobitrate,CRect(nStartX+320,nHeight-320+15+60,nStartX+320+60,nHeight-320+15+60+24),DT_CENTER| DT_VCENTER |DT_SINGLELINE|DT_END_ELLIPSIS);

	}
	else
	{
		CString strNetStream = _T("网络串流:");
		pDC->DrawText(strNetStream,CRect(nStartX,nHeight-320+15+30,nStartX+70,nHeight-320+15+30+24),DT_CENTER| DT_VCENTER |DT_SINGLELINE|DT_END_ELLIPSIS);
	}

	CRect rcPosition;
	nStartX = (nWidth/2-60)/2;
	rcPosition.SetRect(nStartX , 39, nStartX + 67, 50 );
	CString strLocalVideo = _T("本地视频");
	pDC->DrawText(strLocalVideo,rcPosition,DT_CENTER| DT_VCENTER |DT_SINGLELINE|DT_END_ELLIPSIS);
	
	rcPosition.SetRect(nStartX+nWidth/2 , 39, nStartX+nWidth/2 + 67, 50 );
	CString strServerlVideo = _T("直播视频");
	pDC->DrawText(strServerlVideo,rcPosition,DT_CENTER| DT_VCENTER |DT_SINGLELINE|DT_END_ELLIPSIS);

	//右部分
	//控件位置处理
	nStartX = nWidth/2+(nWidth/2-450)/2;
	//rcPosition.SetRect(nStartX , nHeight-280+15, nStartX + 100, nHeight-280+15+24 );
	rcPosition.SetRect(nStartX , nHeight-320+15, nStartX + 420, nHeight-320+15+24 );
	CString strServerIp = _T("EasyDarwin服务器流URL:  (服务器流 推送地址/直播地址)");
	pDC->DrawText(strServerIp,rcPosition,DT_LEFT| DT_VCENTER |DT_SINGLELINE|DT_END_ELLIPSIS);

 	rcPosition.SetRect(nStartX, nHeight-320+15+30, nStartX + 30, nHeight-320+15+30+25 );
	CString strIp = _T("IP:");
 	pDC->DrawText(strIp,rcPosition,DT_LEFT| DT_VCENTER |DT_SINGLELINE|DT_END_ELLIPSIS);

	rcPosition.SetRect(nStartX+210, nHeight-320+15+30, nStartX + 210+30, nHeight-320+15+30+25 );
	CString strPort = _T("Port:");
	pDC->DrawText(strPort,rcPosition,DT_CENTER| DT_VCENTER |DT_SINGLELINE|DT_END_ELLIPSIS);

	rcPosition.SetRect(nStartX+280, nHeight-320+15+30, nStartX + 280+60, nHeight-320+15+30+25 );
	CString strStream = _T("Stream:");
	pDC->DrawText(strStream,rcPosition,DT_CENTER| DT_VCENTER |DT_SINGLELINE|DT_END_ELLIPSIS);

	//推送参数设置
	rcPosition.SetRect(nStartX , nHeight-320+15+60, nStartX + 80, nHeight-320+15+60+24 );
	CString strPushParam = _T("推送缓存设置:");
	pDC->DrawText(strPushParam,rcPosition,DT_LEFT| DT_VCENTER |DT_SINGLELINE|DT_END_ELLIPSIS);

	rcPosition.SetRect(nStartX+180 , nHeight-320+15+60, nStartX + 380, nHeight-320+15+60+24 );
	CString strPushParamExplain = _T("(范围：512~2048   单位：KB)");
	pDC->DrawText(strPushParamExplain,rcPosition,DT_LEFT| DT_VCENTER |DT_SINGLELINE|DT_END_ELLIPSIS);

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
	TRACE("%d\r\n", pWnd->GetDlgCtrlID());
	if (pWnd->GetDlgCtrlID()==IDC_SYSLINK_EASYDARWIN || pWnd->GetDlgCtrlID() == IDC_SYSLINK_EASYCLIENT_GITHUB)
	{
		pDC-> SetBkMode(TRANSPARENT); 
		CRect rc;
		pWnd->GetWindowRect(&rc);
		ScreenToClient(&rc);

		CDC* dc = GetDC();
		pDC->BitBlt(0,0,rc.Width(),rc.Height(),dc,rc.left,rc.top,SRCCOPY);    //把父窗口背景先画到按钮上
		ReleaseDC(dc);

        hbr = (HBRUSH) ::GetStockObject(NULL_BRUSH);
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
	// TODO: Add your message handler code here and/or call default

	return CEasySkinManager::OnEraseBkgnd(pDC);
}
