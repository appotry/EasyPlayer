// MyRtmp.h: interface for the MyRtmp class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MYRTMP_H__36C4FA2C_221E_4621_AD56_01467C871052__INCLUDED_)
#define AFX_MYRTMP_H__36C4FA2C_221E_4621_AD56_01467C871052__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
//#include "StdAfx.h"
#include <librtmp/rtmp.h>

#pragma comment(lib,"librtmp.lib")

class ZRTMP  
{
//	CRITICAL_SECTION m_cs;
	long m_startimestamp;
	RTMP*m_rtmp;
	char*tempbuff;
	RTMPPacket*m_packet;
	int m_status;
public:
/*	void InitAAC(BYTE *data, int len, long timestemp);
	void GetAACData(BYTE *data, int len,long timestemp);
	void GetH264Data(BYTE*data,int len,bool key,long timestemp);
	void InitH264(BYTE *sps, int spslen, BYTE *pps, int ppslen,long timestemp);
*/
	bool WriteH264SPSandPPS(byte*sps,int spslen,byte*pps,int ppslen,int width=0,int height=0);
	//写入AAC信息
	bool WriteAACInfo(byte*info,int len);
	//写入一帧，前四字节为该帧NAL长度
	bool WriteH264Frame(byte*data,int len,long timestamp,bool keyframe);
	//写入aac数据，只有raw数据
	bool WriteAACFrame(byte*data,int len,long timestamp);
	bool CanWrite();

	void Send(char*data,int len,int type,int timestemp);
	bool Link(LPTSTR url);
	void Clean();
	void Init();
	bool IsLinked();
	ZRTMP();
	virtual ~ZRTMP();
};

#endif // !defined(AFX_MYRTMP_H__36C4FA2C_221E_4621_AD56_01467C871052__INCLUDED_)
