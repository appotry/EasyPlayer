// ZAAC.cpp: implementation of the ZAAC class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
//#include "zcaptureh264aac.h"
#include "ZAAC.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

ZAAC::ZAAC()
{
	IsPlay=false;	
	m_flv=NULL;
	m_mp4=NULL;
	//m_rtmp=NULL;
	pbAACBuffer=NULL;
	hEncoder=NULL;
}

ZAAC::~ZAAC()
{
	Clean();
}

void ZAAC::AACEncode(BYTE *indata, int inlen)
{
	if (!IsPlay)
	{
		return;
	}
	int nRet = faacEncEncode(hEncoder, (__int32*)indata, nInputSamples, pbAACBuffer, nMaxOutputBytes);
	if(nRet>0)
	{
		if(m_flv)
			m_flv->GetAACData(pbAACBuffer,nRet,clock());
		if(m_mp4)
			m_mp4->GetAACData(pbAACBuffer,nRet,clock());
	//	if (m_rtmp)
	//		m_rtmp->GetAACData(pbAACBuffer,nRet,clock());
	}
}

void ZAAC::Clean()
{
	IsPlay=false;
	if (hEncoder!=NULL)
	{
		faacEncClose(hEncoder);
		hEncoder=NULL;			
		free(pbAACBuffer);
		pbAACBuffer=NULL;
		
	}
	m_flv=NULL;
	m_mp4=NULL;
//	m_rtmp=NULL;
}

void ZAAC::Init(ZFLV *flv,ZMP4 *mp4)//,ZRTMP*rtmp)
{
	m_flv=flv;
	m_mp4=mp4;
	//m_rtmp=rtmp;
	nSampleRate = 44100;  // 采样率
    nChannels = 2;         // 声道数
    nPCMBitSize = 16;      // 单样本位数
	ULONG nInputSamples = 0;//样本数
    nMaxOutputBytes = 0;
	ULONG nPCMByteSize=0;
	hEncoder = faacEncOpen(nSampleRate, nChannels, &nInputSamples, &nMaxOutputBytes);	
	pConfiguration = faacEncGetCurrentConfiguration(hEncoder);  
	pConfiguration->inputFormat = FAAC_INPUT_16BIT;
	pConfiguration->outputFormat=0;//0raw 1adst
	pConfiguration->useTns=true;
	pConfiguration->useLfe=false;
	pConfiguration->aacObjectType=LOW;
	pConfiguration->shortctl=SHORTCTL_NORMAL;
	pConfiguration->quantqual=100;
	pConfiguration->bandWidth=0;
	pConfiguration->bitRate=0;
	faacEncSetConfiguration(hEncoder, pConfiguration);	
	nPCMByteSize=nInputSamples*nPCMBitSize/8;

	pbAACBuffer=(BYTE*)malloc(nMaxOutputBytes);
	UINT SampIndex=GetSRIndex(nSampleRate);
	FLVAACADST m_flvadst={0};
	m_flvadst.OBjecttype=2;
	m_flvadst.SamplIndex1=SampIndex>>1&0x03;
	m_flvadst.SamplIndex2=SampIndex&0x01;	
	m_flvadst.channel=nChannels;	
	if(m_flv)
		m_flv->InitAAC((unsigned char*)&m_flvadst,2,clock());
	if (m_mp4)
		m_mp4->InitAAC((unsigned char*)&m_flvadst,2,clock());
	//if (m_rtmp)
	//	m_rtmp->InitAAC((unsigned char*)&m_flvadst,2,clock());

	IsPlay=true;
}
