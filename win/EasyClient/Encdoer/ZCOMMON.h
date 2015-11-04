// ZCOMMON.h: interface for the ZCOMMON class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ZCOMMON_H__FDFC6F6F_33E2_4C3B_B5C0_5A85176D2C15__INCLUDED_)
#define AFX_ZCOMMON_H__FDFC6F6F_33E2_4C3B_B5C0_5A85176D2C15__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define HTON16(x)  ((x>>8&0xff)|(x<<8&0xff00))
#define HTON24(x)  ((x>>16&0xff)|(x<<16&0xff0000)|x&0xff00)
#define HTON32(x)  ((x>>24&0xff)|(x>>8&0xff00)|\
(x<<8&0xff0000)|(x<<24&0xff000000))

bool Write8(int i8,FILE*f);
bool Write16(int i16,FILE*f);
bool Write24(int i24,FILE*f);
bool Write32(int i32,FILE*f);
bool WriteTime(int itime,FILE*f);



#endif // !defined(AFX_ZCOMMON_H__FDFC6F6F_33E2_4C3B_B5C0_5A85176D2C15__INCLUDED_)
