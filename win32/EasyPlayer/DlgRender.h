/*
	Copyright (c) 2013-2015 EasyDarwin.ORG.  All rights reserved.
	Github: https://github.com/EasyDarwin
	WEChat: EasyDarwin
	Website: http://www.easydarwin.org
*/
#pragma once


// CDlgRender 对话框

class CDlgRender : public CDialogEx
{
	DECLARE_DYNAMIC(CDlgRender)

public:
	CDlgRender(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CDlgRender();

// 对话框数据
	enum { IDD = IDD_DIALOG_RENDER };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
};
