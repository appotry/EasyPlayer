// ZOutFile.h: interface for the ZOutFile class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ZOUTFILE_H__96237C74_F7A8_482A_8876_A12ED200CC9F__INCLUDED_)
#define AFX_ZOUTFILE_H__96237C74_F7A8_482A_8876_A12ED200CC9F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

///先视频，后音频，关键帧优先
#include <time.h>

#define ZOUTFILE_FLAG_VIDEO 0x01
#define ZOUTFILE_FLAG_AUDIO 0x02
#define ZOUTFILE_FLAG_FULL (ZOUTFILE_FLAG_AUDIO|ZOUTFILE_FLAG_VIDEO)
class ZOutFile  
{
public:
	//文件名，类型
	virtual	bool CreateFile(char*filename,int flag)=0;
	//sps,pps第一个字节为0x67或68,
	virtual bool WriteH264SPSandPPS(byte*sps,int spslen,byte*pps,int ppslen,int width,int height)=0;
	//写入AAC信息
	virtual bool WriteAACInfo(byte*info,int len)=0;
	//写入一帧，前四字节为该帧NAL长度
	virtual bool WriteH264Frame(byte*data,int len,long timestamp)=0;
	//写入aac数据，只有raw数据
	virtual bool WriteAACFrame(byte*data,int len,long timestamp)=0;
	//保存文件
	virtual bool SaveFile()=0;
	//写状态

//	void GetWidthAndHeightFromPPS(byte*pps,int ppslen,int&width,int &height);
	ZOutFile();
	virtual ~ZOutFile();

};

#endif // !defined(AFX_ZOUTFILE_H__96237C74_F7A8_482A_8876_A12ED200CC9F__INCLUDED_)
