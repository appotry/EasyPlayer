// AACEncoderManager.cpp: implementation of the CAACEncoderManager class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "AACEncoderManager.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CAACEncoderManager::CAACEncoderManager()
{
	m_nSampleRate=44100;  // 采样率
    m_nChannels=2;         // 声道数
    m_nPCMBitSize=16;      // 单样本位数
    m_nInputSamples=2048;	//最大输入样本数
    m_nMaxOutputBytes=0;	//最大输出字节
	
	m_hAACEncoder=NULL;
    m_pAACConfiguration=NULL; 
//	m_bIsworking=false;
	m_paacbuffer=NULL;
	m_paacbuffer=(byte*)malloc(4096);
	InitializeCriticalSection(&m_hcritical_section);
}

CAACEncoderManager::~CAACEncoderManager()
{
	Clean();
	if(m_paacbuffer)
	{
		free(m_paacbuffer);
		m_paacbuffer=NULL;
	}
	DeleteCriticalSection(&m_hcritical_section);
}

byte* CAACEncoderManager::Encoder(byte *indata,int inlen,int &outlen)
{

	EnterCriticalSection(&m_hcritical_section);
	if(m_hAACEncoder!=NULL)
	{
		int nInputSamples = (inlen>>1);/(16/8);
		outlen = faacEncEncode(m_hAACEncoder, (int32_t*)indata, nInputSamples, m_paacbuffer, 2048);
	}else
	{
		outlen=-1;
	}
	LeaveCriticalSection(&m_hcritical_section);
	
	if (outlen>0)
	{
		return m_paacbuffer;
	}else
	{
		return NULL;
	}
	return NULL;
}

int CAACEncoderManager::Init()
{
	if (IsWorking())
	{
		return 0;
	}
//	Clean();
	EnterCriticalSection(&m_hcritical_section);
	m_hAACEncoder = faacEncOpen(m_nSampleRate, m_nChannels, &m_nInputSamples, &m_nMaxOutputBytes);
	if (m_hAACEncoder!=NULL)
	{	
		m_pAACConfiguration = faacEncGetCurrentConfiguration(m_hAACEncoder);  
		m_pAACConfiguration->inputFormat = FAAC_INPUT_16BIT;
		m_pAACConfiguration->outputFormat=0;//0raw 1adst
		m_pAACConfiguration->useTns=true;
		m_pAACConfiguration->useLfe=false;
		m_pAACConfiguration->aacObjectType=LOW;
		m_pAACConfiguration->shortctl=SHORTCTL_NORMAL;
		m_pAACConfiguration->quantqual=100;
		m_pAACConfiguration->bandWidth=0;
		m_pAACConfiguration->bitRate=0;
		faacEncSetConfiguration(m_hAACEncoder,m_pAACConfiguration);
	}
	LeaveCriticalSection(&m_hcritical_section);
	return 0;
}

int CAACEncoderManager::Clean()
{
	EnterCriticalSection(&m_hcritical_section);
	if (m_hAACEncoder!=NULL)
	{
		faacEncClose(m_hAACEncoder);
		m_hAACEncoder=NULL;
	}
	LeaveCriticalSection(&m_hcritical_section);
	return 0;
}

bool CAACEncoderManager::IsWorking(void)
{
	return m_hAACEncoder!=NULL;
}
