// MyRtmp.cpp: implementation of the MyRtmp class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ZRTMP.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////


ZRTMP::ZRTMP()
{
//	InitializeCriticalSection(&m_cs);
	m_rtmp=NULL;
	m_packet=NULL;
	tempbuff=NULL;
	m_status=0;
}

ZRTMP::~ZRTMP()
{

	Clean();
//	DeleteCriticalSection(&m_cs);
}

void ZRTMP::Init()
{
	Clean();
	m_status=0;
	m_rtmp=RTMP_Alloc();
	RTMP_Init(m_rtmp);
	m_rtmp->Link.timeout=10;
	m_packet=new RTMPPacket();
	RTMPPacket_Alloc(m_packet,1920*1024*3);//给packet分配数据空间
	RTMPPacket_Reset(m_packet);//重置packet状态
	tempbuff=m_packet->m_body;
}

void ZRTMP::Clean()
{
//	EnterCriticalSection(&m_cs);
	m_status=0;
	if (m_rtmp!=NULL)
	{
		if(RTMP_IsConnected(m_rtmp))
		{
			RTMP_Close(m_rtmp);
		}
		RTMP_Free(m_rtmp);
		m_rtmp=NULL;
	}

	if (m_packet!=NULL)
	{
		RTMPPacket_Free(m_packet);
		delete m_packet;
		m_packet=NULL;
	}
//	LeaveCriticalSection(&m_cs);	
}

bool ZRTMP::Link(LPTSTR url)
{
	m_startimestamp=0;
	int res=0;
	RTMP_SetupURL(m_rtmp,url);
	RTMP_EnableWrite(m_rtmp);
	res=RTMP_Connect(m_rtmp,NULL);
	if (res==0)
	{
		return false;
	}
	res=RTMP_ConnectStream(m_rtmp,0);
	if (res==0)
	{
		return FALSE;
	}
	m_packet->m_body=tempbuff;
	m_packet->m_hasAbsTimestamp = 0; 
	m_packet->m_nChannel = 0x04; 
	m_packet->m_nInfoField2 = m_rtmp->m_stream_id;
	return true;
}

void ZRTMP::Send(char *data, int len, int type, int timestemp)
{
//	timestemp=clock();
//	memcpy(bodybuff,data,len);
    m_packet->m_headerType = RTMP_PACKET_SIZE_MEDIUM; 
	m_packet->m_nTimeStamp = timestemp; 
	m_packet->m_packetType=type;
	m_packet->m_nBodySize=len;
	TRACE("type:%d len:%d \n",type,len);
	RTMP_SendPacket(m_rtmp,m_packet,0);
}

bool ZRTMP::WriteH264SPSandPPS(byte*sps,int spslen,byte*pps,int ppslen,int width,int height)
{
//	EnterCriticalSection(&m_cs);
	memset(tempbuff,0,100);
	int datalen=spslen+ppslen+16;
	int templen=0;
	tempbuff[0]=0x17;
	tempbuff[1]=0x00;
	tempbuff[5]=0x01;
	tempbuff[6]=sps[1];
	tempbuff[7]=sps[2];
	tempbuff[8]=sps[3];
	tempbuff[9]=0x03;
	tempbuff[10]=(char)0xe1;
	
	templen=HTON16(spslen);
	memcpy(tempbuff+11,&templen,2);
	memcpy(tempbuff+13,sps,spslen);
	
	tempbuff[13+spslen]=0x01;
	
	templen=HTON16(ppslen);
	memcpy(tempbuff+14+spslen,&templen,2);
	memcpy(tempbuff+16+spslen,pps,ppslen);
	Send(tempbuff,datalen,9,0);
	m_status|=ZOUTFILE_FLAG_VIDEO;
//	LeaveCriticalSection(&m_cs);
	return true;
}
//写入AAC信息
bool ZRTMP::WriteAACInfo(byte*info,int len)
{
//	EnterCriticalSection(&m_cs);
	memset(tempbuff,0,100);
	int datalen=len+2;
	tempbuff[0]=(char)0xaf;
	tempbuff[1]=0x00;
	memcpy(tempbuff+2,info,len);
	Send(tempbuff,datalen,8,0);
	m_status|=ZOUTFILE_FLAG_AUDIO;
//	LeaveCriticalSection(&m_cs);
	return true;
}
//写入一帧，前四字节为该帧NAL长度
bool ZRTMP::WriteH264Frame(byte*data,int len,long timestamp,bool keyframe)
{
//	EnterCriticalSection(&m_cs);
//	if (m_status==ZOUTFILE_FLAG_FULL)
	{	
		if (m_startimestamp==0&&keyframe)
		{
			m_startimestamp=timestamp;
		}
		if (m_startimestamp!=0)
		{
			memset(tempbuff,0,100);
			int datalen=len+5;
		//	int templen=HTON32(len);	
			if (keyframe)
				tempbuff[0]=0x17;
			else
				tempbuff[0]=0x27;
			tempbuff[1]=0x01;
			memcpy(tempbuff+5,data,len);
		//	memcpy(tempbuff+5,&templen,4);
		//	memcpy(tempbuff+9,data,len);
			Send(tempbuff,datalen,9,timestamp-m_startimestamp);
		}
	}
//	LeaveCriticalSection(&m_cs);
	return true;
}
//写入aac数据，只有raw数据
bool ZRTMP::WriteAACFrame(byte*data,int len,long timestamp)
{
//	EnterCriticalSection(&m_cs);
	if (m_startimestamp!=0)
	{
		memset(tempbuff,0,1024*32);
		int datalen=len+2;
		tempbuff[0]=(char)0xaf;
		tempbuff[1]=0x01;
		memcpy(tempbuff+2,data,len);
		Send(tempbuff,datalen,8,timestamp-m_startimestamp);
	}
//	LeaveCriticalSection(&m_cs);
	return true;
}

bool ZRTMP::CanWrite()
{
	return IsLinked();//IsLinked();
}

bool ZRTMP::IsLinked()
{
	return RTMP_IsConnected(m_rtmp);
}
