// ZMP4.cpp: implementation of the ZMP4 class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
//#include "zcaptureh264aac.h"
#include "ZMP4.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

ZMP4::ZMP4()
{
	InitializeCriticalSection(&g_cs);
	m_videtrackid=-1;
	m_audiotrackid=-1;
	IsWrite=false;
}

ZMP4::~ZMP4()
{
	Save();
	DeleteCriticalSection(&g_cs);
}
void ZMP4::Init(bool video,bool audio,long width,long hight,long Samples)
{
	Save();
	IsWrite=true;
	firsttime=true;
//	firsttimeA=true;
	char filename[256]={0};
	sprintf(filename,"%d.mp4",time(NULL));
	m_mp4.MP4CreateAndOpenFile(filename);
	m_mp4.MP4SetFileProfile(1000);
	m_wight=width;
	m_hight=hight;
//	if(video)

//	if(audio)

}
void ZMP4::InitAAC(BYTE*data,int len,long timestemp)
{
	EnterCriticalSection(&g_cs);
/*	if (firsttimeA)
	{
		lasttimeA=timestemp;
		firsttimeA=false;
	}
*/
	m_audiotrackid=m_mp4.MP4CreateAudioTrack(1000);
	m_mp4.MP4SetAudioTrackDecInfo(m_audiotrackid, data, len);
	LeaveCriticalSection(&g_cs);
}
void ZMP4::GetAACData(BYTE*data,int len,long timestemp)
{	
	EnterCriticalSection(&g_cs);
	if (firsttime)
	{
		lasttime=timestemp;
		firsttime=false;
	}
/*	if (firsttimeA)
	{
		lasttimeA=timestemp;
		firsttimeA=false;
	}
*/
	m_mp4.MP4RecvSingleDevWriteCache(0,(unsigned char*)data,len,m_audiotrackid,timestemp-lasttime,2);//
	LeaveCriticalSection(&g_cs);
}
void ZMP4::GetH264Data(BYTE*data,int len,bool key,long timestemp){
	EnterCriticalSection(&g_cs);
	if (firsttime)
	{
		lasttime=timestemp;
		firsttime=false;
	}	
	m_mp4.MP4RecvSingleDevWriteCache(0,(unsigned char*)data,len,m_videtrackid,timestemp-lasttime,1);//
	LeaveCriticalSection(&g_cs);
}
void ZMP4::InitH264(BYTE*sps,int spslen,BYTE* pps,int ppslen,long timestemp)
{	
	EnterCriticalSection(&g_cs);

	//	m_videtrackid=m_mp4.MP4CreateH264VideoTrack(1000,width,hight,MP4_INVALID_DURATION,0x42,0xc0,0x15,MP4_NUL_NUM);
	m_videtrackid=m_mp4.MP4CreateH264VideoTrack(1000,m_wight,m_hight,MP4_INVALID_DURATION,sps[1],sps[2],sps[3],MP4_NUL_NUM);

//	m_mp4.MP4RecvSingleDevWriteCache(0,(unsigned char*)sps,spslen,m_videtrackid,0);
//	m_mp4.MP4RecvSingleDevWriteCache(0,(unsigned char*)pps,ppslen,m_videtrackid,0);
	LeaveCriticalSection(&g_cs);
}

void ZMP4::Save()
{
	if (IsWrite)
	{
	long starttime=clock();
	m_mp4.MP4StopWrite();
	long usetime=clock()-starttime;
//	CString str;
//	str.Format("using time:%d",usetime);
//	AfxMessageBox(str);

	IsWrite=false;
	}
}

void ZMP4::Start()
{
	m_mp4.MP4StartWriteFrameFromCache(0);
}
