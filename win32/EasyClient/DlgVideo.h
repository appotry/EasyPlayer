#pragma once

#include "DlgRender.h"
// CDlgVideo 对话框


#include "../libEasyPlayer/libEasyPlayerAPI.h"


class CEasyClientDlg;

class CDlgVideo : public CDialogEx
{
	DECLARE_DYNAMIC(CDlgVideo)

public:
	CDlgVideo(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CDlgVideo();

	void	SetWindowId(int _windowId);
	void	SetShownToScale(int shownToScale);
	void	SetURL(char *url);

	bool	bDrag;

	int		m_WindowId;
	int		m_ChannelId;
	CDlgRender	*pDlgRender;
	CEdit	*pEdtURL;		//IDC_EDIT_RTSP_URL
	CEdit	*pEdtUsername;	//IDC_EDIT_USERNAME
	CEdit	*pEdtPassword;	//IDC_EDIT_PASSWORD
	CButton	*pChkOSD;		//IDC_CHECK_OSD
	CSliderCtrl	*pSliderCache;	//IDC_SLIDER_CACHE
	CButton	*pBtnPreview;	//IDC_BUTTON_PREVIEW
	void	InitialComponents();
	void	CreateComponents();
	void	UpdateComponents();
	void	DeleteComponents();

// 对话框数据
	enum { IDD = IDD_DIALOG_VIDEO };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnBnClickedButtonPreview();
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	virtual BOOL OnInitDialog();
	virtual BOOL DestroyWindow();
	afx_msg void OnBnClickedCheckOsd();
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnPaint();
	//设置视频窗口激活状态
	void SetActiveState(long nActiveState, bool bInterface=false);
	//画窗口边框颜色
	void Draw3DRectState(CDC *pDC);
	long GetActiveState();
	void SetMainDlg(CEasyClientDlg* pMainDlg);
	CString GetDeviceSerial()
	{
		return m_sDevSerial; 
	}
	CString GetDeviceChannel()
	{
		return m_sChannel; 
	}	
	void SetDeviceSerial(CString strDevSerival, CString strChannel)
	{
		m_sDevSerial = strDevSerival;
		m_sChannel = strChannel;
	}
	void SetRMSIPPort(CString strIP, CString strPort)
	{
		m_sRMSIP = strIP;
		m_sRMSPort = strPort;
	}
	CString GetRMSIP()
	{
		return m_sRMSIP ;
	}
	CString GetRMSPort()
	{
		return m_sRMSPort ;
	}

	int Preview();

public:
	COLORREF		m_clrDefault3D;
	COLORREF		m_clrHilight3D;
	COLORREF		m_clrCurrent3D;
	//记录是否有焦点，处于激活状态
	int			m_nActiveState;
	CEasyClientDlg* m_pMainDlg;
	CString m_sDevSerial;
	CString m_sChannel;
	//RMS IP和端口
	CString m_sRMSIP ;
	CString m_sRMSPort ;
};
