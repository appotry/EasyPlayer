/*
	Copyright (c) 2013-2015 EasyDarwin.ORG.  All rights reserved.
	Github: https://github.com/EasyDarwin
	WEChat: EasyDarwin
	Website: http://www.easydarwin.org
*/
// DlgRender.cpp : 实现文件
//

#include "stdafx.h"
#include "EasyPlayer.h"
#include "DlgRender.h"
#include "afxdialogex.h"


// CDlgRender 对话框

IMPLEMENT_DYNAMIC(CDlgRender, CDialogEx)

CDlgRender::CDlgRender(CWnd* pParent /*=NULL*/)
	: CDialogEx(CDlgRender::IDD, pParent)
{

}

CDlgRender::~CDlgRender()
{
}

void CDlgRender::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CDlgRender, CDialogEx)
	ON_WM_LBUTTONDBLCLK()
END_MESSAGE_MAP()


// CDlgRender 消息处理程序


void CDlgRender::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	::PostMessage(GetParent()->GetSafeHwnd(), WM_LBUTTONDBLCLK, 0, 0);

	CDialogEx::OnLButtonDblClk(nFlags, point);
}
