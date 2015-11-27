/*
	Copyright (c) 2013-2014 EasyDarwin.ORG.  All rights reserved.
	Github: https://github.com/EasyDarwin
	WEChat: EasyDarwin
	Website: http://www.EasyDarwin.org
*/
// EasyDarwin推送和接收源端的管理类 [11/8/2015 Dingshuai]
// Add by SwordTwelve
#pragma once

#define EasyClent_VersionInfo _T("Version:1.1.1.3 Powered By SwordTwelve/Gavin/Arno")

//本地音频捕获
#include "AudioSource\DirectSound.h"
//本地视频捕获
#include "VideoSource\USBCamera\CameraDS.h"
//网络音视频流采集
#include "EasyPlayerManager.h"
//DShow音视频采集库头文件添加（添加该库主要是为了解决音视频源头上不同步的问题）
#include "./DShowCapture/DShowCaptureAudioAndVideo_Interface.h"
#pragma comment(lib, "./DShowCapture/DShowCaptureAudioAndVideoDll.lib")

#include "./FFEncoder/FFEncoderAPI.h"
#pragma comment(lib, "./FFEncoder/FFEncoder.lib")

#include "./Encoder/FAACEncoder.h"
#include "./Encoder/H264Encoder.h"
#include "./Encoder/H264EncoderManager.h"


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
	//DShow实时数据回调函数
	static int CALLBACK RealDataCallbackFunc(int nDevId, unsigned char *pBuffer, int nBufSize, 
																	   RealDataStreamType realDataType, /*RealDataStreamInfo*/void* realDataInfo, void* pMaster);
	void DSRealDataManager(int nDevId, unsigned char *pBuffer, int nBufSize, 
		RealDataStreamType realDataType, /*RealDataStreamInfo*/void* realDataInfo);

	static int CALLBACK __MediaSourceCallBack( int _channelId, int *_channelPtr, int _frameType, char *pBuf, RTSP_FRAME_INFO* _frameInfo);
	int SourceManager(int _channelId, int *_channelPtr, int _frameType, char *pBuf, RTSP_FRAME_INFO* _frameInfo);

	int StartDSCapture(int nCamId, int nAudioId,HWND hShowWnd, int nVideoWidth, int nVideoHeight, int nFps, int nBitRate);
	//开始捕获(采集)
	int StartCapture(SOURCE_TYPE eSourceType, int nCamId, int nAudioId,  HWND hCapWnd, 
		char* szURL = NULL, int nVideoWidth=640, int nVideoHeight=480, int nFps=25, int nBitRate=2048);
	//停止采集
	void StopCapture();

	//开始推流
	int StartPush(char* ServerIp, int nPushPort, char* sPushName, int nPushBufSize = 1024);
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
// 		BOOL bCap = FALSE;
// 		int nStreamCap = m_netStreamCapture.InRunning();
// 		int nVideoCap = m_videoCamera.InRunning();
// 		if (nVideoCap>0 ||nStreamCap>0 )
// 			bCap = TRUE;
// 		else
// 			bCap = FALSE;
		return m_bDSCapture;
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
	void ResizeVideoWnd();

protected:
		void	UpdateLocalVideo(unsigned char *pbuf, int size, int width, int height);

	//Member Var
private:
	CEasyClientDlg* m_pMainDlg;
	CDirectSound	m_audioCapture;
	CCameraDS		m_videoCamera;
	//视频设备控制实例
	LPVideoCapturer m_pVideoManager;
	//音频设备控制实例
	LPAudioCapturer m_pAudioManager;

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
	BOOL m_bAVSync;//音视频同步标志
	BOOL m_bDSCapture;

	//FF---编码器相关
	FFE_HANDLE m_hFfeVideoHandle;
	FFE_HANDLE m_hFfeAudioHandle;
	int m_nFrameNum;
	char * m_EncoderBuffer;// = new char[1920*1080];	//申请编码的内存空间

	//AAC编码器
	FAACEncoder m_AACEncoderManager;
	//H264编码器
	CH264EncoderManager m_H264EncoderManager;
	//编码信息配置
	Encoder_Config_Info*	m_pEncConfigInfo;
	byte m_sps[100];
	byte  m_pps[100];
	long m_spslen;
	long m_ppslen;
	byte* m_pFrameBuf; 
};

