// AACEncoderManager.h: interface for the CAACEncoderManager class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_AACENCODERMANAGER_H__CB1D102B_73A8_4C16_9DBA_4F3874CF446D__INCLUDED_)
#define AFX_AACENCODERMANAGER_H__CB1D102B_73A8_4C16_9DBA_4F3874CF446D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CAACEncoderManager  
{
private:
	ULONG m_nSampleRate;  // 采样率
    UINT m_nChannels;         // 声道数
    UINT m_nPCMBitSize;      // 单样本位数
    ULONG m_nInputSamples;	//最大输入样本数
    ULONG m_nMaxOutputBytes;	//最大输出字节
//	ULONG nInputBytes;		//输入字节
	faacEncHandle m_hAACEncoder;
    faacEncConfigurationPtr m_pAACConfiguration; 
//	bool m_bIsworking;
	byte* m_paacbuffer;
	CRITICAL_SECTION m_hcritical_section;	
public:
	int Clean();
	int Init();
	byte* Encoder(byte *indata,int inlen,int &outlen);
	CAACEncoderManager();
	virtual ~CAACEncoderManager();

	bool IsWorking(void);
};

#endif // !defined(AFX_AACENCODERMANAGER_H__CB1D102B_73A8_4C16_9DBA_4F3874CF446D__INCLUDED_)
