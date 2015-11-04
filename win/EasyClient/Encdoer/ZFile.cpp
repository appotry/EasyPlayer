// ZFile.cpp: implementation of the ZFile class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
//#include "zcaptureh264aac.h"
#include "ZFile.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

ZFile::ZFile()
{
	fp=NULL;
	buff=(char*)malloc(ZFILE_MAX_BUFFER);
	buffsize=0;
}

ZFile::~ZFile()
{
	Clean();
	free(buff);
}

void ZFile::Open(char *filename)
{
	Clean();
	fp=fopen(filename,"wb");
	if (fp==NULL)
	{
		AfxMessageBox("open err");
	}
}

void ZFile::Clean()
{	
	if (fp!=NULL)
	{
		fwrite(buff,1,buffsize,fp);
		fclose(fp);
		fp=NULL;
		buffsize=0;
	}

}

void ZFile::WriteData(void *data, long len)
{
	if ((len+buffsize)>ZFILE_MAX_BUFFER)
	{
		fwrite(buff,1,buffsize,fp);
		buffsize=0;
	}
	memcpy(buff+buffsize,data,len);
	buffsize+=len;
}

bool ZFile::Write8(int i8)
{
	WriteData(&i8,1);
		return true;
}
bool ZFile::Write16(int i16)
{
	i16=HTON16(i16);
	WriteData(&i16,2);
		return true;
}
bool ZFile::Write24(int i24)
{
	i24=HTON24(i24);
	WriteData(&i24,3);
		return true;
}
bool ZFile::Write32(int i32)
{
	i32=HTON32(i32);
	WriteData(&i32,4);
		return true;
}
bool ZFile::WriteTime(int itime)
{
	int temp=0;
	temp=HTON24(itime);
	temp|=(itime&0xff000000);
	WriteData(&temp,4);
	return true;
}