// ZCOMMON.cpp: implementation of the ZCOMMON class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
//#include "yuv2h264.h"
#include "ZCOMMON.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
bool Write8(int i8,FILE*f)
{
	if(fwrite(&i8,1,1,f)!=1)
		return false;
	return true;
}
bool Write16(int i16,FILE*f)
{
		i16=HTON16(i16);
	if(fwrite(&i16,1,2,f)!=1)
		return false;

	return true;
}
bool Write24(int i24,FILE*f)
{
	i24=HTON24(i24);
	if(fwrite(&i24,1,3,f)!=1)
		return false;
	
	return true;
}
bool Write32(int i32,FILE*f)
{
		i32=HTON32(i32);
	if(fwrite(&i32,1,4,f)!=1)
		return false;

	return true;
}
bool Peek8(int &i8,FILE*f)
{
	if(fread(&i8,1,1,f)!=1)
		return false;
	fseek(f,-1,SEEK_CUR);
	return true;
}

bool WriteTime(int itime,FILE*f)
{
	int temp=0;
	temp=HTON24(itime);
	temp|=(itime&0xff000000);
	if(fwrite(&temp,1,4,f)!=1)
		return false;
	return true;
}
