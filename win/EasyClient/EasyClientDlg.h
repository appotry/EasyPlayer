/*
	Copyright (c) 2013-2014 EasyDarwin.ORG.  All rights reserved.
	Github: https://github.com/EasyDarwin
	WEChat: EasyDarwin
	Website: http://www.EasyDarwin.org
*/
// EasyClientDlg.h : header file
//

#pragma once

#include "DlgLocalPanel.h"
#include "DlgRemotePanel.h"
#include "DlgPanel.h"
#include "SourceManager.h"

// EasySkinUI Support
#include "EasySkinManager.h"
#include "afxwin.h"

typedef struct __EASY_VIDEO_PANEL_OBJ_T
{
	CDlgLocalPanel	*pDlgLocalPanel;
	CDlgRemotePanel	*pDlgRemotePanel;

	//CDlgPanel	*pDlgLocalPanel;
	//CDlgPanel	*pDlgRemotePanel;
}EASY_VIDEO_PANEL_OBJ_T;

#define MSG_LOG WM_USER +0x1001

// CEasyClientDlg dialog
class CEasyClientDlg : public CEasySkinManager
{
// Construction
public:
	CEasyClientDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	enum { IDD = IDD_EASYCLIENT_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support


protected:
	EASY_VIDEO_PANEL_OBJ_T		easyVideoPanelObj;
	void		UpdateComponents();

// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
public:
	virtual BOOL DestroyWindow();
	void OnGetMinMaxInfo(MINMAXINFO* lpMMI);

private:
	CSourceManager* m_pManager;
	CFont	m_ftSaticDefault;
	CEasySkinButton m_btnLocalView;
	CEasySkinButton m_btnPush;
	CEasySkinButton m_btnLiveView;

public:
	afx_msg void OnCbnSelchangeComboSource();
	afx_msg void OnBnClickedBtnCapture();
	afx_msg void OnBnClickedBtnPush();
	afx_msg void OnBnClickedBtnPlay();
	LRESULT OnLog(WPARAM wParam, LPARAM lParam);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);

	//EasySkinUI 界面美化
protected:
	//窗口绘制
	virtual void DrawClientArea(CDC*pDC,int nWidth,int nHeight);
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
	void UpdataResource();
};
