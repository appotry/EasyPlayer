// DlgRemotePanel.cpp : 实现文件
//

#include "stdafx.h"
#include "EasyCamera.h"
#include "DlgRemotePanel.h"
#include "afxdialogex.h"


// CDlgRemotePanel 对话框

IMPLEMENT_DYNAMIC(CDlgRemotePanel, CDialogEx)

CDlgRemotePanel::CDlgRemotePanel(CWnd* pParent /*=NULL*/)
	: CDialogEx(CDlgRemotePanel::IDD, pParent)
{
	pStaticServerAddr=	NULL;
	pEdtServerAddr	=	NULL;
	pBtnStart		=	NULL;
	pDlgVideo		=	NULL;
}

CDlgRemotePanel::~CDlgRemotePanel()
{
}

void CDlgRemotePanel::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CDlgRemotePanel, CDialogEx)
	ON_BN_CLICKED(IDC_BUTTON_START, &CDlgRemotePanel::OnBnClickedButtonStart)
END_MESSAGE_MAP()


// CDlgRemotePanel 消息处理程序


BOOL CDlgRemotePanel::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	pStaticServerAddr = (CStatic *)GetDlgItem(IDC_STATIC_SERVER_ADDR);
	pEdtServerAddr = (CEdit *)GetDlgItem(IDC_EDIT_SERVER_ADDR);
	pBtnStart = (CButton *)GetDlgItem(IDC_BUTTON_START);

	pDlgVideo = new CDlgVideo();
	pDlgVideo->Create(IDD_DIALOG_VIDEO, this);
	pDlgVideo->ShowWindow(SW_SHOW);

#ifdef _DEBUG
	if (NULL != pEdtServerAddr)	pEdtServerAddr->SetWindowText(TEXT("rtsp://192.168.1.186:8557"));

	if (NULL != pEdtServerAddr)	pEdtServerAddr->SetWindowText(TEXT("rtsp://127.0.0.1:554/stream.sdp"));
#endif

	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}


LRESULT CDlgRemotePanel::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	if (WM_PAINT == message || WM_SIZE == message || WM_MOVE == message)
	{
		UpdateComponents();
	}

	return CDialogEx::WindowProc(message, wParam, lParam);
}


BOOL CDlgRemotePanel::DestroyWindow()
{
	CloseRtspChannel();
	__DESTROY_WINDOW(pDlgVideo);

	return CDialogEx::DestroyWindow();
}
void CDlgRemotePanel::UpdateComponents()
{
	CRect	rcClient;
	GetClientRect(&rcClient);
	if (rcClient.IsRectEmpty())		return;

	CRect	rcServerAddr;
	rcServerAddr.SetRect(rcClient.left+3, rcClient.top+3, rcClient.left+80, rcClient.top+23);
	__MOVE_WINDOW(pStaticServerAddr, rcServerAddr);
	rcServerAddr.SetRect(rcServerAddr.right, rcServerAddr.top, rcClient.right-80, rcServerAddr.bottom);
	__MOVE_WINDOW(pEdtServerAddr, rcServerAddr);
	CRect	rcStartBtn;
	rcStartBtn.SetRect(rcServerAddr.right+5, rcServerAddr.top, rcClient.right-5, rcServerAddr.bottom);
	__MOVE_WINDOW(pBtnStart, rcStartBtn);

	CRect	rcVideo;
	rcVideo.SetRect(rcClient.left+VIDEO_WINDOW_BORDER_WIDTH, rcServerAddr.bottom+5, rcClient.right-VIDEO_WINDOW_BORDER_WIDTH, rcClient.bottom-VIDEO_WINDOW_BORDER_WIDTH);
	__MOVE_WINDOW(pDlgVideo, rcVideo);
}

void		CDlgRemotePanel::CloseRtspChannel()
{
	if (easyChannelInfo.channelId > 0)
	{
		EasyPlayer_CloseStream(easyChannelInfo.channelId);
		easyChannelInfo.channelId = -1;
		if (NULL != pBtnStart)		pBtnStart->SetWindowText(TEXT("Start"));
	}

	if (NULL != pDlgVideo)	pDlgVideo->Invalidate();
}

void CDlgRemotePanel::OnBnClickedButtonStart()
{
	//Stop
	if (easyChannelInfo.channelId > 0)
	{
		CloseRtspChannel();
		return;
	}

	//Start
	wchar_t wszURL[128] = {0,};
	if (NULL != pEdtServerAddr)	pEdtServerAddr->GetWindowTextW(wszURL, sizeof(wszURL));
	if (wcslen(wszURL) < 1)		return;

	char szURL[128] = {0,};
	__WCharToMByte(wszURL, szURL, sizeof(szURL)/sizeof(szURL[0]));

	HWND hWnd = NULL;
	if (NULL != pDlgVideo)	hWnd = pDlgVideo->GetSafeHwnd();

	easyChannelInfo.channelId = EasyPlayer_OpenStream(szURL, hWnd, DISPLAY_FORMAT_RGB24_GDI, 0x00, "", "");
	
	if (easyChannelInfo.channelId > 0)
	{
		EasyPlayer_SetFrameCache(easyChannelInfo.channelId, 3);		//设置缓存
		EasyPlayer_PlaySound(easyChannelInfo.channelId);
		if (NULL != pBtnStart)		pBtnStart->SetWindowText(TEXT("Stop"));
	}
}
