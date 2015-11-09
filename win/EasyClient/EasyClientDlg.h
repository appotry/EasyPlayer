
// EasyClientDlg.h : header file
//

#pragma once

#include "DlgLocalPanel.h"
#include "DlgRemotePanel.h"
#include "DlgPanel.h"
#include "SourceManager.h"

typedef struct __EASY_VIDEO_PANEL_OBJ_T
{
	CDlgLocalPanel	*pDlgLocalPanel;
	CDlgRemotePanel	*pDlgRemotePanel;

	//CDlgPanel	*pDlgLocalPanel;
	//CDlgPanel	*pDlgRemotePanel;
}EASY_VIDEO_PANEL_OBJ_T;

#define MSG_LOG WM_USER +0x1001

// CEasyClientDlg dialog
class CEasyClientDlg : public CDialogEx
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

private:
	CSourceManager* m_pManager;
public:
	afx_msg void OnCbnSelchangeComboSource();
	afx_msg void OnBnClickedBtnCapture();
	afx_msg void OnBnClickedBtnPush();
	afx_msg void OnBnClickedBtnPlay();
	LRESULT OnLog(WPARAM wParam, LPARAM lParam);

};
