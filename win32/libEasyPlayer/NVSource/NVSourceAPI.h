/*
	Copyright (c) 2013-2015 EasyDarwin.ORG.  All rights reserved.
	Github: https://github.com/EasyDarwin
	WEChat: EasyDarwin
	Website: http://www.easydarwin.org
*/
#ifndef __NVSOURCE_API_H__
#define __NVSOURCE_API_H__


#define WIN32_LEAN_AND_MEAN
//#include <windows.h>
#include <winsock2.h>
#define NVSOURCE_API  __declspec(dllexport)

//媒体类型
#ifndef MEDIA_TYPE_VIDEO
#define MEDIA_TYPE_VIDEO		0x00000001
#endif
#ifndef MEDIA_TYPE_AUDIO
#define MEDIA_TYPE_AUDIO		0x00000002
#endif
#ifndef MEDIA_TYPE_EVENT
#define MEDIA_TYPE_EVENT		0x00000004
#endif

//video codec
#define	VIDEO_CODEC_H264	0x1C
#define	VIDEO_CODEC_MJPEG	0x08
#define	VIDEO_CODEC_MPEG4	0x0D
//audio codec
#define AUDIO_CODEC_MP4A	0x15002		//86018
#define AUDIO_CODEC_PCMU	0x10006		//65542


//连接类型
typedef enum __RTP_CONNECT_TYPE
{
	RTP_OVER_TCP	=	0x01,
	RTP_OVER_UDP
}RTP_CONNECT_TYPE;

//帧类型
#ifndef FRAMETYPE_I
#define FRAMETYPE_I		0x01
#endif
#ifndef FRAMETYPE_P
#define FRAMETYPE_P		0x02
#endif
#ifndef FRAMETYPE_B
#define FRAMETYPE_B		0x03
#endif



//帧信息
typedef struct 
{
	unsigned int	codec;			//编码格式
	unsigned char	type;			//帧类型
	unsigned char	fps;			//帧率
	unsigned char	reserved1;
	unsigned char	reserved2;

	unsigned short	width;			//宽
	unsigned short  height;			//高
	unsigned int	sample_rate;	//采样率
	unsigned int	channels;		//声道
	unsigned int	length;			//帧大小
	unsigned int    rtptimestamp;	//rtp timestamp
	unsigned int	timestamp_sec;	//秒
	
	float			bitrate;
	float			losspacket;
}NVS_FRAME_INFO;


typedef int (CALLBACK *NVSourceCallBack)( int _chid, int *_chPtr, int _mediatype, char *pbuf, NVS_FRAME_INFO *frameinfo);


//NVSource Handle
typedef void *NVS_HANDLE;

extern "C"
{
	//获取错误代码
	int	 NVSOURCE_API NVS_GetErrCode();

	int	 NVSOURCE_API	NVS_Init(NVS_HANDLE *handle);
	int	 NVSOURCE_API	NVS_Deinit(NVS_HANDLE *handle);

	int	 NVSOURCE_API	NVS_SetCallback(NVS_HANDLE handle, NVSourceCallBack _callback);

	int	 NVSOURCE_API	NVS_OpenStream(NVS_HANDLE handle, int _channelid, char *_url, RTP_CONNECT_TYPE _connType, unsigned int _mediaType, char *_username, char *_password, void *userPtr, int _reconn/*1000表示长连接,即如果网络断开自动重连, 其它值为连接次数*/);
	int	 NVSOURCE_API	NVS_CloseStream(NVS_HANDLE handle);
};





#endif
