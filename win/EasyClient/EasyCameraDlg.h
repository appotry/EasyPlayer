
// EasyCameraDlg.h : header file
//

#pragma once

#include "DlgLocalPanel.h"
#include "DlgRemotePanel.h"
#include "DlgPanel.h"
typedef struct __EASY_VIDEO_PANEL_OBJ_T
{
	CDlgLocalPanel	*pDlgLocalPanel;
	CDlgRemotePanel	*pDlgRemotePanel;

	//CDlgPanel	*pDlgLocalPanel;
	//CDlgPanel	*pDlgRemotePanel;
}EASY_VIDEO_PANEL_OBJ_T;

// CEasyCameraDlg dialog
class CEasyCameraDlg : public CDialogEx
{
// Construction
public:
	CEasyCameraDlg(CWnd* pParent = NULL);	// standard constructor



// Dialog Data
	enum { IDD = IDD_EASYCAMERA_DIALOG };



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
};
