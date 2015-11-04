#pragma once

// 音视频DShow捕获，回调获取YUV数据编码推送EasyDarwin服务器管理类 [11/3/2015-12:29:46 Dingshuai]
// Create By 天涯浪子(Dingshuai)

#include "TypeDefine.h"

#include "Encdoer/RGBtoYUV.h"
#include "./Encdoer/FAACEncoder.h"
#include "./Encdoer/H264Encoder.h"
#include "./Encdoer/H264EncoderManager.h"
#include "./EasyPusher/EasyPusherAPI.h"
#pragma comment(lib, "./EasyPusher/libEasyPusher.lib")

//包含DShow捕获音视频模块DLL库
#include "./DShowCapture/DShowCaptureAudioAndVideo_Interface.h"
#pragma comment(lib, "./DShowCapture/DShowCaptureAudioAndVideoDll.lib")

class CEasyClientDlg;

class CMaster
{
	//Common Function
public:
	CMaster(CEasyClientDlg* pMainDlg);
	~CMaster(void);
	//初始化唯一的管理实例
	static CMaster* Instance(CEasyClientDlg* pMainDlg);
	//销毁唯一的管理实例
	static void UnInstance();
	//释放Master所占相关资源
	void RealseMaster();

	//Member Function
public:
	void InitDSCapture();
	void UnInitDSCapture();
	void InitEncoderAndPusher(int nFileIndex = 0);
	void UnInitEncoderAndPusher();
	int StartDSCapture(HWND hShowWnd[], int nWndNum);
	//实时数据回调函数
	static int CALLBACK RealDataCallbackFunc(int nDevId, unsigned char *pBuffer, int nBufSize, 
		RealDataStreamType realDataType, /*RealDataStreamInfo*/void* realDataInfo, void* pMaster);
	//回调取得实时数据进行处理
	int RealDataManager(int nDevId, unsigned char *pBuffer, int nBufSize, 
		RealDataStreamType realDataType, /*RealDataStreamInfo*/void* realDataInfo);

	//Member Var
private:
	//唯一的实例化管理指针 
	static CMaster* s_pMaster;
	// 主管理类所在的主窗口
	CEasyClientDlg* m_pMainDlg;
	INT m_nDevCount;//启动设备数

	//视频设备控制实例
	LPVideoCapturer m_pVideoManager[MAX_DEV];
	//音频设备控制实例
	LPAudioCapturer m_pAudioManager;
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
	//Pusher相关
	void* m_pusherId;
	byte* m_pFrameBuf; 
	BOOL m_bStartPusher;

};
