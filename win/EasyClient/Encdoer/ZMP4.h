// ZMP4.h: interface for the ZMP4 class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ZMP4_H__CD8C3DF9_A2A4_494D_948E_5672ADBE739D__INCLUDED_)
#define AFX_ZMP4_H__CD8C3DF9_A2A4_494D_948E_5672ADBE739D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include "GnElecPackageMP4.h"
#define MP4_NUL_NUM 3
class ZMP4  
{
	bool IsWrite;
public:
	ZMP4();
	virtual ~ZMP4();

	CRITICAL_SECTION g_cs;	
	CGnElecPackageMP4 m_mp4;
	int m_wight;
	int m_hight;
	int m_videtrackid;
	int m_audiotrackid;
//	unsigned char*tempbuff;
	long lasttime;
//	long lasttimeA;
	bool firsttime;
//	bool firsttimeA;
	
	
public:
	//开始
	void Start();
	/*
	是否第一次获取数据
	*/

	/*
	初始化AAC信息
	BYTE*data,数据内容
	int len,数据长度
	long timestemp,时间戳
	*/
	void InitAAC(BYTE*data,int len,long timestemp);
	/*
	获取AAC数据
	BYTE*data,数据内容
	int len,数据长度
	long timestemp,时间戳
	*/
	void GetAACData(BYTE*data,int len,long timestemp);
	/*
	获取H264数据
	BYTE*data,数据内容
	int len,数据长度
	bool key,是否关键帧
	long timestemp,时间戳
	*/
	void GetH264Data(BYTE*data,int len,bool key,long timestemp);
	/*
	初始化H264信息
	BYTE*sps,sps数据
	int spslen,sps长度
	BYTE* pps,pps数据
	int ppslen,pps长度
	long timestemp,时间戳
	*/	
	void InitH264(BYTE*sps,int spslen,BYTE* pps,int ppslen,long timestemp);
	/*
	初始化，
	bool video,是否有视频
	bool audio,是否有音频
	long width,视频长
	long hight,视频宽
	long Samples,音频采样率
	*/
	void Init(bool video,bool audio,long width,long hight,long Samples);
	//保存
	void Save();


};

#endif // !defined(AFX_ZMP4_H__CD8C3DF9_A2A4_494D_948E_5672ADBE739D__INCLUDED_)
