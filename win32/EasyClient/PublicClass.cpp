#include "stdafx.h"
#include "PublicClass.h"

CPublicClass::CPublicClass(void)
{
}

CPublicClass::~CPublicClass(void)
{
}




void CPublicClass::ReadCtrlConfig(CString strCtrlName, CString strSecName, CString strIniFile, BOOL& bIsVisible, CRect& rect)
{
	TCHAR buf[64];
	CString strKey;
	strKey.Format(_T("%s_IsVisible"), strCtrlName);
	bIsVisible = GetPrivateProfileInt(strSecName, strKey, 0, strIniFile);
	if (bIsVisible)
	{
		//创建控件
		strKey.Format(_T("%s_Position"), strCtrlName);
		int nRet=GetPrivateProfileString(strSecName, strKey, _T(""), buf, sizeof(buf), strIniFile);
		rect = CPublicClass::StringToRect(buf);
	}
	else
	{
		rect.SetRect(0, 0, 0, 0);
	}
}

CRect CPublicClass::ResizeRect(double fxScale,double fyScale,CRect srcRect,BOOL bScaleSize)
{
	CRect desRect;
	int nDesWidth = srcRect.Width();
	int nDesHeight = srcRect.Height();
	int nDesCenterX = (srcRect.Width()/2+srcRect.left)*fxScale;
	int nDesCenterY = (srcRect.Height()/2+srcRect.top)*fyScale;
	if(bScaleSize)
	{
		nDesWidth  = srcRect.Width()*fxScale;
		nDesHeight = srcRect.Height()*fyScale;
	}


	desRect.left =  nDesCenterX - nDesWidth/2;
	desRect.top =  nDesCenterY - nDesHeight/2;
	desRect.right = desRect.left + nDesWidth;
	desRect.bottom = desRect.top + nDesHeight;
	return desRect;
}

POINT CPublicClass::StringToPoint(CString szPos)
{
	CString str = szPos;
	if (str.GetLength() == 0)
	{
		POINT point;
		point.x=0;
		point.y=0;
		return point;
	}

	POINT point;
	CString val;
	int nFirst = 0;
	int nNext = 0;
	str.TrimLeft();
	str.TrimRight();

	//取 Size.cx
	nNext = str.Find(',', nFirst);
	val = str.Mid(nFirst, nNext - nFirst);
	val.TrimLeft();
	val.TrimRight();
	point.x = atoi(val);

	//取 Size.cy
	val = str.Mid(nNext + 1);
	val.TrimLeft();
	val.TrimRight();
	point.y = atoi(val);

	return point;

}
CRect CPublicClass::StringToRect(char *szPos)
{
	CString str = szPos;
	if (str.GetLength() == 0)
	{
		return CRect(0, 0, 0, 0);
	}

	CRect rect;
	CString val;
	int nFirst = 0;
	int nNext = 0;
	str.TrimLeft();
	str.TrimRight();

	//取 Rect.left
	nNext = str.Find(',', nFirst);
	val = str.Mid(nFirst, nNext - nFirst);
	val.TrimLeft();
	val.TrimRight();
	rect.left = atoi(val);

	//取 Rect.top
	nFirst = nNext + 1;
	nNext = str.Find(',', nFirst);
	val = str.Mid(nFirst, nNext - nFirst);
	val.TrimLeft();
	val.TrimRight();
	rect.top = atoi(val);

	//取 Rect.Right
	nFirst = nNext + 1;
	nNext = str.Find(',', nFirst);
	val = str.Mid(nFirst, nNext - nFirst);
	val.TrimLeft();
	val.TrimRight();
	rect.right = rect.left+atoi(val);

	//取 Rect.Bottom
	nFirst = nNext + 1;
	val = str.Mid(nFirst);
	val.TrimLeft();
	val.TrimRight();
	rect.bottom = rect.top+atoi(val);

	return rect;
}

CString CPublicClass::UTF82GBK(CString cstr)
{
	 CString result;
     WCHAR *strSrc;
     TCHAR *szRes;
     const char* str = (LPCTSTR)cstr;

     //获得临时变量的大小
     int i = MultiByteToWideChar(CP_UTF8, 0, str, -1, NULL, 0);
     strSrc = new WCHAR[i+1];
     MultiByteToWideChar(CP_UTF8, 0, str, -1, strSrc, i);

     //获得临时变量的大小
     i = WideCharToMultiByte(CP_ACP, 0, strSrc, -1, NULL, 0, NULL, NULL);
     szRes = new TCHAR[i+1];
     int j=WideCharToMultiByte(CP_ACP, 0, strSrc, -1, szRes, i, NULL, NULL);

     result = szRes;
     delete []strSrc;
     delete []szRes;
	  return result;
}

CString CPublicClass::GBK2UTF8(CString cstr)
{
     CString result;
     WCHAR *strSrc;
     TCHAR *szRes;
     const char* str = (LPCTSTR)cstr;

     //获得临时变量的大小
     int i = MultiByteToWideChar(CP_ACP, 0, str, -1, NULL, 0);
     strSrc = new WCHAR[i+1];
     MultiByteToWideChar(CP_ACP, 0, str, -1, strSrc, i);

     //获得临时变量的大小
     i = WideCharToMultiByte(CP_UTF8, 0, strSrc, -1, NULL, 0, NULL, NULL);
     szRes = new TCHAR[i+1];
     int j=WideCharToMultiByte(CP_UTF8, 0, strSrc, -1, szRes, i, NULL, NULL);

     result = szRes;
     delete []strSrc;
     delete []szRes;

     return result;
}

CString CPublicClass::ExtendNameByName(CString strName)
{
	CString strExtendName="";
	// 将Find改为ReverseFind，从后面找 [9/11/2015 yuyin]
	int nFind = strName.ReverseFind('.');
	strExtendName=strName.Right(strName.GetLength()-nFind-1);
	return strExtendName;
}

CString CPublicClass::GetFloderPathByPath(CString strPath)
{
	CString strFloder(_T(""));
	int nFind=strPath.ReverseFind('\\');
	if(nFind>0)
	{
		strFloder=strPath.Left(nFind);

	}
	return strFloder;
}

BOOL CPublicClass::CreateDirectoryAll(const char * szPath)
{
	int index, start = 0;
	CString strTmp;  
	CString strPath = szPath;
	strPath.Replace("/","\\");
	
	while((index = strPath.Find('\\', start)) != -1)
	{  
		strTmp = strPath.Left(index);
		CreateDirectory(strTmp,   NULL);  
		start = index + 1;
	}  
	
	if(strPath.GetLength() != strTmp.GetLength()+1)
		CreateDirectory(strPath, NULL);

	if(GetFileAttributes(szPath)==0xFFFFFFFF)
		return false;

	return true;
}
