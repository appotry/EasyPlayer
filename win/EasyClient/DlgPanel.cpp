/*
	Copyright (c) 2013-2014 EasyDarwin.ORG.  All rights reserved.
	Github: https://github.com/EasyDarwin
	WEChat: EasyDarwin
	Website: http://www.EasyDarwin.org
*/
// DlgPanel.cpp : 实现文件
//

#include "stdafx.h"
#include "EasyClient.h"
#include "DlgPanel.h"
#include "afxdialogex.h"


// CDlgPanel 对话框

IMPLEMENT_DYNAMIC(CDlgPanel, CDialogEx)

CDlgPanel::CDlgPanel(CWnd* pParent /*=NULL*/)
	: CDialogEx(CDlgPanel::IDD, pParent)
{
	pDlgVideo		=	NULL;

	//memset(&easyChannelInfo, 0x00, sizeof(EASY_CHANNEL_INFO_T));
}

CDlgPanel::~CDlgPanel()
{
}

void CDlgPanel::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CDlgPanel, CDialogEx)
	ON_BN_CLICKED(IDC_BUTTON_START, &CDlgPanel::OnBnClickedButtonStart)
END_MESSAGE_MAP()


// CDlgPanel 消息处理程序


BOOL CDlgPanel::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	pDlgVideo = new CDlgVideo();
	pDlgVideo->Create(IDD_DIALOG_VIDEO, this);
	pDlgVideo->ShowWindow(SW_SHOW);

	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}

BOOL CDlgPanel::DestroyWindow()
{
	__DESTROY_WINDOW(pDlgVideo);

	return CDialogEx::DestroyWindow();
}


LRESULT CDlgPanel::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	if (WM_PAINT == message || WM_SIZE == message || WM_MOVE == message)
	{
		UpdateComponents();
	}

	return CDialogEx::WindowProc(message, wParam, lParam);
}
void CDlgPanel::UpdateComponents()
{
	CRect	rcClient;
	GetClientRect(&rcClient);
	if (rcClient.IsRectEmpty())		return;

	CRect	rcVideo;
	rcVideo.SetRect(rcClient.left, rcClient.top+80, rcClient.right, rcClient.bottom);
	__MOVE_WINDOW(pDlgVideo, rcVideo);
}


void CDlgPanel::OnBnClickedButtonStart()
{
	
	//easyChannelInfo


}
