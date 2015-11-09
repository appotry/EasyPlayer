/*
	Copyright (c) 2013-2014 EasyDarwin.ORG.  All rights reserved.
	Github: https://github.com/EasyDarwin
	WEChat: EasyDarwin
	Website: http://www.EasyDarwin.org
*/
// EasyDarwin推送和接收源端的管理类 [11/8/2015 Dingshuai]
// Add by SwordTwelve

#pragma once

//本地音频捕获
#include "AudioSource\DirectSound.h"
//本地视频捕获
#include "VideoSource\USBCamera\CameraDS.h"
//网络音视频流采集
#include "EasyPlayerManager.h"

typedef enum tagSOURCE_TYPE
{
	SOURCE_LOCAL_CAMERA = 0,//本地音视频
	SOURCE_RTSP_STREAM=1,//RTSP流
	SOURCE_ONVIF_STREAM=2//Onvif流

}SOURCE_TYPE;

class CEasyClientDlg ;

class CSourceManager
{
public:
	CSourceManager(void);
	~CSourceManager(void);

public:
	static CSourceManager* s_pSourceManager; 
	//初始化唯一的管理实例
	static CSourceManager* Instance();
	//销毁唯一的管理实例
	static void UnInstance();
	//释放Master所占相关资源
	void RealseManager();
	// Member Function Interface
public:
	//执行视频源初始化操作
	void InitSource();
	//反初始化
	void UnInitSource();
	static int CALLBACK __MediaSourceCallBack( int _channelId, int *_channelPtr, int _frameType, char *pBuf, RTSP_FRAME_INFO* _frameInfo);
	int SourceManager(int _channelId, int *_channelPtr, int _frameType, char *pBuf, RTSP_FRAME_INFO* _frameInfo);

	//开始捕获(采集)
	int StartCapture(SOURCE_TYPE eSourceType, int nCamId,  HWND hCapWnd, char* szURL = NULL);
	//停止采集
	void StopCapture();

	//开始推流
	int StartPush(char* ServerIp, int nPushPort, char* sPushName);
	//停止推流
	void StopPush();
	
	//开始播放
	int StartPlay(char* szURL, HWND hShowWnd);
	//停止播放
	void StopPlay();
	void SetMainDlg(	CEasyClientDlg* pMainDlg);
	void LogErr(CString strLog);
	void EnumLocalAVDevInfo(CWnd* pComboxMediaSource, CWnd* pComboxAudioSource);

	//状态
	BOOL IsInCapture()
	{
		BOOL bCap = FALSE;
		int nStreamCap = m_netStreamCapture.InRunning();
		int nVideoCap = m_videoCamera.InRunning();
		if (nVideoCap>0 ||nStreamCap>0 )
			bCap = TRUE;
		else
			bCap = FALSE;
		return bCap;
	}
	BOOL IsInPushing()
	{
		//return (int)m_sPushInfo.pusherHandle>0?TRUE:FALSE;
		return m_bPushing;
	}
	BOOL IsInPlaying()
	{
		return m_netStreamPlayer.InRunning()>0?TRUE:FALSE;
	}
protected:
		void	UpdateLocalVideo(unsigned char *pbuf, int size, int width, int height);

	//Member Var
private:
	CEasyClientDlg* m_pMainDlg;
	CDirectSound	m_audioCapture;
	CCameraDS		m_videoCamera;
	//接收网络RTSP流进行推流
	EasyPlayerManager m_netStreamCapture;
	//接收EasyDarwin推出的RTSP流进行播放
	EasyPlayerManager m_netStreamPlayer;

	EASY_MEDIA_INFO_T   m_mediainfo;
	CAMERA_LIST_T		*m_pCameraList;
	EASY_LOCAL_SOURCE_T m_sSourceInfo;
	EASY_LOCAL_SOURCE_T m_sPushInfo;
	EASY_LOCAL_SOURCE_T m_sPlayInfo;
	int			m_nLocalVideoWidth;
	int			m_nLocalVideoHeight;
	unsigned char	*rgbData;
	int				rgbDataSize;
	D3D_HANDLE		m_d3dHandle;
	HWND m_hCaptureWnd;
	HWND m_hPlayWnd;
	BOOL m_bPushing;
};

