/*
	Copyright (c) 2013-2014 EasyDarwin.ORG.  All rights reserved.
	Github: https://github.com/EasyDarwin
	WEChat: EasyDarwin
	Website: http://www.EasyDarwin.org
*/
#include "StdAfx.h"
#include "SourceManager.h"
#include "EasyClientDlg.h"

CSourceManager* CSourceManager::s_pSourceManager = NULL;

CSourceManager::CSourceManager(void)
{
	s_pSourceManager = NULL;
	m_bPushing = FALSE;
	m_sPushInfo.pusherHandle = 0;
}


CSourceManager::~CSourceManager(void)
{
}

//初始化唯一的管理实例
CSourceManager* CSourceManager::Instance()
{
	if (!s_pSourceManager)
	{
		s_pSourceManager = new CSourceManager();
		s_pSourceManager->InitSource();
	}
	return s_pSourceManager;
}

//销毁唯一的管理实例
 void CSourceManager::UnInstance()
{
	if (s_pSourceManager)
	{
		s_pSourceManager->RealseManager();

		delete s_pSourceManager;
		s_pSourceManager = NULL;
	}
}
//释放Master所占相关资源
void CSourceManager::RealseManager()
{
	StopPlay();
	StopPush();
	StopCapture();
	UnInitSource();

}

//执行视频源初始化操作
void CSourceManager::InitSource()
{
	RGB_InitDraw(&m_d3dHandle);
	EasyPlayerManager::Init();

}
//反初始化
void CSourceManager::UnInitSource()
{
	RGB_DeinitDraw(&m_d3dHandle);
	EasyPlayerManager::UnInit();
}

int CALLBACK CSourceManager::__MediaSourceCallBack( int _channelId, int *_channelPtr, int _frameType, char *pBuf, RTSP_FRAME_INFO* _frameInfo)
{
	if (s_pSourceManager)
	{
		s_pSourceManager->SourceManager(_channelId, _channelPtr, _frameType, pBuf, _frameInfo);
	}
	
	return 0;
}

int CSourceManager::SourceManager(int _channelId, int *_channelPtr, int _frameType, char *pBuf, RTSP_FRAME_INFO* _frameInfo)
{
		EASY_LOCAL_SOURCE_T *pLocalSource = (EASY_LOCAL_SOURCE_T *)_channelPtr;
	if (NULL == pLocalSource)		return 0;

	EASY_AV_Frame	frame;
	memset(&frame, 0x00, sizeof(EASY_AV_Frame));
	if (NULL != _frameInfo)
	{
		frame.pBuffer = (Easy_U8*)pBuf;
		frame.u32AVFrameLen = _frameInfo->length;
		frame.u32TimestampSec = _frameInfo->timestamp_sec;
		frame.u32TimestampUsec = _frameInfo->timestamp_usec;
		frame.u32VFrameType   = _frameInfo->type;
	}
	if (_frameType == EASY_SDK_VIDEO_RAW_RGB)
	{
		UpdateLocalVideo((unsigned char*)pBuf, _frameInfo->length, _frameInfo->width, _frameInfo->height);
	}
	else if (_frameType == EASY_SDK_VIDEO_FRAME_FLAG)
	{
		BOOL bIsPushing = IsInPushing();
		if(bIsPushing)
		{
			frame.u32AVFrameFlag = EASY_SDK_VIDEO_FRAME_FLAG;
			EasyPusher_PushFrame(m_sPushInfo.pusherHandle, &frame );
		}
	}
	else if (_frameType == EASY_SDK_AUDIO_FRAME_FLAG)
	{
		if(IsInPushing())
		{
			frame.u32AVFrameFlag = EASY_SDK_AUDIO_FRAME_FLAG;
			EasyPusher_PushFrame(m_sPushInfo.pusherHandle, &frame );
		}
	}
	else if (_frameType == EASY_SDK_MEDIA_INFO_FLAG)
	{
		memcpy_s(&m_mediainfo, sizeof(m_mediainfo), pBuf, sizeof(EASY_MEDIA_INFO_T));
		if (IsInPushing())
		{
			//停止推流
			StopPush();
			//开始推流
			StartPush(m_sPushInfo.pushServerAddr, m_sPushInfo.pushServerPort, m_sPushInfo.sdpName);

		}
// 		pLocalSource->pusherHandle = EasyPusher_Create();
// 		if (NULL != pLocalSource->pusherHandle)
// 		{
// 			EasyPusher_StartStream(pLocalSource->pusherHandle, pLocalSource->pushServerAddr, pLocalSource->pushServerPort, pLocalSource->sdpName+1, "", "", (EASY_MEDIA_INFO_T*)pBuf, 1024, 0);
// 		}
	}
	return 1;
}

//DShow采集视频显示接口
void CSourceManager::UpdateLocalVideo(unsigned char *pbuf, int size, int width, int height)
{
	RECT rcClient;
	SetRect(&rcClient, 0, 0, width, height);
	RGB_DrawData(m_d3dHandle, m_hCaptureWnd, (char*)pbuf, width, height, &rcClient, 0x00, RGB(0x00,0x00,0x00), 0x01);
}


//开始捕获(采集)
// eSourceType==SOURCE_LOCAL_CAMERA时，nCamId有效
// eSourceType==SOURCE_RTSP_STREAM/SOURCE_ONVIF_STREAM时，szURL有效
int CSourceManager::StartCapture(SOURCE_TYPE eSourceType, int nCamId, HWND hCapWnd, char* szURL)
{
	if (m_netStreamCapture.InRunning()>0 || m_videoCamera.InRunning()>0)
	{
		LogErr(_T("采集正在进行中..."));
		//StopCapture();
		return -1;
	}

		m_hCaptureWnd = hCapWnd;
	//RTSP Source
	if (eSourceType==SOURCE_LOCAL_CAMERA )
	{
		//USB Camera
		CAMERA_LIST_T *pUSBCameraList = m_videoCamera.GetCameraList();
		CAMERA_INFO_T	*pCameraInfo = pUSBCameraList->pCamera;
		if (NULL != pCameraInfo)
		{
			int selCameraIdx = nCamId;//pComboxMediaSource->GetCurSel();
			int cameraIdx = 0;
			while (pCameraInfo)
			{
				if (cameraIdx == selCameraIdx)
				{
					bool bSuc = m_videoCamera.OpenCamera(cameraIdx, 640, 480, (MediaSourceCallBack)CSourceManager::__MediaSourceCallBack, (void *)&m_sSourceInfo);
					if (!bSuc)
					{
						LogErr(_T("本地摄像头采集失败,请更换视频设备！"));
						return -1;
					}
					break;
				}
				cameraIdx ++;
				pCameraInfo = pCameraInfo->pNext;
			}

			//声音捕获
			BOOL bSuc = m_audioCapture.InitDirectSound();
			if (!bSuc)
			{
				LogErr(_T("本地音频采集初始化失败，请更换音频设备！"));
				return -1;
			}
			m_audioCapture.SetCallback(&CSourceManager::__MediaSourceCallBack, (void *)&m_sSourceInfo);
			WAVEFORMATEX	wfx;
			ZeroMemory(&wfx, sizeof(wfx));
			wfx.wFormatTag = WAVE_FORMAT_PCM;
			wfx.nSamplesPerSec = 16000;	//16000   8000
			wfx.wBitsPerSample = 16;	//16	  8
			wfx.nChannels      = 2;		//1		  2
			wfx.nBlockAlign  = wfx.nChannels * (wfx.wBitsPerSample / 8);
			wfx.nAvgBytesPerSec = wfx.nBlockAlign * wfx.nSamplesPerSec;

			m_audioCapture.CreateCaptureBuffer(&wfx);
			m_audioCapture.Start();
		}
	}
	else
	{
		memset(m_sSourceInfo.pushServerAddr, 0x00, sizeof(m_sSourceInfo.pushServerAddr));
		m_sSourceInfo.pushServerPort = 0;
		memset(m_sSourceInfo.sdpName, 0x00, sizeof(m_sSourceInfo.sdpName));
		char	pushHead[128]  = {0,};
		strcpy(pushHead,szURL+7);//strlen(szURL)
		sscanf(pushHead, "%[-_a-zA-Z0-9.]:%d%[-_a-zA-Z0-9:/.]", m_sSourceInfo.pushServerAddr, &m_sSourceInfo.pushServerPort, m_sSourceInfo.sdpName);
		if (m_sSourceInfo.pushServerPort < 1)
		{
			LogErr(_T("网络视频流采集，格式输入不合法！"));

			return -1;
		}
		if ( (int)strlen(m_sSourceInfo.sdpName) < 1)
		{
			LogErr(_T("网络视频流采集，格式输入不合法！"));
			return -1;
		}

		m_sSourceInfo.rtspSourceId = m_netStreamCapture.Start(szURL, hCapWnd, DISPLAY_FORMAT_RGB24_GDI, 0x00, "", "", &CSourceManager::__MediaSourceCallBack, (void *)&m_sSourceInfo);
		m_netStreamCapture.Config(3, FALSE, FALSE);
		if (m_sSourceInfo.rtspSourceId<=0)
		{
			LogErr(_T("网络视频流采集失败！"));
			return -1;
		}		
	}
	return 1;
}
//停止采集
void CSourceManager::StopCapture()
{
	//Stop
	m_videoCamera.CloseCamera();
	m_audioCapture.Stop();

	m_netStreamCapture.Close();

}
//开始推流
int CSourceManager::StartPush(char* ServerIp, int nPushPort, char* sPushName)
{
		m_sPushInfo.pusherHandle = EasyPusher_Create();
		strcpy(m_sPushInfo.pushServerAddr,  ServerIp);
		m_sPushInfo.pushServerPort = nPushPort;
		strcpy(m_sPushInfo.sdpName, sPushName);
		Easy_U32 nRet = 0;
		if (NULL != m_sPushInfo.pusherHandle )
		{
			Easy_U32 nRet = EasyPusher_StartStream(m_sPushInfo.pusherHandle , ServerIp, nPushPort, sPushName, "", "", (EASY_MEDIA_INFO_T*)&m_mediainfo, 1024, 0);
			if(nRet>=0)
			{
				m_bPushing = TRUE;
			}
			else
			{
				StopPush();
			}
		}
		return nRet;
}
//停止推流
void CSourceManager::StopPush()
{
	//Close Pusher
	if (NULL != m_sPushInfo.pusherHandle)
	{
		EasyPusher_StopStream(m_sPushInfo.pusherHandle);
		EasyPusher_Release(m_sPushInfo.pusherHandle);
		m_sPushInfo.pusherHandle = NULL;
	}
	m_bPushing = FALSE;
}

//开始播放
int CSourceManager::StartPlay(char* szURL, HWND hShowWnd)
{
	m_sPlayInfo.rtspSourceId = m_netStreamPlayer.Start(szURL, hShowWnd, DISPLAY_FORMAT_RGB24_GDI, 0x00, "", "");
	m_netStreamPlayer.Config(3, TRUE, TRUE);
	return m_sPlayInfo.rtspSourceId ;
}
//停止播放
void CSourceManager::StopPlay()
{
	m_netStreamPlayer.Close();
}

void CSourceManager::LogErr(CString strLog)
{
	if(!strLog.IsEmpty())
	{
		TCHAR* szLog = new TCHAR[strLog.GetLength()+1];
		StrCpy(szLog, strLog);
		if(m_pMainDlg)
			m_pMainDlg->SendMessage(MSG_LOG, 0, (LPARAM)szLog);

		delete[] szLog;
		szLog = NULL;
	}
}

void CSourceManager::SetMainDlg(	CEasyClientDlg* pMainDlg)
{
	m_pMainDlg = pMainDlg;
}

void CSourceManager::EnumLocalAVDevInfo(CWnd* pComboxMediaSource, CWnd* pComboxAudioSource)
{
	if (NULL != pComboxMediaSource)
	{
		CComboBox * pVideoSource = (CComboBox *)pComboxMediaSource;
		CAMERA_LIST_T *pUSBCameraList = m_videoCamera.GetCameraList();
		CAMERA_INFO_T	*pCameraInfo = pUSBCameraList->pCamera;
		if (NULL != pCameraInfo)
		{
			while (pCameraInfo)
			{
				wchar_t wszCameraName[64] = {0,};
				__MByteToWChar(pCameraInfo->friendlyName, wszCameraName, sizeof(wszCameraName)/sizeof(wszCameraName[0]));

				pVideoSource->AddString(wszCameraName);

				pCameraInfo = pCameraInfo->pNext;
			}
		}
		pVideoSource->SetCurSel(0);
	}

	if (NULL != pComboxAudioSource)
	{
		m_audioCapture.GetAudioInputDevices(pComboxAudioSource->GetSafeHwnd());
	}

}