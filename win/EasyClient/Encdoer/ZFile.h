// ZFile.h: interface for the ZFile class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ZFILE_H__C1272172_3473_4395_8AC5_203BC6D8D8A3__INCLUDED_)
#define AFX_ZFILE_H__C1272172_3473_4395_8AC5_203BC6D8D8A3__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define ZFILE_MAX_BUFFER 1024*1024

class ZFile  
{
	FILE*fp;
	char*buff;
	long buffsize;
public:
	void WriteData(void*data,long len);
	void Clean();
	void Open(char*filename);
	bool Write8(int i8);
	bool Write16(int i16);
	bool Write24(int i24);
	bool Write32(int i32);
	bool WriteTime(int itime);


	ZFile();
	virtual ~ZFile();

};

#endif // !defined(AFX_ZFILE_H__C1272172_3473_4395_8AC5_203BC6D8D8A3__INCLUDED_)
