// DlgVideo.cpp : 实现文件
//

#include "stdafx.h"
#include "EasyCamera.h"
#include "DlgVideo.h"
#include "afxdialogex.h"


// CDlgVideo 对话框

IMPLEMENT_DYNAMIC(CDlgVideo, CDialogEx)

CDlgVideo::CDlgVideo(CWnd* pParent /*=NULL*/)
	: CDialogEx(CDlgVideo::IDD, pParent)
{

}

CDlgVideo::~CDlgVideo()
{
}

void CDlgVideo::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CDlgVideo, CDialogEx)
END_MESSAGE_MAP()


// CDlgVideo 消息处理程序
