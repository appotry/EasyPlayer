/*
	Copyright (c) 2013-2014 EasyDarwin.ORG.  All rights reserved.
	Github: https://github.com/EasyDarwin
	WEChat: EasyDarwin
	Website: http://www.EasyDarwin.org
*/
#include "StdAfx.h"
#include "SourceManager.h"
#include "EasyClientDlg.h"
#include "YUVTransform.h"

CSourceManager* CSourceManager::s_pSourceManager = NULL;

CSourceManager::CSourceManager(void)
{
	s_pSourceManager = NULL;
	m_bPushing = FALSE;
	m_sPushInfo.pusherHandle = 0;
	m_bAVSync = FALSE;
	//视频设备控制实例
	m_pVideoManager = NULL;
	//音频设备控制实例
	m_pAudioManager = NULL;
	m_bDSCapture = FALSE;
	m_hFfeVideoHandle = NULL;
	m_hFfeAudioHandle = NULL;
	m_nFrameNum = 0;
	m_EncoderBuffer = NULL;
	m_pEncConfigInfo = NULL;
	m_pFrameBuf = NULL;
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

//实时数据回调函数
int  CSourceManager::RealDataCallbackFunc(int nDevId, unsigned char *pBuffer, int nBufSize, 
																		RealDataStreamType realDataType, /*RealDataStreamInfo*/void* realDataInfo, void* pMaster)
{

	if (!pBuffer || nBufSize <= 0)
	{
		return -1;
	}
	CSourceManager* pThis = (CSourceManager*)pMaster;
	if (pThis)
	{
		pThis->DSRealDataManager(nDevId, pBuffer, nBufSize, realDataType, realDataInfo);
	}

	return 0;
}

void CSourceManager::DSRealDataManager(int nDevId, unsigned char *pBuffer, int nBufSize, 
										RealDataStreamType realDataType, /*RealDataStreamInfo*/void* realDataInfo)
{
	switch (realDataType)
	{
	case REALDATA_VIDEO:
		{
			//if (m_hFfeVideoHandle)
			if (m_bPushing)
			{
				//YUV格式转换
				int nVideoWidth = 640;
				int	nVideoHeight = 480;
				int nWidhtHeightBuf=(nVideoWidth*nVideoHeight*3)>>1;
				BYTE* pDataBuffer=new unsigned char[nWidhtHeightBuf];

				YUY2toI420(nVideoWidth,nVideoHeight,pBuffer, pDataBuffer);
// 				//编码
// 				int enc_size = 0;
// 				int ret = FFE_EncodeVideo(m_hFfeVideoHandle, pDataBuffer, (unsigned char*)m_EncoderBuffer, &enc_size, ++m_nFrameNum, 0);
// 				if (ret == 0x00 && enc_size>0)
// 				{
// 					RTSP_FRAME_INFO	frameinfo;
// 					memset(&frameinfo, 0x00, sizeof(RTSP_FRAME_INFO));
// 					frameinfo.codec = EASY_SDK_VIDEO_CODEC_H264;
// 					frameinfo.length = enc_size;
// 					frameinfo.width	 = 640;
// 					frameinfo.height = 480;
// 					frameinfo.fps    = 25;
// 					frameinfo.type	 = ( (unsigned char)m_EncoderBuffer[4]==0x67?EASY_SDK_VIDEO_FRAME_I:EASY_SDK_VIDEO_FRAME_P);
// 					long nTimeStamp = clock();
// 					frameinfo.timestamp_sec = nTimeStamp/1000;
// 					frameinfo.timestamp_usec = (nTimeStamp%1000)*1000;
// 
// 					//推送回调管理
// 					SourceManager(nDevId, (int*)&m_sSourceInfo, EASY_SDK_VIDEO_FRAME_FLAG, m_EncoderBuffer, &frameinfo);
// 				}

				//编码实现
				byte*pdata=NULL;
				int datasize=0;
				bool keyframe=false;
				pdata=m_H264EncoderManager.Encoder(0, pDataBuffer,
					nWidhtHeightBuf,datasize,keyframe);

				if (pDataBuffer)
				{
					delete pDataBuffer;
					pDataBuffer = NULL;
				}
				if (datasize>0)
				{
					memset(m_pFrameBuf, 0, 1024*512);

					RTSP_FRAME_INFO	frameinfo;
					memset(&frameinfo, 0x00, sizeof(RTSP_FRAME_INFO));
					frameinfo.codec = EASY_SDK_VIDEO_CODEC_H264;
					frameinfo.width	 = 640;
					frameinfo.height = 480;
					frameinfo.fps    = 25;

					bool bKeyF = keyframe;
					byte btHeader[4];
					btHeader[0] = 0x00;
					btHeader[1] = 0x00;
					btHeader[2] = 0x00;
					btHeader[3] = 0x01;
					if (bKeyF)
					{
						frameinfo.length = datasize + 8 + m_spslen+m_ppslen;
						memcpy(m_pFrameBuf, btHeader, 4);
						memcpy(m_pFrameBuf+4, m_sps, m_spslen);
						memcpy(m_pFrameBuf+4+m_spslen, btHeader, 4);
						memcpy(m_pFrameBuf+4+m_spslen+4, m_pps, m_ppslen);
						memcpy(m_pFrameBuf+4+m_spslen+4+m_ppslen, btHeader, 4);
						memcpy(m_pFrameBuf+4+m_spslen+4+m_ppslen+4, pdata+4, datasize-4);
					} 
					else
					{
						frameinfo.length = datasize;
						memcpy(m_pFrameBuf, btHeader, 4);
						memcpy(m_pFrameBuf+4, pdata+4, datasize-4);
					}
					frameinfo.type	 = ( (bKeyF)?EASY_SDK_VIDEO_FRAME_I:EASY_SDK_VIDEO_FRAME_P);
					long nTimeStamp = clock();
					frameinfo.timestamp_sec = nTimeStamp/1000;
					frameinfo.timestamp_usec = (nTimeStamp%1000)*1000;

					//推送回调管理
					SourceManager(nDevId, (int*)&m_sSourceInfo, EASY_SDK_VIDEO_FRAME_FLAG, (char*)m_pFrameBuf, &frameinfo);
				}

				if (pDataBuffer)
				{
					delete[] pDataBuffer;
					pDataBuffer = NULL;
				}			
			}
		}
		break;
	case REALDATA_AUDIO:
		{
// 			if (m_hFfeAudioHandle)
// 			{
// 				//音频编码
// 				unsigned char *pAACbuf= NULL;
// 				int enc_size = 0;
// 				int ret = AAC_Encode(m_hFfeAudioHandle, (int*)pBuffer, nBufSize, &pAACbuf, &enc_size);
// 				if (ret == 0x00 && enc_size>0)
// 				{
// 					RTSP_FRAME_INFO	frameinfo;
// 					memset(&frameinfo, 0x00, sizeof(RTSP_FRAME_INFO));
// 					frameinfo.codec = EASY_SDK_AUDIO_CODEC_AAC;
// 					frameinfo.length = enc_size;
// 					frameinfo.sample_rate	=	16000;
// 					frameinfo.channels = 2;
// 
// 					long nTimeStamp = clock();
// 					frameinfo.timestamp_sec = nTimeStamp/1000;
// 					frameinfo.timestamp_usec = (nTimeStamp%1000)*1000;
// 					SourceManager(nDevId, (int*)&m_sSourceInfo, EASY_SDK_AUDIO_FRAME_FLAG, (char*)pAACbuf, &frameinfo);
// 				}	

			if (m_bPushing)
			{
				byte*pdata=NULL;
				int datasize=0;
				pdata=m_AACEncoderManager.Encoder(pBuffer,nBufSize,datasize);	
				if(datasize>0)
				{
					RTSP_FRAME_INFO	frameinfo;
					memset(&frameinfo, 0x00, sizeof(RTSP_FRAME_INFO));
					frameinfo.codec = EASY_SDK_AUDIO_CODEC_AAC;
					frameinfo.length = datasize;
					frameinfo.sample_rate	=	16000;
					frameinfo.channels = 2;

					long nTimeStamp = clock();
					frameinfo.timestamp_sec = nTimeStamp/1000;
					frameinfo.timestamp_usec = (nTimeStamp%1000)*1000;

					SourceManager(nDevId, (int*)&m_sSourceInfo, EASY_SDK_AUDIO_FRAME_FLAG, (char*)pdata, &frameinfo);
				}		
			}

		}
		break;
	}
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
		if(bIsPushing /*&& m_bAVSync*/)
		{
			frame.u32AVFrameFlag = EASY_SDK_VIDEO_FRAME_FLAG;
			EasyPusher_PushFrame(m_sPushInfo.pusherHandle, &frame );
		}
	}
	else if (_frameType == EASY_SDK_AUDIO_FRAME_FLAG)
	{
		if(IsInPushing())
		{
			//m_bAVSync = TRUE;//音频同步视频

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

int CSourceManager::StartDSCapture(int nCamId, int nAudioId,HWND hShowWnd)
{
	if (m_bDSCapture)
	{
		return 0;
	}

	if(!m_pVideoManager)
	{
		m_pVideoManager = Create_VideoCapturer();
	}
	if(!m_pAudioManager)
	{
		m_pAudioManager = Create_AudioCapturer();
	}
	//设备连接配置信息结构
	DEVICE_CONFIG_INFO DevConfigInfo;
	memset(&DevConfigInfo, 0x0, sizeof(DevConfigInfo));

	BOOL bUseThread = FALSE;
	int nRet = 0;
	CString strTemp = _T("");
	//连接设备
	// 理论上这里应该有个配置来配置设备的信息 [11/3/2015-13:21:06 Dingshuai]
	//GetDevInfoByDevIndex(nI, &DevConfigInfo);

		//1. 我们来简单配置一个设备信息
		DevConfigInfo.nDeviceId = 1;
		DevConfigInfo.nVideoId = nCamId;//摄像机视频捕获ID
		DevConfigInfo.nAudioId = nAudioId;//音频捕获ID
		DevConfigInfo.VideoInfo.nFrameRate = 25;
		DevConfigInfo.VideoInfo.nHeight = 480;
		DevConfigInfo.VideoInfo.nWidth = 640;
		strcpy_s(DevConfigInfo.VideoInfo.strDataType, 64, "YUY2");
		DevConfigInfo.VideoInfo.nRenderType = 1;
		DevConfigInfo.VideoInfo.nPinType = 1;
		DevConfigInfo.VideoInfo.nVideoWndId = 0;

		DevConfigInfo.AudioInfo.nAudioBufferType = 4096;
		DevConfigInfo.AudioInfo.nBytesPerSample = 16;
		DevConfigInfo.AudioInfo.nSampleRate = 16000;//44100;
		DevConfigInfo.AudioInfo.nChannaels = 2;
		DevConfigInfo.AudioInfo.nPinType = 2;

		//初始化Pusher结构信息
		memset(&m_mediainfo, 0x00, sizeof(EASY_MEDIA_INFO_T));
		m_mediainfo.u32VideoCodec =  EASY_SDK_VIDEO_CODEC_H264;//0x1C;
		m_mediainfo.u32VideoFps = 25;
		m_mediainfo.u32AudioCodec = EASY_SDK_AUDIO_CODEC_AAC;
		m_mediainfo.u32AudioChannel = 2;
		m_mediainfo.u32AudioSamplerate = 16000;//44100;


		//视频可用
		if (DevConfigInfo.nVideoId >= 0)
		{
			HWND hWnd = hShowWnd;		
			// 2.设置视频获取显示参数
			m_pVideoManager->SetVideoCaptureData(0, DevConfigInfo.nVideoId,
				hWnd,
				DevConfigInfo.VideoInfo.nFrameRate,  DevConfigInfo.VideoInfo.nWidth,
				DevConfigInfo.VideoInfo.nHeight,     DevConfigInfo.VideoInfo.strDataType, 
				DevConfigInfo.VideoInfo.nRenderType, DevConfigInfo.VideoInfo.nPinType, 1, bUseThread);

			m_pVideoManager->SetDShowCaptureCallback((RealDataCallback)(CSourceManager::RealDataCallbackFunc), (void*)s_pSourceManager);

			// 3.创建获取视频的图像
			nRet =m_pVideoManager->CreateCaptureGraph();
			if(nRet<=0)
			{
				m_pVideoManager->SetCaptureVideoErr(nRet);
				Release_VideoCapturer(m_pVideoManager);
				m_pVideoManager = NULL;

				strTemp.Format(_T("Video[%d]--创建基本链路失败--In StartDSCapture()."), nCamId);
				LogErr(strTemp);
				return -1;
			}
		}

		if (m_pVideoManager)
		{
			nRet = m_pVideoManager->BulidPrivewGraph();
			if(nRet<0)
			{
				Release_VideoCapturer(m_pVideoManager);
				m_pVideoManager = NULL;

				strTemp.Format(_T("Video[%d]--连接链路失败--In StartDSCapture()."), nCamId);			
				LogErr(strTemp);
				return -1;
			}
			else
			{
				m_pVideoManager->BegineCaptureThread();
			}
		}

	//音频可用
	if (DevConfigInfo.nAudioId >= 0)
	{
		m_pAudioManager->SetAudioCaptureData(DevConfigInfo.nAudioId, DevConfigInfo.AudioInfo.nChannaels, 
			DevConfigInfo.AudioInfo.nBytesPerSample,  DevConfigInfo.AudioInfo.nSampleRate, 
			DevConfigInfo.AudioInfo.nAudioBufferType, DevConfigInfo.AudioInfo.nPinType, 2, bUseThread);

		m_pAudioManager->SetDShowCaptureCallback((RealDataCallback)(CSourceManager::RealDataCallbackFunc), (void*)s_pSourceManager);

		nRet =m_pAudioManager->CreateCaptureGraph();
		if(nRet<=0)
		{
			strTemp.Format(_T("Audio[%d]--创建基本链路失败--In StartDSCapture()."), nAudioId);
			LogErr(strTemp);

			Release_AudioCapturer(m_pAudioManager);
			m_pAudioManager = NULL;
			return -2;
		}
		if (m_pAudioManager)
		{
			nRet = m_pAudioManager->BulidCaptureGraph();
			if(nRet<0)
			{
				strTemp.Format(_T("Audio[%d]--连接链路失败--In StartDSCapture()."), nAudioId);
				LogErr(strTemp);

				Release_AudioCapturer(m_pAudioManager);
				m_pAudioManager = NULL;
				return -2;
			}
			else
			{
				m_pAudioManager->BegineCaptureThread();	
			}
		}
	}	
	return nRet;
}

//开始捕获(采集)
// eSourceType==SOURCE_LOCAL_CAMERA时，nCamId有效
// eSourceType==SOURCE_RTSP_STREAM/SOURCE_ONVIF_STREAM时，szURL有效
int CSourceManager::StartCapture(SOURCE_TYPE eSourceType, int nCamId, int nAudioId, HWND hCapWnd, char* szURL)
{
	if (IsInCapture())
	{
		LogErr(_T("采集正在进行中..."));
		//StopCapture();
		return -1;
	}
	int nRet = 1;
	m_hCaptureWnd = hCapWnd;
	//RTSP Source
	if (eSourceType==SOURCE_LOCAL_CAMERA )
	{
		// 经测试，音视频采集时出现不同步 [11/19/2015 SwordTwelve]
// 		//声音捕获
// 		BOOL bSuc = m_audioCapture.InitDirectSound();
// 		if (!bSuc)
// 		{
// 			LogErr(_T("本地音频采集初始化失败，请更换音频设备！"));
// 			return -1;
// 		}
// 		m_audioCapture.SetCallback(&CSourceManager::__MediaSourceCallBack, (void *)&m_sSourceInfo);
// 		WAVEFORMATEX	wfx;
// 		ZeroMemory(&wfx, sizeof(wfx));
// 		wfx.wFormatTag = WAVE_FORMAT_PCM;
// 		wfx.nSamplesPerSec = 16000;	//16000   8000
// 		wfx.wBitsPerSample = 16;	//16	  8
// 		wfx.nChannels      = 2;		//1		  2
// 		wfx.nBlockAlign  = wfx.nChannels * (wfx.wBitsPerSample / 8);
// 		wfx.nAvgBytesPerSec = wfx.nBlockAlign * wfx.nSamplesPerSec;
// 
// 		m_audioCapture.CreateCaptureBuffer(&wfx);
// 		m_audioCapture.Start();
// 
// 		//USB Camera捕获
// 		CAMERA_LIST_T *pUSBCameraList = m_videoCamera.GetCameraList();
// 		CAMERA_INFO_T	*pCameraInfo = pUSBCameraList->pCamera;
// 		if (NULL != pCameraInfo)
// 		{
// 			int selCameraIdx = nCamId;//pComboxMediaSource->GetCurSel();
// 			int cameraIdx = 0;
// 			while (pCameraInfo)
// 			{
// 				if (cameraIdx == selCameraIdx)
// 				{
// 					bool bSuc = m_videoCamera.OpenCamera(cameraIdx, 640, 480, (MediaSourceCallBack)CSourceManager::__MediaSourceCallBack, (void *)&m_sSourceInfo);
// 					if (!bSuc)
// 					{
// 						LogErr(_T("本地摄像头采集失败,请更换视频设备！"));
// 						return -1;
// 					}
// 					break;
// 				}
// 				cameraIdx ++;
// 				pCameraInfo = pCameraInfo->pNext;
// 			}
// 		}
		//DShow本地采集
		nRet = StartDSCapture(nCamId, nAudioId, m_hCaptureWnd);	
	}
	else
	{
		memset(m_sSourceInfo.pushServerAddr, 0x00, sizeof(m_sSourceInfo.pushServerAddr));
		m_sSourceInfo.pushServerPort = 0;
		memset(m_sSourceInfo.sdpName, 0x00, sizeof(m_sSourceInfo.sdpName));
		char	pushHead[128]  = {0,};
		strcpy(pushHead,szURL+7);//strlen(szURL)
		sscanf(pushHead, "%[-_a-zA-Z0-9.]:%d%[-_a-zA-Z0-9:/.]", m_sSourceInfo.pushServerAddr, &m_sSourceInfo.pushServerPort, m_sSourceInfo.sdpName);
		// 这里不作为错误判断 [11/10/2015 Administrator]
// 		if (m_sSourceInfo.pushServerPort < 1)
// 		{
// 			LogErr(_T("网络视频流采集，格式输入不合法！"));
// 
// 			return -1;
// 		}
// 		if ( (int)strlen(m_sSourceInfo.sdpName) < 1)
// 		{
// 			LogErr(_T("网络视频流采集，格式输入不合法！"));
// 			return -1;
// 		}

		m_sSourceInfo.rtspSourceId = m_netStreamCapture.Start(szURL, hCapWnd, DISPLAY_FORMAT_RGB24_GDI, 0x00, "", "", &CSourceManager::__MediaSourceCallBack, (void *)&m_sSourceInfo);
		m_netStreamCapture.Config(3, FALSE, FALSE);
		if (m_sSourceInfo.rtspSourceId<=0)
		{
			LogErr(_T("网络视频流采集失败！"));
			return -1;
		}		
	}
	m_bDSCapture = TRUE;

	return nRet;
}
//停止采集
void CSourceManager::StopCapture()
{
	//Stop Capture
// 	m_videoCamera.CloseCamera();
// 	m_audioCapture.Stop();
	//清除窗口关联设备
	if (m_pVideoManager)
	{
		Release_VideoCapturer(m_pVideoManager);
		m_pVideoManager = NULL;
	}
	if (m_pAudioManager)
	{
		Release_AudioCapturer(m_pAudioManager);
		m_pAudioManager = NULL;
	}
	m_netStreamCapture.Close();
	
	m_bDSCapture = FALSE;
}

int __EasyPusher_Callback(int _id, EASY_PUSH_STATE_T _state, EASY_AV_Frame *_frame, void *_userptr)
{
	if (_state == EASY_PUSH_STATE_CONNECTING)               TRACE("Connecting...\n");
	else if (_state == EASY_PUSH_STATE_CONNECTED)           TRACE("Connected\n");
	else if (_state == EASY_PUSH_STATE_CONNECT_FAILED)      TRACE("Connect failed\n");
	else if (_state == EASY_PUSH_STATE_CONNECT_ABORT)       TRACE("Connect abort\n");
	else if (_state == EASY_PUSH_STATE_PUSHING)             TRACE("P->");
	else if (_state == EASY_PUSH_STATE_DISCONNECTED)        TRACE("Disconnect.\n");

	return 0;
}

//开始推流
int CSourceManager::StartPush(char* ServerIp, int nPushPort, char* sPushName)
{
	// 	int	width = 640;
	// 	int height = 480;
	// 	int fps = 25;
	// 	int gop = 30;
	// 	int bitrate = 512000;
	// 	int	intputformat = 0;		//3:rgb24  0:yv12
	// 
	// 	m_nFrameNum = 0;
	// 	if (!m_EncoderBuffer)
	// 	{
	// 		m_EncoderBuffer = new char[1920*1080];	//申请编码的内存空间
	// 	}	
	// 	//初始化H264编码器
	// 	FFE_Init(&m_hFfeVideoHandle);	//初始化
	// 	FFE_SetVideoEncodeParam(m_hFfeVideoHandle, ENCODER_H264, width, height, fps, gop, bitrate, intputformat);		//设置编码参数
	// 	//初始化AAC编码器
	// 	AAC_Init(&m_hFfeAudioHandle, 16000, 2);

	if(!m_pEncConfigInfo)
		m_pEncConfigInfo = new Encoder_Config_Info;

	m_AACEncoderManager.Init();
	m_pEncConfigInfo->nScrVideoWidth = 640;
	m_pEncConfigInfo->nScrVideoHeight = 480;
	m_pEncConfigInfo->nFps = 25;
	m_pEncConfigInfo->nMainKeyFrame = 50;
	m_pEncConfigInfo->nMainBitRate = 2048;
	m_pEncConfigInfo->nMainEncLevel = 1;
	m_pEncConfigInfo->nMainEncQuality = 20;
	m_pEncConfigInfo->nMainUseQuality = 0;

	m_H264EncoderManager.Init(0,m_pEncConfigInfo->nScrVideoWidth,
		m_pEncConfigInfo->nScrVideoHeight,m_pEncConfigInfo->nFps,m_pEncConfigInfo->nMainKeyFrame,
		m_pEncConfigInfo->nMainBitRate,m_pEncConfigInfo->nMainEncLevel,
		m_pEncConfigInfo->nMainEncQuality,m_pEncConfigInfo->nMainUseQuality);

	byte  sps[100];
	byte  pps[100];
	long spslen=0;
	long ppslen=0;
	m_H264EncoderManager.GetSPSAndPPS(0,sps,spslen,pps,ppslen);
	memcpy(m_sps, sps,100) ;
	memcpy(m_pps, pps,100) ;
	m_spslen = spslen;
	m_ppslen = ppslen;
	m_pFrameBuf	= new byte[1024*512];

		m_sPushInfo.pusherHandle = EasyPusher_Create();
		strcpy(m_sPushInfo.pushServerAddr,  ServerIp);
		m_sPushInfo.pushServerPort = nPushPort;
		strcpy(m_sPushInfo.sdpName, sPushName);
		Easy_U32 nRet = 0;
		if (NULL != m_sPushInfo.pusherHandle )
		{
			EasyPusher_SetEventCallback(m_sPushInfo.pusherHandle, __EasyPusher_Callback, 0, NULL);
			Easy_U32 nRet = EasyPusher_StartStream(m_sPushInfo.pusherHandle , ServerIp, nPushPort, sPushName, "admin", "admin", (EASY_MEDIA_INFO_T*)&m_mediainfo, 1024, 0);
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
	m_bAVSync = FALSE;

	// 	if (m_hFfeVideoHandle)
	// 	{
	// 		FFE_Deinit(&m_hFfeVideoHandle);
	// 		m_hFfeVideoHandle = NULL;
	// 	}
	// 	if (m_hFfeAudioHandle)
	// 	{
	// 		AAC_Deinit(&m_hFfeAudioHandle);
	// 		m_hFfeAudioHandle = NULL;
	// 	}
	// 
	// 	m_nFrameNum = 0;
	// 	if (m_EncoderBuffer)
	// 	{
	// 		delete[] m_EncoderBuffer ;	//申请编码的内存空间
	// 		m_EncoderBuffer = NULL;
	// 	}	

	if (m_pEncConfigInfo)
	{
		delete m_pEncConfigInfo;
		m_pEncConfigInfo = NULL;
	}
	m_H264EncoderManager.Clean();
	m_AACEncoderManager.Clean();

	if (m_pFrameBuf)
	{
		delete[] m_pFrameBuf;
		m_pFrameBuf = NULL;
	}
}

//开始播放
int CSourceManager::StartPlay(char* szURL, HWND hShowWnd)
{
	m_sPlayInfo.rtspSourceId = m_netStreamPlayer.Start(szURL, hShowWnd, DISPLAY_FORMAT_RGB24_GDI, 0x01, "", "");
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

void CSourceManager::ResizeVideoWnd()
{
	if (m_pVideoManager)
	{
		m_pVideoManager->ResizeVideoWindow();
	}
}