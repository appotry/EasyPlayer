
// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently,
// but are changed infrequently

#pragma once

#ifndef _SECURE_ATL
#define _SECURE_ATL 1
#endif

#ifndef VC_EXTRALEAN
#define VC_EXTRALEAN            // Exclude rarely-used stuff from Windows headers
#endif

#include "targetver.h"

#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS      // some CString constructors will be explicit

// turns off MFC's hiding of some common and often safely ignored warning messages
#define _AFX_ALL_WARNINGS

#include <afxwin.h>         // MFC core and standard components
#include <afxext.h>         // MFC extensions



#ifndef _AFX_NO_OLE_SUPPORT
#include <afxdtctl.h>           // MFC support for Internet Explorer 4 Common Controls
#endif
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>             // MFC support for Windows Common Controls
#endif // _AFX_NO_AFXCMN_SUPPORT

<<<<<<< HEAD
#include <afxcontrolbars.h>     // MFC support for ribbons and control bars


#define	VIDEO_WINDOW_BORDER_WIDTH	3

#define __MOVE_WINDOW(x,rect)	{if (NULL!=x){x->MoveWindow(&rect);}}//x->Invalidate();}}
#define __DESTROY_WINDOW(x)	{if (NULL!=x){x->DestroyWindow();delete x;x=NULL;}}


bool __WCharToMByte(LPCWSTR lpcwszStr, LPSTR lpszStr, DWORD dwSize);
bool __MByteToWChar(LPCSTR lpcszStr, LPWSTR lpwszStr, DWORD dwSize);

=======
//#include <afxcontrolbars.h> // MFC support for ribbons and control bars
#define CWinAppEx CWinApp
>>>>>>> df8a7330224cac3307df9716c34f8b81ae944af4

#define	WM_UPDATE_LOCAL_VIDEO		(WM_USER+6001)
#ifdef _UNICODE
#if defined _M_IX86
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='x86' publicKeyToken='6595b64144ccf1df' language='*'\"")
#elif defined _M_X64
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='amd64' publicKeyToken='6595b64144ccf1df' language='*'\"")
#else
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")
#endif
#endif


