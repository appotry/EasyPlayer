// ZFLV.cpp: implementation of the ZFLV class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
//#include "yuv2h264.h"
#include "ZFLV.h"
#include "ZCOMMON.h"
#include <time.h>

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

ZFLV::ZFLV()
{
//	fp=NULL;
//	tempbuff=NULL;
	InitializeCriticalSection(&g_cs);
//	m_videtrackid=-1;
//	m_audiotrackid=-1;
}

ZFLV::~ZFLV()
{
	DeleteCriticalSection(&g_cs);
	Save();
}

void ZFLV::Save()
{
	m_fp.Clean();
/*	if (fp!=NULL)
	{
		fclose(fp);
//		m_mp4.MP4StopWrite();
	//	m_mp4.MP4CloseWriteFile();
		fp=NULL;
	}
//	if (tempbuff!=NULL)
//	{
//		free(tempbuff);
//	}
*/
}

void ZFLV::Init(bool video, bool audio)
{
//	if (fp)
//	{
		Save();
//	}
	char filename[256]={0};
	sprintf(filename,"%d.flv",time(NULL));

	m_fp.Open(filename);

//	fp=fopen(filename,"wb");	
	char*head="FLV";
	m_fp.WriteData(head,3);

//	fwrite(head,1,3,fp);
//	Write8(1,fp);
	m_fp.Write8(1);
	char type=0;
	if (video)
	{
		type|=0x1;
	}
	if (audio)
	{
		type|=0x04;
	}
//	Write8(type,fp);
	m_fp.Write8(type);
//	Write32(9,fp);
	m_fp.Write32(9);

//	Write32(0,fp);
	m_fp.Write32(0);

	lasttime=clock();
	firsttime=true;


	CString tempname=filename;
		tempname=tempname+".mp4";

//	m_mp4.MP4CreateAndOpenFile(tempname);
//	m_mp4.MP4SetFileProfile();

//	m_videtrackid=m_mp4.MP4CreateH264VideoTrack(90000,1920,1080,MP4_INVALID_DURATION,0x42,0xc0,0x15,MP4_NUL_NUM);
//	m_audiotrackid=m_mp4.MP4CreateAudioTrack(44100);
//	BYTE ucDecInfoBuff[] =  {0x12,0x10};
//	uint32_t unBuffSize = sizeof(ucDecInfoBuff)*sizeof(BYTE);
//	m_mp4.MP4SetAudioTrackDecInfo(m_audiotrackid, ucDecInfoBuff, unBuffSize);
//	m_mp4.MP4StartWriteFrameFromCache(0);


//	tempbuff=(unsigned char*)malloc(1024*768);
//	memset(tempbuff,0,1024*768);
//	tempbuff[MP4_NUL_NUM]=0x01;
	

}

void ZFLV::InitH264(BYTE *sps, int spslen, BYTE *pps, int ppslen,long timestemp)
{
/*	if (!fp)
	{return;
	}
*/	EnterCriticalSection(&g_cs);


	if (firsttime)
	{
		lasttime=timestemp;
			firsttime=false;
	}

//	Write8(9,fp);
	m_fp.Write8(9);

	int datalen=spslen+ppslen+16;
//	Write24(datalen,fp);
	m_fp.Write24(datalen);

//	WriteTime(0,fp);
	m_fp.WriteTime(0);
//	Write24(0,fp);
	m_fp.Write24(0);
	///////////////////////////
	
//	Write8(0x17,fp);
//	Write8(0,fp);
//	Write24(0,fp);

	m_fp.Write8(0x17);
	m_fp.Write8(0);
	m_fp.Write24(0);

/*	Write8(0x01,fp);
	Write8(0x42,fp);
	Write8(0xc0,fp);
	Write8(0x15,fp);
	Write8(0x03,fp);
	Write8(0xE1,fp);
*/

	m_fp.Write8(0x01);

	m_fp.Write8(sps[1]);
	m_fp.Write8(sps[2]);
	m_fp.Write8(sps[3]);

//	m_fp.Write8(0x42);
//	m_fp.Write8(0xc0);
//	m_fp.Write8(0x15);
	m_fp.Write8(0x03);
	m_fp.Write8(0xe1);

/*	Write16(spslen,fp);
	fwrite(sps,1,spslen,fp);
	Write8(0x01,fp);
	Write16(ppslen,fp);
	fwrite(pps,1,ppslen,fp);
	*/
	m_fp.Write16(spslen);
	m_fp.WriteData(sps,spslen);
	m_fp.Write8(0x01);
	m_fp.Write16(ppslen);
	m_fp.WriteData(pps,ppslen);
////////////////////
	//Write32(datalen+11,fp);
	m_fp.Write32(datalen+11);
////////////////////////////////////////



//	memcpy(tempbuff+MP4_NUL_NUM+1,sps,spslen);
//	m_mp4.MP4RecvSingleDevWriteCache(0,(unsigned char*)tempbuff,spslen+MP4_NUL_NUM+1,m_videtrackid,0);
//	memcpy(tempbuff+MP4_NUL_NUM+1,pps,ppslen);
//	m_mp4.MP4RecvSingleDevWriteCache(0,(unsigned char*)tempbuff,ppslen+MP4_NUL_NUM+1,m_videtrackid,0);


//////////////////////////////////////////
	LeaveCriticalSection(&g_cs);
}

void ZFLV::GetH264Data(BYTE*data,int len,bool key,long timestemp)
{
//	if (!fp)
//	{return;
//	}
	EnterCriticalSection(&g_cs);

	if (firsttime)
	{
		lasttime=timestemp;
			firsttime=false;
	}
	int datalen=len+9;
	m_fp.Write8(9);
	m_fp.Write24(datalen);
	m_fp.WriteTime(timestemp-lasttime);
	m_fp.Write24(0);

//	Write8(9,fp);
//	Write24(datalen,fp);
//	WriteTime(timestemp-lasttime,fp);
//	Write24(0,fp);

	///////////////////////////
	if (key)
//	Write8(0x17,fp);
	m_fp.Write8(0x17);
	else
//	Write8(0x27,fp);
	m_fp.Write8(0x27);

/*	Write8(1,fp);
	Write24(0,fp);
	Write32(len,fp);
	fwrite(data,1,len,fp);
*/	////////////////////
	m_fp.Write8(1);
	m_fp.Write24(0);
	m_fp.Write32(len);
	m_fp.WriteData(data,len);
//	Write32(datalen+11,fp);
	m_fp.Write32(datalen+11);
//	memcpy(tempbuff+3,data,len);
//	m_mp4.MP4RecvSingleDevWriteCache(0,(unsigned char*)tempbuff,len+3,m_videtrackid,timestemp-lasttime);

	LeaveCriticalSection(&g_cs);
}

void ZFLV::GetAACData(BYTE *data, int len,long timestemp)
{
//	if (!fp)
//	{return;
//	}
	EnterCriticalSection(&g_cs);
	if (firsttime)
	{
		lasttime=timestemp;
		firsttime=false;
	}
//	Write8(8,fp);
	m_fp.Write8(8);
	int datalen=len+2;
/*	Write24(datalen,fp);
	WriteTime(timestemp-lasttime,fp);
	Write24(0,fp);
*/
	m_fp.Write24(datalen);
	m_fp.WriteTime(timestemp-lasttime);
	m_fp.Write24(0);
	///////////////////////////
/*	Write8(0xaf,fp);
	Write8(0x01,fp);
	fwrite(data,1,len,fp);
*/	////////////////////
	m_fp.Write8(0xaf);
	m_fp.Write8(0x01);
	m_fp.WriteData(data,len);
	//	Write32(datalen+11,fp);
	m_fp.Write32(datalen+11);

//	m_mp4.MP4RecvSingleDevWriteCache(0,(unsigned char*)data,len,m_audiotrackid,timestemp-lasttime,2);

	LeaveCriticalSection(&g_cs);
}

void ZFLV::InitAAC(BYTE *data, int len, long timestemp)
{
//	if (!fp)
//	{return;
//	}
	EnterCriticalSection(&g_cs);
	if (firsttime)
	{
		lasttime=timestemp;
		firsttime=false;
	}
	//Write8(8,fp);
	m_fp.Write8(8);
	int datalen=len+2;
/*	Write24(datalen,fp);
	WriteTime(timestemp-lasttime,fp);
	Write24(0,fp);
*/
	m_fp.Write24(datalen);
	m_fp.WriteTime(timestemp-lasttime);
	m_fp.Write24(0);
	
	///////////////////////////
/*	Write8(0xaf,fp);
	Write8(0x00,fp);
	fwrite(data,1,len,fp);
*/
	m_fp.Write8(0xaf);
	m_fp.Write8(0x00);
	m_fp.WriteData(data,len);
	////////////////////
//	Write32(datalen+11,fp);
	m_fp.Write32(datalen+11);



//	m_mp4.MP4SetAudioTrackDecInfo(m_audiotrackid, data, len);

	LeaveCriticalSection(&g_cs);
}

void ZFLV::Start()
{

}
