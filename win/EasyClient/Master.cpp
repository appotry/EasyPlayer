#include "StdAfx.h"
#include "Master.h"
#include "EasyClientDlg.h"

CMaster* CMaster::s_pMaster = NULL;

CMaster::CMaster(CEasyClientDlg* pMainDlg)
{
	s_pMaster = NULL;
	m_nDevCount = MAX_DEV;
	//视频设备控制实例
	for(int nI=0; nI<MAX_DEV; nI++)
	{
		m_pVideoManager[nI] = NULL;
	}
	//音频设备控制实例
	m_pAudioManager = NULL;
	m_pMainDlg = NULL;
	m_pFrameBuf = NULL;
	m_pEncConfigInfo = NULL;
	m_bStartPusher = FALSE;
}

CMaster::~CMaster(void)
{
}

CMaster* CMaster::Instance(CEasyClientDlg* pMainDlg)
{
	if (!s_pMaster)
	{
		s_pMaster = new CMaster(pMainDlg);
	}
	return s_pMaster;
}

void CMaster::UnInstance()
{
	s_pMaster->RealseMaster();
	if (s_pMaster)
	{
		delete s_pMaster;
		s_pMaster = NULL;
	}
}

//释放Master所占相关资源
void CMaster::RealseMaster()
{
	UnInitEncoderAndPusher();
	UnInitDSCapture();
}

void CMaster::InitDSCapture()
{
	if (m_nDevCount <= 0 || m_nDevCount > MAX_DEV)
	{
		m_nDevCount = MAX_DEV;
	}

	for (int nI = 0; nI < m_nDevCount; nI++)
	{
		if(!m_pVideoManager[nI])
		{
			//m_pVideoManager[nI] = new CCaptureVideo();
			m_pVideoManager[nI] = Create_VideoCapturer();
		}

	}
	if(!m_pAudioManager)
	{
		//		m_pAudioManager = new CCaptureAudio();
		m_pAudioManager = Create_AudioCapturer();
	}
}

void CMaster::UnInitDSCapture()
{
	//清除窗口关联设备
	for(int nI = 0; nI < m_nDevCount; nI++)
	{
		if (m_pVideoManager[nI])
		{
			Release_VideoCapturer(m_pVideoManager[nI]);
			m_pVideoManager[nI] = NULL;
		}
	}

	if (m_pAudioManager)
	{
		Release_AudioCapturer(m_pAudioManager);
		m_pAudioManager = NULL;
	}
}

int CMaster::StartDSCapture(HWND hShowWnd[], int nWndNum)
{
	//设备连接配置信息结构
	DEVICE_CONFIG_INFO DevConfigInfo;
	memset(&DevConfigInfo, 0x0, sizeof(DevConfigInfo));
	
	m_nDevCount = nWndNum;
	BOOL bUseThread = FALSE;
	int nRet = 0;
	CString strTemp = _T("");
	//连接设备
	for (int nI = 0; nI < m_nDevCount; nI++)
	{
		memset(&DevConfigInfo, 0x0, sizeof(DevConfigInfo));
		// 理论上这里应该有个配置来配置设备的信息 [11/3/2015-13:21:06 Dingshuai]
		//GetDevInfoByDevIndex(nI, &DevConfigInfo);
			
		//我们来简单配置一个设备信息
		DevConfigInfo.nDeviceId = 1;
		DevConfigInfo.nVideoId = 0;
		DevConfigInfo.nAudioId = 0;
		DevConfigInfo.VideoInfo.nFrameRate = 25;
		DevConfigInfo.VideoInfo.nHeight = 480;
		DevConfigInfo.VideoInfo.nWidth = 640;
		_tcscpy_s(DevConfigInfo.VideoInfo.strDataType, MAXNAMESIZE, _T("YUY2"));
		DevConfigInfo.VideoInfo.nRenderType = 1;
		DevConfigInfo.VideoInfo.nPinType = 1;
		DevConfigInfo.VideoInfo.nVideoWndId = nI;

		DevConfigInfo.AudioInfo.nAudioBufferType = 4096;
		DevConfigInfo.AudioInfo.nBytesPerSample = 16;
		DevConfigInfo.AudioInfo.nSampleRate = 44100;
		DevConfigInfo.AudioInfo.nChannaels = 2;
		DevConfigInfo.AudioInfo.nPinType = 2;

		//视频可用
		if (DevConfigInfo.nVideoId >= 0)
		{

			HWND hWnd = hShowWnd[nI];		

			// 2.设置视频获取显示参数
			m_pVideoManager[nI]->SetVideoCaptureData(nI, DevConfigInfo.nVideoId,
				hWnd,
				DevConfigInfo.VideoInfo.nFrameRate,  DevConfigInfo.VideoInfo.nWidth,
				DevConfigInfo.VideoInfo.nHeight,     DevConfigInfo.VideoInfo.strDataType, 
				DevConfigInfo.VideoInfo.nRenderType, DevConfigInfo.VideoInfo.nPinType, 1, bUseThread);

			m_pVideoManager[nI]->SetDShowCaptureCallback((RealDataCallback)(CMaster::RealDataCallbackFunc), (void*)s_pMaster);

			// 3.创建获取视频的图像
			nRet =m_pVideoManager[nI]->CreateCaptureGraph();
			if(nRet<=0)
			{
				strTemp.Format(_T("Video[%d]--创建基本链路失败--In StartDSCapture()."), nI);
				AfxMessageBox(strTemp);
				m_pVideoManager[nI]->SetCaptureVideoErr(nRet);
				//delete m_pVideoManager[nI];
				Release_VideoCapturer(m_pVideoManager[nI]);
				m_pVideoManager[nI] = NULL;
				continue;
				//return -1;
			}
		}

		if (m_pVideoManager[nI])
		{
			nRet = m_pVideoManager[nI]->BulidPrivewGraph();
			if(nRet<0)
			{
				strTemp.Format(_T("Video[%d]--连接链路失败--In StartDSCapture()."), nI);			
				AfxMessageBox(strTemp);
				//delete m_pVideoManager[nI];
				Release_VideoCapturer(m_pVideoManager[nI]);
				m_pVideoManager[nI] = NULL;
				//return -1;
			}
			else
			{
				m_pVideoManager[nI]->BegineCaptureThread();
			}
		}
	}

	//音频可用
	if (DevConfigInfo.nAudioId >= 0)
	{
		m_pAudioManager->SetAudioCaptureData(DevConfigInfo.nAudioId, DevConfigInfo.AudioInfo.nChannaels, 
			DevConfigInfo.AudioInfo.nBytesPerSample,  DevConfigInfo.AudioInfo.nSampleRate, 
			DevConfigInfo.AudioInfo.nAudioBufferType, DevConfigInfo.AudioInfo.nPinType, 2, bUseThread);

		m_pAudioManager->SetDShowCaptureCallback((RealDataCallback)(CMaster::RealDataCallbackFunc), (void*)s_pMaster);

		nRet =m_pAudioManager->CreateCaptureGraph();
		if(nRet<=0)
		{
			strTemp.Format(_T("Audio[%d]--创建基本链路失败--In StartDSCapture()."), 0);
			AfxMessageBox(strTemp);

			Release_AudioCapturer(m_pAudioManager);
			m_pAudioManager = NULL;
			//			return -2;
		}
		if (m_pAudioManager)
		{
			nRet = m_pAudioManager->BulidCaptureGraph();
			if(nRet<0)
			{
				strTemp.Format(_T("Audio[%d]--连接链路失败--In StartDSCapture()."), 0);
				AfxMessageBox(strTemp);

				Release_AudioCapturer(m_pAudioManager);
				m_pAudioManager = NULL;
				//			return -2;
			}
			else
			{
				m_pAudioManager->BegineCaptureThread();	
			}
		}
	}	
	return nRet;

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

void CMaster::InitEncoderAndPusher(int nFileIndex)
{
	if(!m_pEncConfigInfo)
		m_pEncConfigInfo = new Encoder_Config_Info;
	if (nFileIndex==0)
	{
		m_AACEncoderManager.Init();
		m_pEncConfigInfo->nScrVideoWidth = 640;
		m_pEncConfigInfo->nScrVideoHeight = 480;
		m_pEncConfigInfo->nFps = 25;
		m_pEncConfigInfo->nMainKeyFrame = 50;
		m_pEncConfigInfo->nMainBitRate = 2048;
		m_pEncConfigInfo->nMainEncLevel = 1;
		m_pEncConfigInfo->nMainEncQuality = 20;
		m_pEncConfigInfo->nMainUseQuality = 0;
			
		m_H264EncoderManager.Init(nFileIndex,m_pEncConfigInfo->nScrVideoWidth,
		m_pEncConfigInfo->nScrVideoHeight,m_pEncConfigInfo->nFps,m_pEncConfigInfo->nMainKeyFrame,
		m_pEncConfigInfo->nMainBitRate,m_pEncConfigInfo->nMainEncLevel,
		m_pEncConfigInfo->nMainEncQuality,m_pEncConfigInfo->nMainUseQuality);
	}

	byte  sps[100];
	byte  pps[100];
	long spslen=0;
	long ppslen=0;
	m_H264EncoderManager.GetSPSAndPPS(nFileIndex,sps,spslen,pps,ppslen);
	memcpy(m_sps, sps,100) ;
	memcpy(m_pps, pps,100) ;
	m_spslen = spslen;
	m_ppslen = ppslen;

	// 测试代码 [8/5/2015-13:28:09 Dingshuai]
	EASY_MEDIA_INFO_T   mediainfo;

	m_pFrameBuf	= new byte[1024*512];
	WSADATA wsaData;
	WSAStartup(MAKEWORD(2,2), &wsaData);

	memset(&mediainfo, 0x00, sizeof(EASY_MEDIA_INFO_T));
	mediainfo.u32VideoCodec =  EASY_SDK_VIDEO_CODEC_H264;//0x1C;
	mediainfo.u32AudioCodec = EASY_SDK_AUDIO_CODEC_AAC;
	mediainfo.u32AudioChannel = 2;
	mediainfo.u32AudioSamplerate = 44100;

	//推送器初始化
	m_pusherId = EasyPusher_Create();
	EasyPusher_SetEventCallback(m_pusherId, __EasyPusher_Callback, 0, NULL);
	EasyPusher_StartStream(m_pusherId, "127.0.0.1", 8554, "live.sdp", "admin", "admin", &mediainfo,1024, 0);
	m_bStartPusher = TRUE;
}

void CMaster::UnInitEncoderAndPusher()
{
	m_bStartPusher = FALSE;
	EasyPusher_StopStream(m_pusherId);
	EasyPusher_Release(m_pusherId);
	m_pusherId = 0;

	if (m_pEncConfigInfo)
	{
		delete m_pEncConfigInfo;
		m_pEncConfigInfo = NULL;
	}
	m_H264EncoderManager.Clean();
	m_AACEncoderManager.Clean();

	if (m_pFrameBuf)
	{
		delete m_pFrameBuf;
		m_pFrameBuf = NULL;
	}
}

//实时数据回调函数
int  CMaster::RealDataCallbackFunc(int nDevId, unsigned char *pBuffer, int nBufSize, 
								   RealDataStreamType realDataType, /*RealDataStreamInfo*/void* realDataInfo, void* pMaster)
{

	if (!pBuffer || nBufSize <= 0)
	{
		return -1;
	}
	CMaster* pThis = (CMaster*)pMaster;
	if (pThis)
	{
		//		EnterCriticalSection(&pThis->m_realDataCS);
		pThis->RealDataManager(nDevId, pBuffer, nBufSize, realDataType, realDataInfo);
		//		LeaveCriticalSection(&pThis->m_realDataCS);
	}

	return 0;
}

//实时音视频数据管理
// nDevId: 设备编号：音频 == -1，视频 == 0---N
// realDataType: 实时音视频数据类型；视频 == 1，音频 == 2
int CMaster::RealDataManager(int nDevId, unsigned char *pBuffer, int nBufSize, 
							 RealDataStreamType realDataType, /*RealDataStreamInfo*/void* realDataInfo)
{
	if (!pBuffer || nBufSize <= 0 )
	{
		return -1;
	}
	//获取实时数据流回调信息
	RealDataStreamInfo* callbackParam = (RealDataStreamInfo*)realDataInfo;
	// 	//录制设备型号
	// 	RecordDevFlag nDeviceType = (RecordDevFlag)(nDevId + 1);
	// 
	// 	BYTE* pWriteBuffer = NULL;//new BYTE[nBufSize];
	// 	BOOL bUseMutiThread = m_pEncConfigInfo->bUseMutiThread;

	//实时音视频数据处理
	switch (realDataType)
	{
	case REALDATA_VIDEO:
		{
			if (m_bStartPusher)
			{
				int nVideoWidth = 640;
				int	nVideoHeight = 480;
				int nWidhtHeightBuf=(nVideoWidth*nVideoHeight*3)>>1;
				BYTE* pDataBuffer=new unsigned char[nWidhtHeightBuf];
				YUY2toI420(nVideoWidth, nVideoHeight, pBuffer, pDataBuffer);
				
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
				if (datasize>0&&m_bStartPusher)
				{
					memset(m_pFrameBuf, 0, 1024*512);
					EASY_AV_Frame   avFrame;
					bool bKeyF = keyframe;
					byte btHeader[4];
					btHeader[0] = 0x00;
					btHeader[1] = 0x00;
					btHeader[2] = 0x00;
					btHeader[3] = 0x01;
					memset(&avFrame, 0x00, sizeof(EASY_AV_Frame));
					if (bKeyF)
					{
						avFrame.u32AVFrameLen  =  datasize + 8 + m_spslen+m_ppslen;
						memcpy(m_pFrameBuf, btHeader, 4);
						memcpy(m_pFrameBuf+4, m_sps, m_spslen);
						memcpy(m_pFrameBuf+4+m_spslen, btHeader, 4);
						memcpy(m_pFrameBuf+4+m_spslen+4, m_pps, m_ppslen);
						memcpy(m_pFrameBuf+4+m_spslen+4+m_ppslen, btHeader, 4);
						memcpy(m_pFrameBuf+4+m_spslen+4+m_ppslen+4, pdata+4, datasize-4);
					} 
					else
					{
						avFrame.u32AVFrameLen = datasize;
						memcpy(m_pFrameBuf, btHeader, 4);
						memcpy(m_pFrameBuf+4, pdata+4, datasize-4);
					}
					avFrame.u32AVFrameFlag = EASY_SDK_VIDEO_FRAME_FLAG;
					avFrame.pBuffer = (unsigned char*)m_pFrameBuf;
					avFrame.u32VFrameType = (bKeyF)?EASY_SDK_VIDEO_FRAME_I:EASY_SDK_VIDEO_FRAME_P;
					long nTimeStamp = clock();
					avFrame.u32TimestampSec = nTimeStamp/1000;
					avFrame.u32TimestampUsec = (nTimeStamp%1000)*1000;

					EasyPusher_PushFrame(m_pusherId, &avFrame);
				}
			}

// 			if (m_pRecordModel->nRecFlag==RECFLAG_RUNNING&&(m_pmp4file[0]||m_pMP4Package[0]))//&&(m_nCountQueueVideoFrames<m_nMaxQueueVideoFrames))
// 			{	
// 				FrameInfo *pTmpinfo=NULL;
// 				pTmpinfo=new FrameInfo;
// 
// 				int nWidhtHeightBuf=(nVideoWidth*nVideoHeight*3)>>1;
// 				pTmpinfo->pDataBuffer=new unsigned char[nWidhtHeightBuf];
// 				YUY2toI420(nVideoWidth,nVideoHeight,pDataWriteWaterMask,pTmpinfo->pDataBuffer);
// 				pTmpinfo->nBufSize=nWidhtHeightBuf;
// 				//	memcpy(pTmpinfo->pDataBuffer,m_pRealWriteBuffer,nBufSize);			
// 				pTmpinfo->bIsVideo=TRUE;
// 				pTmpinfo->nID=0;	
// 
// 				EnterCriticalSection(&m_cs);					
// 				pTmpinfo->nTimestamp=clock()-m_unVPauseTime;
// 				m_queueframe.push(pTmpinfo);
// 				m_nCountQueueVideoFrames++;
// 				LeaveCriticalSection(&m_cs);
// 			}					

		}//case REALDATA_VIDEO
		break;//REALDATA_VIDEO

	case REALDATA_AUDIO://PCM等原始音频数据
		{
			if (m_bStartPusher)
			{
				byte*pdata=NULL;
				int datasize=0;
				pdata=m_AACEncoderManager.Encoder(pBuffer,nBufSize,datasize);	
				if(datasize>0&&m_bStartPusher)
				{

					EASY_AV_Frame   AudioFrame;
					AudioFrame.u32AVFrameFlag = EASY_SDK_AUDIO_FRAME_FLAG;
					AudioFrame.pBuffer = (unsigned char*)pdata;
					AudioFrame.u32VFrameType = FALSE;//(bKeyF)?EASY_SDK_VIDEO_FRAME_I:EASY_SDK_VIDEO_FRAME_P;
					AudioFrame.u32AVFrameLen = datasize;
					long nTimeStamp = clock();
					AudioFrame.u32TimestampSec = nTimeStamp/1000;
					AudioFrame.u32TimestampUsec = (nTimeStamp%1000)*1000;

					EasyPusher_PushFrame(m_pusherId, &AudioFrame);

// 					if(m_pRecordModel->nRecFlag==RECFLAG_RUNNING&&m_bIsAudioWriteInFile==TRUE)
// 					{
// 						FrameInfo *pTmpinfo;
// 						pTmpinfo=new FrameInfo;
// 						pTmpinfo->pDataBuffer=new unsigned char[datasize];
// 						memcpy(pTmpinfo->pDataBuffer,pdata,datasize);					
// 						pTmpinfo->nBufSize=datasize;
// 						pTmpinfo->bIsVideo=FALSE;
// 						pTmpinfo->nID=0;	
// 
// 						EnterCriticalSection(&m_cs);
// 						pTmpinfo->nTimestamp=clock()-m_unAPauseTime;
// 						m_queueframe.push(pTmpinfo);
// 						LeaveCriticalSection(&m_cs);
// 					}

				}
			}	
		}
		break;

	default:
		break;
	}

	return TRUE;//音视频文件录制
}
