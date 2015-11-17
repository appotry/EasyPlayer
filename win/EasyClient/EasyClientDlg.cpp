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
END_MESSAGE_MAP()


// CEasyClientDlg message handlers

BOOL CEasyClientDlg::OnInitDialog()
{
	CEasySkinManager::OnInitDialog();

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

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
	} 
	else
	{
		pVideoCombo->ShowWindow(SW_HIDE);
		pAudioCombo->ShowWindow(SW_HIDE);
		pRtspURL->ShowWindow(SW_SHOW);
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
	
	//上部分控件位置处理
	rcPosition.SetRect(100 , 50, 228, 74 );
	__MOVE_WINDOW(pComboxMediaSource, rcPosition);
	rcPosition.SetRect(nWidth-445 , 50, nWidth-5, 74 );
	__MOVE_WINDOW(pEdtRtspSource, rcPosition);

	rcPosition.SetRect(nWidth-445 , 50, nWidth-260, 74 );
	__MOVE_WINDOW(pVideoCombo, rcPosition);
	rcPosition.SetRect(nWidth-190 , 50, nWidth-5, 74 );
	__MOVE_WINDOW(pComboxAudioSource, rcPosition);

	//下部分控件位置处理
	int nStartX = (nWidth-210*3-140*2)/2;
// 	rcPosition.SetRect(nStartX , nHeight-280+15, nStartX + 100, nHeight-280+15+24 );
// 	__MOVE_WINDOW(pStcServerIp, rcPosition);
	rcPosition.SetRect(nStartX + 100 + 5, nHeight-280+15, nStartX + 210, nHeight-280+15+24 );
	__MOVE_WINDOW(pIP, rcPosition);

// 	rcPosition.SetRect(nStartX +210 +140, nHeight-280+15, nStartX + 210+140+100, nHeight-280+15+24 );
// 	__MOVE_WINDOW(pStcServerPort, rcPosition);
	rcPosition.SetRect(nStartX +210 +140 + 90 +5, nHeight-280+15, nStartX +210+140 + 210, nHeight-280+15+24 );
	__MOVE_WINDOW(pPort, rcPosition);

// 	rcPosition.SetRect(nStartX + (210 +140)*2+40, nHeight-280+15, nStartX + (210 +140)*2+100, nHeight-280+15+24 );
// 	__MOVE_WINDOW(pStcStreanName, rcPosition);
	rcPosition.SetRect(nStartX + (210 +140)*2 + 90+5, nHeight-280+15, nStartX + (210 +140)*2 + 210, nHeight-280+15+24 );
	__MOVE_WINDOW(pName, rcPosition);

	int nBtnStartX =  (rcClient.Width()-62*3-212*2)/2;

// 	rcPosition.SetRect(nBtnStartX , nHeight-280+24+25, nBtnStartX + 62, nHeight-280+25+24+33 );
// 	__MOVE_WINDOW(pBtnCapture, rcPosition);
// 	rcPosition.SetRect(nBtnStartX+62+212 , nHeight-280+24+25, nBtnStartX+62+212 + 62, nHeight-280+25+24+33 );
// 	__MOVE_WINDOW(pBtnPush, rcPosition);
// 	rcPosition.SetRect(nBtnStartX+(62+212)*2 , nHeight-280+24+25, nBtnStartX+(62+212)*2  + 62, nHeight-280+25+24+33 );
// 	__MOVE_WINDOW(pBtnPlay, rcPosition);

	DeferWindowPos(hDwp,m_btnLocalView,NULL,nBtnStartX,nHeight-280+24+25,0,0,uFlags|SWP_NOSIZE);
	DeferWindowPos(hDwp,m_btnPush,NULL,nBtnStartX+62+212, nHeight-280+24+25,0,0,uFlags|SWP_NOSIZE);
	DeferWindowPos(hDwp,m_btnLiveView,NULL,nBtnStartX+(62+212)*2,nHeight-280+24+25,0,0,uFlags|SWP_NOSIZE);

	rcPosition.SetRect(10 , nHeight-280+24+25+44,  nWidth-10, nHeight-30 );
	//__MOVE_WINDOW(pEdtShowLog, rcPosition);
	//DeferWindowPos(hDwp,pEdtShowLog->GetSafeHwnd(),NULL,10,nHeight-280+24+25+44,0,0,uFlags|SWP_NOSIZE);
	//大小变动控件
	if (pEdtShowLog)
	{
		DeferWindowPos(hDwp, pEdtShowLog->GetSafeHwnd(), NULL, 10, nHeight-280+24+25+44, nWidth-20,157, uFlags);
	}

		//结束调整
	LockWindowUpdate();
	EndDeferWindowPos(hDwp);
	UnlockWindowUpdate();

	int iPanelWidth = ((rcClient.Width()) / 2);

	CRect	rcLocalPanel;
	rcLocalPanel.SetRect(rcClient.left+2, rcClient.top+100, rcClient.left+iPanelWidth-2, rcClient.bottom-280);
	__MOVE_WINDOW(easyVideoPanelObj.pDlgLocalPanel, rcLocalPanel);

	CRect	rcRemotePanel;
	rcRemotePanel.SetRect(rcLocalPanel.right+1, rcLocalPanel.top, rcClient.right-3, rcLocalPanel.bottom);
	__MOVE_WINDOW(easyVideoPanelObj.pDlgRemotePanel, rcRemotePanel);
	Invalidate();
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
	} 
	else
	{
		pVideoCombo->ShowWindow(SW_HIDE);
		pComboxAudioSource->ShowWindow(SW_HIDE);
		pEdtRtspSource->ShowWindow(SW_SHOW);
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

// SkinUI 界面统一绘制函数
void CEasyClientDlg::DrawClientArea( CDC*pDC,int nWidth,int nHeight )
{
	CEasySkinManager::DrawClientArea(pDC,nWidth,nHeight);
	pDC->SetBkMode(TRANSPARENT);

	//绘制静态文本 （有没有更好的方法呢？？？--!）
	//上部分		
	CFont *pOldFont=pDC->SelectObject(&m_ftSaticDefault);
	CString strSourceType = _T("选择源类型:");
	pDC->DrawText(strSourceType,CRect(15,50,25+70,50+24),DT_CENTER| DT_VCENTER |DT_SINGLELINE|DT_END_ELLIPSIS);

	CComboBox* pSouceCombo = (CComboBox*)GetDlgItem(IDC_COMBO_SOURCE);
	int nSel  = 	pSouceCombo->GetCurSel();
	if (nSel == 0)
	{
		CString strCamera = _T("摄像头:");
		pDC->DrawText(strCamera,CRect(nWidth-520,50,nWidth-450,50+24),DT_CENTER| DT_VCENTER |DT_SINGLELINE|DT_END_ELLIPSIS);
		CString strMicphone = _T("麦克风:");
		pDC->DrawText(strMicphone,CRect(nWidth-260,50,nWidth-195,50+24),DT_CENTER| DT_VCENTER |DT_SINGLELINE|DT_END_ELLIPSIS);
	}
	else
	{
		CString strNetStream = _T("网络串流:");
		pDC->DrawText(strNetStream,CRect(nWidth-520,50,nWidth-445,50+24),DT_CENTER| DT_VCENTER |DT_SINGLELINE|DT_END_ELLIPSIS);
	}

	//下部分
	//下部分控件位置处理
	CRect rcPosition;
	int nStartX = (nWidth-210*3-140*2)/2;
	rcPosition.SetRect(nStartX , nHeight-280+15, nStartX + 100, nHeight-280+15+24 );
	CString strServerIp = _T("EasyDarwin IP:");
	pDC->DrawText(strServerIp,rcPosition,DT_CENTER| DT_VCENTER |DT_SINGLELINE|DT_END_ELLIPSIS);

	CString strServerPort = _T("EasyDarwin Port:");
	rcPosition.SetRect(nStartX +210 +140, nHeight-280+15, nStartX + 210+140+100, nHeight-280+15+24 );
	pDC->DrawText(strServerPort,rcPosition,DT_CENTER| DT_VCENTER |DT_SINGLELINE|DT_END_ELLIPSIS);

	CString strStreamName = _T("流名称:");
	rcPosition.SetRect(nStartX + (210 +140)*2+40, nHeight-280+15, nStartX + (210 +140)*2+100, nHeight-280+15+24 );
	pDC->DrawText(strStreamName,rcPosition,DT_CENTER| DT_VCENTER |DT_SINGLELINE|DT_END_ELLIPSIS);

	CString strDebugInfo = _T("Trace:");
	rcPosition.SetRect(10, nHeight-280+24+25+20, 70, nHeight-280+24+25+20+24 );
	pDC->DrawText(strDebugInfo,rcPosition,DT_CENTER| DT_VCENTER |DT_SINGLELINE|DT_END_ELLIPSIS);

	CString strVersionInfo = EasyClent_VersionInfo;
	rcPosition.SetRect(nWidth-520 , nHeight-25,  nWidth, nHeight );
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
}


HBRUSH CEasyClientDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CEasySkinManager::OnCtlColor(pDC, pWnd, nCtlColor);

	if(nCtlColor == CTLCOLOR_SCROLLBAR || //静态文本背景设置为透明
		nCtlColor == CTLCOLOR_STATIC)//||nCtlColor == CTLCOLOR_SCROLLBAR
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
