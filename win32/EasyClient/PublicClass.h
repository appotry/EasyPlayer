#pragma once

class CPublicClass
{
public:
	CPublicClass(void);
	~CPublicClass(void);
public:
	static CRect StringToRect(char *szPos);
	static POINT StringToPoint(CString szPos);
	static CRect ResizeRect(double fxScale,double fyScale,CRect srcRect,BOOL bScaleSize);
	static void ReadCtrlConfig(CString strCtrlName, CString strSecName, CString strIniFile, BOOL& bIsVisible, CRect& rect);
		
	static CString UTF82GBK(CString cstr);
	static CString GBK2UTF8(CString cstr);
	static CString ExtendNameByName(CString strName);
	static CString GetFloderPathByPath(CString strPath);
	static BOOL CreateDirectoryAll(const char * szPath);

};
