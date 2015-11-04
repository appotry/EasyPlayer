// ZAAC.h: interface for the ZAAC class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ZAAC_H__CCDDE86E_DF80_4C62_8551_46F9DA67D81D__INCLUDED_)
#define AFX_ZAAC_H__CCDDE86E_DF80_4C62_8551_46F9DA67D81D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

//4096输入字节
//1024输入样本数

class ZAAC  
{
	bool IsPlay;	
	ZFLV*m_flv;
	ZMP4*m_mp4;
	//ZRTMP*m_rtmp;

	ULONG nSampleRate;  // 采样率
    UINT nChannels;         // 声道数
    UINT nPCMBitSize;      // 单样本位数
    ULONG nInputSamples;	//最大输入样本数
    ULONG nMaxOutputBytes;	//最大
	ULONG nInputBytes;

	faacEncHandle hEncoder;
    faacEncConfigurationPtr pConfiguration; 
	BYTE* pbAACBuffer;

public:
	void Init(ZFLV*flv,ZMP4*mp4);//,ZRTMP*rtmp);
	void Clean();
	void AACEncode(BYTE*indata,int inlen);
	ZAAC();
	virtual ~ZAAC();

};

#endif // !defined(AFX_ZAAC_H__CCDDE86E_DF80_4C62_8551_46F9DA67D81D__INCLUDED_)
