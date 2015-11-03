
// EasyCameraDlg.cpp : implementation file
//

#include "stdafx.h"
#include "EasyCamera.h"
#include "EasyCameraDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif




// CEasyCameraDlg dialog




CEasyCameraDlg::CEasyCameraDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CEasyCameraDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);

	memset(&easyVideoPanelObj, 0x00, sizeof(EASY_VIDEO_PANEL_OBJ_T));
}

void CEasyCameraDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CEasyCameraDlg, CDialogEx)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
END_MESSAGE_MAP()


// CEasyCameraDlg message handlers

BOOL CEasyCameraDlg::OnInitDialog()
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

	EasyPlayer_Init();

	MoveWindow(0, 0, 1024, 480);

	return TRUE;  // return TRUE  unless you set the focus to a control
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CEasyCameraDlg::OnPaint()
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
HCURSOR CEasyCameraDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



LRESULT CEasyCameraDlg::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	if (WM_PAINT == message || WM_SIZE == message || WM_MOVE == message)
	{
		UpdateComponents();
	}

	return CDialogEx::WindowProc(message, wParam, lParam);
}
void	CEasyCameraDlg::UpdateComponents()
{
	CRect	rcClient;
	GetClientRect(&rcClient);
	if (rcClient.IsRectEmpty())		return;

	int iPanelWidth = (rcClient.Width() / 2);

	CRect	rcLocalPanel;
	rcLocalPanel.SetRect(rcClient.left, rcClient.top, rcClient.left+iPanelWidth, rcClient.bottom);
	__MOVE_WINDOW(easyVideoPanelObj.pDlgLocalPanel, rcLocalPanel);

	CRect	rcRemotePanel;
	rcRemotePanel.SetRect(rcLocalPanel.right, rcLocalPanel.top, rcClient.right, rcLocalPanel.bottom);
	__MOVE_WINDOW(easyVideoPanelObj.pDlgRemotePanel, rcRemotePanel);
}


BOOL CEasyCameraDlg::DestroyWindow()
{
	EasyPlayer_Release();

	__DESTROY_WINDOW(easyVideoPanelObj.pDlgLocalPanel);
	__DESTROY_WINDOW(easyVideoPanelObj.pDlgRemotePanel);

	return CDialogEx::DestroyWindow();
}
