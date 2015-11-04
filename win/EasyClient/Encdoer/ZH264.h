// ZH264.h: interface for the ZH264 class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ZH264_H__73683E64_93AB_48F8_BC47_1EEE761D8CA8__INCLUDED_)
#define AFX_ZH264_H__73683E64_93AB_48F8_BC47_1EEE761D8CA8__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "stdint.h"
#include "ZCOMMON.h"
#include "ZFLV.h"
#include "ZMP4.h"
extern "C"
{
#include "x264_config.h"
#include "x264.h"
}

#define ZH264_DEFAULT 0
#define ZH264_ULTRAFAST 1
#define ZH264_SUPERFAST 2
#define ZH264_VERYFAST 3
#define ZH264_FASTER 4
#define ZH264_FAST 5

void X264_CONFIG_SET(x264_param_t*param,int mode);

class ZH264  
{

	ZFLV*m_flv;
	ZMP4*m_mp4;

	x264_t*h;
	x264_param_t param;
	x264_picture_t m_pic;
	x264_picture_t m_picout;
	x264_nal_t*	nal;
	int yw;
	int yh;

public:
//	void ZSet(	x264_param_t*param);
	/*
	编码yuv数据
	BYTE*indata,YUV420数据
	int inlen,数据长度
	*/
	void H264Encode(BYTE*indata,int inlen);
//	void GetFrame(BYTE*indata,int inlen,BYTE *outdata, int &outlen,bool &key);
//	void GetFirstFrame(BYTE*indata,int inlen,BYTE*spsdata,int &spslen,BYTE*ppsdata,int &ppslen,BYTE*data,int&datalen);
	/*
	清理
	*/
	void Clean();
	/*
	初始化ZH264
	ZFLV*flv,ZFLV指针
	ZMP4*mp4,ZMP4指针
	int w,视频宽
	int h,视频高
	int fps,视频fps
	int cpu=0,无效
	int leave=0,压缩级别1~7，0为默认7
	int mode=0,搜索方式，0菱形，1六边形
	*/
	void Init(ZFLV*flv,ZMP4*mp4,int w,int h,int fps,int bitrate=0,int leave=0,int mode=0);
	ZH264();
	virtual ~ZH264();

};

#endif // !defined(AFX_ZH264_H__73683E64_93AB_48F8_BC47_1EEE761D8CA8__INCLUDED_)
