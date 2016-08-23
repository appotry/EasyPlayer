
// EasyPlayerDlg.h : 头文件
//

#pragma once

#include "DlgVideo.h"
#include ".\EasySkinUI\skindialog.h"
// EasyProtocol协议支持 [8/11/2016 Dingshuai]
#include "EasyProtocol.h"
using namespace EasyDarwin::Protocol;
#pragma comment(lib, "../bin/EasyProtocol.lib")
#pragma comment(lib, "../bin/libjson.lib")

#include <string.h>
using namespace std;


#define	_SURV_MAX_WINDOW_NUM		16

typedef struct __VIDEO_NODE_T
{
	bool		fullscreen;
	int			maximizedId;
	int			selectedId;
	int			channels;
	CDlgVideo	*pDlgVideo;
}VIDEO_NODE_T;

// CEasyClientDlg 对话框
class CEasyClientDlg : public CSkinDialog
{
// 构造
public:
	CEasyClientDlg(CWnd* pParent = NULL);	// 标准构造函数

// 对话框数据
	enum { IDD = IDD_EASYPLAYER_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持

// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnCbnSelchangeComboSplitScreen();
	afx_msg void OnCbnSelchangeComboRenderFormat();
	afx_msg void OnBnClickedCheckShowntoscale();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnDestroy();
	//设备树上节点双击消息
	afx_msg void	OnNMDBClickListDevices(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg long HandleButtonMessage(WPARAM wCmdID, LPARAM lParm);	DECLARE_MESSAGE_MAP()
	LRESULT OnWindowMaximized(WPARAM wParam, LPARAM lParam);
public:
	virtual BOOL DestroyWindow();
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);

	void SetActiveState(int nWndId, int nActiveState=1);
	int SendHttpReqDeviceList();
	static UINT WINAPI ReqDeviceListThread(LPVOID pParam);
	void ProcessReqDevListThread();
	void	InitialComponents();
	void	CreateComponents();
	void	UpdateComponents();
	void	DeleteComponents();
	void	UpdateVideoPosition(LPRECT lpRect);
	/*
	函数：DeleteChildren(HTREEITEM hItem)
	功能：删除hItem下的所有孩子节点
	*/
	UINT DeleteChildren(CSkinTreeCtrl *pTreeCtrl, HTREEITEM hItem);
	void ExpandAllItem(CSkinTreeCtrl *pTreeCtrl, HTREEITEM hTreeItem, bool bExpand = true);

private:
	//控件声明变量
	CComboBox		*pComboxSplitScreen;
	CComboBox		*pComboxRenderFormat;	//IDC_COMBO_RENDER_FORMAT
	VIDEO_NODE_T	*pVideoWindow;		//视频窗口
	CSkinButton			pChkShownToScale;	//按比例显示
	CStatic			*pStaticCopyright;	//IDC_STATIC_COPYRIGHT
	CSkinTreeCtrl* m_pTreeCtrDevList;
	HTREEITEM m_hRoot;
	//树上的图标列表
	CImageList	m_StatusImage;

	// EasyClient功能扩展 [8/11/2016 Dingshuai]
	CInternetSession *m_pSession;
	EasyDarwin::Protocol::EasyDevicesInfo m_devicesInfo;
	CString m_strCMSIP; 
	int m_nCMSPort;
	int m_nCurSelWnd;
	HANDLE m_hReqDeviceListThread;

public:
	bool m_bReqDeviceListThreadRuning;
};
