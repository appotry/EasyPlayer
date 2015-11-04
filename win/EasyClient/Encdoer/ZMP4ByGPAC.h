// ZMP4ByGPAC.h: interface for the ZMP4AVC1class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(ZMP4GPAC_H)
#define ZMP4GPAC_H

#include <time.h>

#include <gpac/isomedia.h>
#pragma comment(lib,"libgpac.lib")

#ifndef ZOUTFILE_FLAG_FULL
#define ZOUTFILE_FLAG_VIDEO 0x01
#define ZOUTFILE_FLAG_AUDIO 0x02
#define ZOUTFILE_FLAG_FULL (ZOUTFILE_FLAG_AUDIO|ZOUTFILE_FLAG_VIDEO)
#endif

class ZMP4ByGPAC 
{
	//设置风格
	int		m_flag;//包含音频视频

	long	m_audiostartimestamp;
	long	m_videostartimestamp;

	GF_ISOFile *p_file;//MP4文件
    GF_AVCConfig *p_config;//MP4配置
    GF_ISOSample *p_videosample;//MP4帧
	GF_ISOSample *p_audiosample;//MP4帧

	
	int m_wight;
	int m_hight;
	int m_videtrackid;
	int m_audiotrackid;

	unsigned int i_videodescidx;
	unsigned int i_audiodescidx;
	

public:
	ZMP4ByGPAC();
	virtual ~ZMP4ByGPAC();
public:

	bool CreateFile(char*filename,int flag);
	//sps,pps第一个字节为0x67或68,
	bool WriteH264SPSandPPS(unsigned char*sps,int spslen,unsigned char*pps,int ppslen,int width,int height);
	//写入AAC信息
	bool WriteAACInfo(unsigned char*info,int len);
	//写入一帧，前四字节为该帧NAL长度
	bool WriteH264Frame(unsigned char*data,int len,bool keyframe,long timestamp);
	//写入aac数据，只有raw数据
	bool WriteAACFrame(unsigned char*data,int len,long timestamp);
	//保存文件
	bool SaveFile();
	
	bool CanWrite();
};

#endif // !defined(AFX_ZMP4_H__CD8C3DF9_A2A4_494D_948E_5672ADBE739D__INCLUDED_)
