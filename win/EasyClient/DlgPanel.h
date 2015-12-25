/*
	Copyright (c) 2013-2014 EasyDarwin.ORG.  All rights reserved.
	Github: https://github.com/EasyDarwin
	WEChat: EasyDarwin
	Website: http://www.EasyDarwin.org
*/
#pragma once

#include "DlgVideo.h"
#include "SourceManager.h"
#include "afxwin.h"

// CDlgPanel 对话框
class CEasyClientDlg;

class CDlgPanel : public CEasySkinDialog
{
	DECLARE_DYNAMIC(CDlgPanel)

public:
	CDlgPanel(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CDlgPanel();

// 对话框数据
	enum { IDD = IDD_DIALOG_PANEL };

protected:
	CDlgVideo		*pDlgVideo;
	CEdit	*m_pEdtServerIP;		
	CEdit	*m_pEdtServerPort;		
	CEdit	*m_pEdtServerStream;		

	CComboBox* m_pCmbType;//直播/推送切选
	CComboBox* m_pCmbSourceType;//源类型选择
	CComboBox* m_pCmbCamera;
	CComboBox* m_pCmbMic;
	CComboBox* m_pCmbScreenMode;
	
	CEdit	*m_pEdtRtspStream;		
	CEasySkinButton m_btnStart;
	CEasyClientDlg* m_pMainDlg;
	//EASY_CHANNEL_INFO_T	easyChannelInfo;

protected:
	//UI 界面绘制
	virtual void DrawClientArea(CDC*pDC,int nWidth,int nHeight);
	void		UpdateComponents();
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	afx_msg void OnBnClickedButtonStart();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnCbnSelchangeComboPannelType();
	afx_msg void OnCbnSelchangeComboPannelSource();
	DECLARE_MESSAGE_MAP()

public:
	virtual BOOL DestroyWindow();
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	virtual BOOL OnInitDialog();
	CWnd* GetDlgVideo();
	void SetMainDlg(CEasyClientDlg* pMainDlg, int nId);
	void UpdataResource();
	//流名称格式化
	void FormatStreamName(char* sStreamName);

private:
	CSourceManager* m_pManager;
	int m_nWndId;//自己的窗口Id
	CFont	m_ftSaticDefault;

public:
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnCbnSelchangeComboCapscreenMode();
};
