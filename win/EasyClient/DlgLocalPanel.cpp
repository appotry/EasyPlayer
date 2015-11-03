// DlgLocalPanel.cpp : 实现文件
//

#include "stdafx.h"
#include "EasyCamera.h"
#include "DlgLocalPanel.h"
#include "afxdialogex.h"


int CALLBACK __MediaSourceCallBack( int _channelId, int *_channelPtr, int _frameType, char *pBuf, RTSP_FRAME_INFO* _frameInfo);

// CDlgLocalPanel 对话框

IMPLEMENT_DYNAMIC(CDlgLocalPanel, CDialogEx)

CDlgLocalPanel::CDlgLocalPanel(CWnd* pParent /*=NULL*/)
	: CDialogEx(CDlgLocalPanel::IDD, pParent)
{
	pStaticPushAddr		=	NULL;
	pEdtPushAddr		=	NULL;
	pStaticMediaSource	=	NULL;
	pComboxMediaSource	=	NULL;
	pComboxAudioSource	=	NULL;
	pBtnStart			=	NULL;
	pEdtRtspSource		=	NULL;
	pDlgVideo			=	NULL;

	memset(&localSource, 0x00, sizeof(EASY_LOCAL_SOURCE_T));
}

CDlgLocalPanel::~CDlgLocalPanel()
{
}

void CDlgLocalPanel::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CDlgLocalPanel, CDialogEx)
	ON_CBN_SELCHANGE(IDC_COMBO_MEDIA_SOURCE, &CDlgLocalPanel::OnCbnSelchangeComboMediaSource)
	ON_BN_CLICKED(IDC_BUTTON_START, &CDlgLocalPanel::OnBnClickedButtonStart)
END_MESSAGE_MAP()


// CDlgLocalPanel 消息处理程序


BOOL CDlgLocalPanel::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  在此添加额外的初始化
	pStaticPushAddr	=	(CStatic *)GetDlgItem(IDC_STATIC_PUSH_ADDR);
	pEdtPushAddr	=	(CEdit *)GetDlgItem(IDC_EDIT_PUSH_ADDR);

	pStaticMediaSource	=	(CStatic *)GetDlgItem(IDC_STATIC_MEDIA_SOURCE);
	pComboxMediaSource	=	(CComboBox *)GetDlgItem(IDC_COMBO_MEDIA_SOURCE);
	pComboxAudioSource	=	(CComboBox *)GetDlgItem(IDC_COMBO_AUDIO_SOURCE);
	pBtnStart		=	(CButton *)GetDlgItem(IDC_BUTTON_START);
	pEdtRtspSource	=	(CEdit *)GetDlgItem(IDC_EDIT_RTSP_SOURCE);

	pDlgVideo = new CDlgVideo();
	pDlgVideo->Create(IDD_DIALOG_VIDEO, this);
	pDlgVideo->ShowWindow(SW_SHOW);

	if (NULL != pEdtPushAddr)
	{
		pEdtPushAddr->SetWindowTextW(TEXT("127.0.0.1:554/stream.sdp"));
	}

	if (NULL != pComboxMediaSource)
	{
		CAMERA_LIST_T *pUSBCameraList = usbCamera.GetCameraList();
		CAMERA_INFO_T	*pCameraInfo = pUSBCameraList->pCamera;
		if (NULL != pCameraInfo)
		{
			while (pCameraInfo)
			{
				wchar_t wszCameraName[64] = {0,};
				__MByteToWChar(pCameraInfo->friendlyName, wszCameraName, sizeof(wszCameraName)/sizeof(wszCameraName[0]));

				pComboxMediaSource->AddString(wszCameraName);

				pCameraInfo = pCameraInfo->pNext;
			}
		}

		pComboxMediaSource->AddString(TEXT("Rtsp Source"));

		pComboxMediaSource->SetCurSel(0);
	}

	if (NULL != pComboxAudioSource)
	{
		audioCapture.GetAudioInputDevices(pComboxAudioSource->GetSafeHwnd());
	}

	if (NULL != pEdtRtspSource)
	{
		pEdtRtspSource->SetWindowTextW(TEXT("rtsp://"));
#ifdef _DEBUG
		pEdtRtspSource->SetWindowTextW(TEXT("rtsp://192.168.10.29/ufirststream"));
#endif
	}

	OnCbnSelchangeComboMediaSource();

	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}


LRESULT CDlgLocalPanel::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	if (WM_PAINT == message || WM_SIZE == message || WM_MOVE == message)
	{
		UpdateComponents();
	}

	return CDialogEx::WindowProc(message, wParam, lParam);
}


BOOL CDlgLocalPanel::DestroyWindow()
{
	CloseLocalSource();
	__DESTROY_WINDOW(pDlgVideo);

	return CDialogEx::DestroyWindow();
}
void CDlgLocalPanel::UpdateComponents()
{
	CRect	rcClient;
	GetClientRect(&rcClient);
	if (rcClient.IsRectEmpty())		return;

	CRect	rcPushAddr, rcEdtAddr;
	rcPushAddr.SetRect(rcClient.left+3, rcClient.top+3, rcClient.left+80, rcClient.top+23);
	__MOVE_WINDOW(pStaticPushAddr, rcPushAddr);
	rcEdtAddr.SetRect(rcPushAddr.right, rcPushAddr.top, rcClient.right-5, rcPushAddr.bottom);
	__MOVE_WINDOW(pEdtPushAddr, rcEdtAddr);

	CRect	rcSource, rcEdtSource, rcAudioSource, rcBtnStart;
	rcSource.SetRect(rcPushAddr.left, rcPushAddr.bottom+2, rcPushAddr.right, rcPushAddr.bottom+2+rcPushAddr.Height());
	__MOVE_WINDOW(pStaticMediaSource, rcSource);
	rcEdtSource.SetRect(rcSource.right, rcSource.top, rcClient.right-90, rcSource.bottom);
	__MOVE_WINDOW(pComboxMediaSource, rcEdtSource);
	//rcAudioSource.SetRect(rcEdtSource.right+5, rcEdtSource.top, rcEdtSource.right+5+180, rcEdtSource.bottom);
	//__MOVE_WINDOW(pComboxAudioSource, rcAudioSource);
	rcBtnStart.SetRect(rcEdtSource.right+5, rcEdtSource.top, rcClient.right-5, rcEdtSource.bottom+20);
	__MOVE_WINDOW(pBtnStart, rcBtnStart);

	CRect	rcRtspSource;
	rcRtspSource.SetRect(rcEdtSource.left, rcSource.bottom+2, rcEdtSource.right, rcSource.bottom+2+rcSource.Height());
	__MOVE_WINDOW(pEdtRtspSource, rcRtspSource);
	CopyRect(&rcAudioSource, rcRtspSource);
	rcAudioSource.right = rcEdtSource.right;
	__MOVE_WINDOW(pComboxAudioSource, rcAudioSource);

	CRect	rcVideo;
	rcVideo.SetRect(rcClient.left+VIDEO_WINDOW_BORDER_WIDTH, rcSource.bottom+5, rcClient.right-VIDEO_WINDOW_BORDER_WIDTH, rcClient.bottom-VIDEO_WINDOW_BORDER_WIDTH);
	//if (NULL != pEdtRtspSource && pEdtRtspSource->IsWindowVisible())
	{
		rcVideo.top+=rcSource.Height();
		//rcVideo.bottom += rcSource.Height();
	}

	__MOVE_WINDOW(pDlgVideo, rcVideo);
}

void	CDlgLocalPanel::CloseLocalSource()
{
	//Stop

	usbCamera.CloseCamera();
	audioCapture.FreeDirectSound();

	if (localSource.rtspSourceId > 0)
	{
		EasyPlayer_CloseStream(localSource.rtspSourceId);
		localSource.rtspSourceId = -1;
	}

	//Close Pusher
	if (NULL != localSource.pusherHandle)
	{
		EasyPusher_StopStream(localSource.pusherHandle);
		EasyPusher_Release(localSource.pusherHandle);
		localSource.pusherHandle = NULL;
	}

	if (NULL != pBtnStart)		pBtnStart->SetWindowText(TEXT("Start"));
	if (NULL != pDlgVideo)	pDlgVideo->Invalidate();
}

void CDlgLocalPanel::OnCbnSelchangeComboMediaSource()
{
	if (NULL == pComboxMediaSource)		return;

	int iCount = pComboxMediaSource->GetCount();
	int iSel = pComboxMediaSource->GetCurSel();

	if (NULL != pComboxAudioSource)	pComboxAudioSource->ShowWindow(iSel == iCount-1?SW_HIDE:SW_SHOW);
	if (NULL != pEdtRtspSource)	pEdtRtspSource->ShowWindow(iSel == iCount-1?SW_SHOW:SW_HIDE);
}


void CDlgLocalPanel::OnBnClickedButtonStart()
{
	if (localSource.rtspSourceId > 0 || usbCamera.InRunning()==0x00)
	{
		CloseLocalSource();
		return;
	}

	memset(localSource.pushServerAddr, 0x00, sizeof(localSource.pushServerAddr));
	localSource.pushServerPort = 0;
	memset(localSource.sdpName, 0x00, sizeof(localSource.sdpName));


	wchar_t wszURL[128] = {0};//TEXT("192.168.200.7:554/stream.sdp");
	if (NULL != pEdtPushAddr)		pEdtPushAddr->GetWindowText(wszURL, sizeof(wszURL));
	if ( (int)wcslen(wszURL) < 1)
	{
		//please input push server addr
		return;
	}
	char szURL[128] = {0,};
	__WCharToMByte(wszURL, szURL, sizeof(szURL)/sizeof(szURL[0]));

	sscanf(szURL, "%[-_a-zA-Z0-9.]:%d%[-_a-zA-Z0-9:/.]", localSource.pushServerAddr, &localSource.pushServerPort, localSource.sdpName);
	if (localSource.pushServerPort < 1)
	{
		return;
	}
	if ( (int)strlen(localSource.sdpName) < 1)
	{
		return;
	}


	//Rtsp Source
	if (NULL != pEdtRtspSource && pEdtRtspSource->IsWindowVisible())
	{
		//Start
		wchar_t wszURL[128] = {0,};
		if (NULL != pEdtRtspSource)	pEdtRtspSource->GetWindowTextW(wszURL, sizeof(wszURL));
		if (wcslen(wszURL) < 1)		return;

		char szURL[128] = {0,};
		__WCharToMByte(wszURL, szURL, sizeof(szURL)/sizeof(szURL[0]));

		HWND hWnd = NULL;
		if (NULL != pDlgVideo)	hWnd = pDlgVideo->GetSafeHwnd();

		localSource.rtspSourceId = EasyPlayer_OpenStream(szURL, hWnd, DISPLAY_FORMAT_RGB24_GDI, 0x00, "", "", __MediaSourceCallBack, (void *)&localSource);
	
		if (localSource.rtspSourceId > 0)
		{
			EasyPlayer_SetFrameCache(localSource.rtspSourceId, 3);		//设置缓存
			//EasyPlayer_PlaySound(localSource.rtspSourceId);
			if (NULL != pBtnStart)		pBtnStart->SetWindowText(TEXT("Stop"));
		}
	}
	else
	{
		//USB Camera
		CAMERA_LIST_T *pUSBCameraList = usbCamera.GetCameraList();
		CAMERA_INFO_T	*pCameraInfo = pUSBCameraList->pCamera;
		if (NULL != pCameraInfo)
		{
			int selCameraIdx = pComboxMediaSource->GetCurSel();
			int cameraIdx = 0;
			while (pCameraInfo)
			{
				if (cameraIdx == selCameraIdx)
				{
					usbCamera.OpenCamera(cameraIdx, 640, 480, __MediaSourceCallBack, (void *)&localSource);
					break;
				}
				cameraIdx ++;
				pCameraInfo = pCameraInfo->pNext;
			}

			audioCapture.InitDirectSound(pComboxAudioSource->GetSafeHwnd());
			audioCapture.SetCallback(__MediaSourceCallBack, (void *)&localSource);
			WAVEFORMATEX	wfx;
			ZeroMemory(&wfx, sizeof(wfx));
			wfx.wFormatTag = WAVE_FORMAT_PCM;
			wfx.nSamplesPerSec = 16000;	//16000   8000
			wfx.wBitsPerSample = 16;	//16	  8
			wfx.nChannels      = 2;		//1		  2
			wfx.nBlockAlign  = wfx.nChannels * (wfx.wBitsPerSample / 8);
			wfx.nAvgBytesPerSec = wfx.nBlockAlign * wfx.nSamplesPerSec;

			audioCapture.CreateCaptureBuffer(&wfx);
			audioCapture.Start();

			if (NULL != pBtnStart)		pBtnStart->SetWindowText(TEXT("Stop"));
		}
	}
}


int CALLBACK __MediaSourceCallBack( int _channelId, int *_channelPtr, int _frameType, char *pBuf, RTSP_FRAME_INFO* _frameInfo)
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
	if (_frameType == EASY_SDK_VIDEO_FRAME_FLAG)
	{
		frame.u32AVFrameFlag = EASY_SDK_VIDEO_FRAME_FLAG;

		//EasyPusher_PushFrame(pLocalSource->pusherHandle, &frame );
	}
	else if (_frameType == EASY_SDK_AUDIO_FRAME_FLAG)
	{
		frame.u32AVFrameFlag = EASY_SDK_AUDIO_FRAME_FLAG;

		EasyPusher_PushFrame(pLocalSource->pusherHandle, &frame );
	}
	else if (_frameType == EASY_SDK_MEDIA_INFO_FLAG)
	{
		pLocalSource->pusherHandle = EasyPusher_Create();
		if (NULL != pLocalSource->pusherHandle)
		{
			EasyPusher_StartStream(pLocalSource->pusherHandle, pLocalSource->pushServerAddr, pLocalSource->pushServerPort, pLocalSource->sdpName+1, "", "", (EASY_MEDIA_INFO_T*)pBuf, 1024, 0);
		}
	}

	return 0;
}
