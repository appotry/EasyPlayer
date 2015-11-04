
// 宏、结构以及全局变量的定义 [11/3/2015-12:40:12 Dingshuai]
// Create By 天涯浪子(Dingshuai)
// #include "TypeDefine.h"

#define MAX_DEV 1
#define MAXNAMESIZE 64

// 设备捕获参数信息
typedef struct tagDEVICE_CONFIG_INFO
{
	int nVideoId;//视频	ID -1==不用，0-n == id
	int nAudioId;//音频id -1==不用，0-n == id
	int  nDeviceId;//设备序号 1,2,3,4,5....
	struct tagVideoInfo
	{
		int nWidth;
		int nHeight;
		int nVideoWndId;
		int nFrameRate;//视频帧率
		TCHAR strDataType[MAXNAMESIZE];//标识YUV数据类型，如"YUY2" "YUYV" "UYVY等"
		int nRenderType;//呈现方式
		int nPinType;//捕获口
	}VideoInfo;

	struct tagAudioInfo
	{
		int nChannaels;//通道数
		int nBytesPerSample;//采样位数
		int nSampleRate;//采样率
		DWORD nAudioBufferType;//音频缓存区的大小
		int nPinType;//捕获口
	}AudioInfo;

}DEVICE_CONFIG_INFO;

//实时数据流的信息 与 REALDATA_TYPE 配合使用
typedef struct _realdata_callback_param
{	
	int		size;
	struct _video{
		int iEnable;	//该结构体内的信息是否有效 //REALDATA_VIDEO or REALDATA_YUV 有效
		unsigned int timestampe;	//时间戳信息，单位毫秒(ms)
		int width;		//视频宽度	//REALDATA_YUV 有效
		int height;		//视频长度  //REALDATA_YUV 有效
		int encodeType;	//视频编码类型 0:未知 H264 = 1 or mpeg4 = 2 //当前默认为H264 //REALDATA_VIDEO 有效
		int frameType;	//帧类型 0:未知 IDR帧=1 I帧=2 P帧=3 B帧=4 //REALDATA_VIDEO 有效
	}video;
	struct _audio{
		int iEnable;	//该结构体内的信息是否有效 //REALDATA_AUDIO or REALDATA_PCM 有效
		unsigned int timestampe;	//时间戳信息，单位毫秒(ms)
		int	bits;		//位宽 16bits or 8bits	//REALDATA_PCM 有效
		int samplerate;	//采样率 8000Hz 16000Hz 44100Hz等等	//REALDATA_PCM 有效
		int channels;	//声道数 单声道=1 or 双声道=2	//REALDATA_PCM 有效
		int encodeType;	//音频编码类型 0:未知 AAC=1 MP3=2 G729=10	//REALDATA_AUDIO 有效
	}audio;
}RealDataStreamInfo;

//编码配置信息
//m_nResizeVideoWidth,m_nResizeVideoHeight,25,2000,3,m_nScrVideoWidth,m_nScrVideoHeight);
typedef struct tagEncoder_Config_Info
{
	int nScrVideoWidth;
	int nScrVideoHeight;
	int nResizeVideoWidth;
	int nResizeVideoHeight;
	int nFps;//帧率
	int nMainBitRate;//主视频码率
	int nSingleBitRate;//从视频码率
	int nMainEncLevel;//主编码级别
	int nSingleEncLevel;//从编码级别
	int nMainEncQuality;//主编码视频质量
	int nSingleEncQuality;//从编码视频质量


	int nMainKeyFrame;//主关键帧
	int nSingleKeyFrame;//从关键帧

	int nMainUseQuality;
	int nSingleUseQuality;//是否适用质量参数1--质量,0--码率
	int nRtmpUseQuality;//是否适用质量参数1--质量,0--码率
	int nRtmpVideoWidth;//rtmp
	int nRtmpVideoHeight;
	int nRtmpEncLevel;
	int nRtmpEncQuality;
	int nRtmpBitRate;
	int nRtmpKeyFrame;
	CString nRtmpURL;

	int nMaxVideoFrames;//电影模式最大视频帧数
	int nSingleVideoFrames;//资源模式最大视频帧数

	DWORD nMaxPerFileSize;//最大每个文件大小
	int nResizeType;

}Encoder_Config_Info;