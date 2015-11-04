
//////////////////////////////////////////////////////////////////////////
//
// DShow获取摄像机视频和输入音频原始数据Dll导出类定义
// DShowCaptureAudioAndVideo_Interface.h
// 
//////////////////////////////////////////////////////////////////////////

#ifndef DSHOWCAPTUREAUDIOANDVIDEO_H
#define DSHOWCAPTUREAUDIOANDVIDEO_H

#ifdef __cplusplus

//为了实现而做的纯虚函数
//因为C＋＋中引入的话就需要源代码，然而按这样去做的话才能只要文件头不用代码了。

//实时数据流的类型
typedef enum _REALDATA_TYPE
{
	REALDATA_RAW = 0,				//网络中直接收到的码流,包含视频、音频等等
	REALDATA_VIDEO,					//原始视频码流,可能是YUY2 UYVY等等
	REALDATA_AUDIO,					//原始音频码流,可能是PCM等等
	REALDATA_H264,					//视频编码后的H264视频数据
	REALDATA_AAC,					//音频编码后的AAC音频数据
}RealDataStreamType;


//回调取得实时数据进行处理
// int RealDataManager(int nDevId, unsigned char *pBuffer, int nBufSize, 
// 					RealDataStreamType realDataType, void* realDataInfo);

typedef int (WINAPI *RealDataCallback)(int nDevId, unsigned char *pBuffer, int nBufSize, 
								RealDataStreamType realDataType, void* realDataInfo, void* pMaster);


typedef struct tagThreadCalInfo
{
	RealDataCallback realDataCalFunc;
	void* pMaster;
}ThreadCalInfo;

//获取视频数据导出类接口
class DShowCaptureVideo_Interface
{
public:
	virtual void WINAPI SetVideoCaptureData(int nIndex,int iDeviceId,HWND hWnd,int nFrameRate,
		int iWidth,int iHeight,char* strRGBByte,int nRenderType,int nPinType, 
		int nDataType, BOOL bIsThread) = 0;
	virtual int WINAPI CreateCaptureGraph() = 0;
	virtual int WINAPI BulidPrivewGraph() = 0;
	virtual void WINAPI SetCaptureVideoErr(int nError) = 0;
	virtual void WINAPI BegineCaptureThread() = 0;
	virtual HRESULT WINAPI SetupVideoWindow(HWND hWnd=0) = 0;
	virtual void WINAPI ResizeVideoWindow() = 0;
	virtual void WINAPI SetDShowCaptureCallback(RealDataCallback realDataCalBack, void* pMaster) = 0;

};

//获取音频数据导出类接口
class DShowCaptureAudio_Interface
{
public:
	virtual void WINAPI SetAudioCaptureData(int iDeviceId,int nChannels,
		int nBytesPerSample,int nSampleRate,int nAudioBufferType,int nPinType, 
		int nDataType, BOOL bIsThread) = 0;
	virtual int  WINAPI CreateCaptureGraph()  = 0;
	virtual int  WINAPI BulidCaptureGraph()   = 0;
	virtual void WINAPI BegineCaptureThread() = 0;
	virtual void WINAPI SetDShowCaptureCallback(RealDataCallback realDataCalBack, void* pMaster) = 0;

};

//视频获取定义接口指针类型
typedef	DShowCaptureVideo_Interface*	LPVideoCapturer;	

LPVideoCapturer	APIENTRY Create_VideoCapturer();//创建控制接口指针
void APIENTRY Release_VideoCapturer(LPVideoCapturer lpVideoCapturer);//销毁控制接口指针


//音频获取定义接口指针类型
typedef	DShowCaptureAudio_Interface*	LPAudioCapturer;	

LPAudioCapturer	APIENTRY Create_AudioCapturer();//创建控制接口指针
void APIENTRY Release_AudioCapturer(LPAudioCapturer lpAudioCapturer);//销毁控制接口指针

//void APIENTRY SetDShowCaptureCallback(RealDataCallback realDataCalBack, void* pMaster);


#endif//__cplusplus

#endif//DSHOWCAPTUREAUDIOANDVIDEO_H