#pragma once


#include "DlgVideo.h"
#include "AudioSource\DirectSound.h"
#include "VideoSource\USBCamera\CameraDS.h"
#include "../libEasyPlayer/RtspClient/EasyTypes.h"
#include "../libEasyPlayer/libEasyPlayerAPI.h"
#ifdef _DEBUG
#pragma comment(lib, "../Debug/libEasyPlayer.lib")
#else
#pragma comment(lib, "../Release/libEasyPlayer.lib")
#endif
#include "EasyPusher\EasyPusherAPI.h"
#pragma comment(lib, "./EasyPusher/libEasyPusher.lib")

typedef struct __EASY_LOCAL_SOURCE_T
{
	
	
	int		rtspSourceId;

	Easy_Pusher_Handle	pusherHandle;


	char	pushServerAddr[128];
	int		pushServerPort;
	char	sdpName[64];
}EASY_LOCAL_SOURCE_T;


// CDlgLocalPanel 对话框

class CDlgLocalPanel : public CDialogEx
{
	DECLARE_DYNAMIC(CDlgLocalPanel)

public:
	CDlgLocalPanel(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CDlgLocalPanel();

// 对话框数据
	enum { IDD = IDD_DIALOG_LOCAL_PANEL };

protected:
	CStatic		*pStaticPushAddr;	//IDC_STATIC_PUSH_ADDR
	CEdit		*pEdtPushAddr;		//IDC_EDIT_PUSH_ADDR
	CStatic		*pStaticMediaSource;	//IDC_STATIC_MEDIA_SOURCE
	CComboBox	*pComboxMediaSource;	//IDC_COMBO_MEDIA_SOURCE
	CComboBox	*pComboxAudioSource;	//IDC_COMBO_AUDIO_SOURCE
	CButton		*pBtnStart;				//IDC_BUTTON_START
	CEdit		*pEdtRtspSource;		//IDC_EDIT_RTSP_SOURCE
	CDlgVideo		*pDlgVideo;
	void		UpdateComponents();

	CDirectSound	audioCapture;
	CCameraDS		usbCamera;
	CAMERA_LIST_T		*pUSBCameraList;
	EASY_LOCAL_SOURCE_T		localSource;

	void	CloseLocalSource();

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	virtual BOOL DestroyWindow();
	afx_msg void OnCbnSelchangeComboMediaSource();
	afx_msg void OnBnClickedButtonStart();
};
