// RGBtoYUV.h: interface for the RGBtoYUV class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_RGBTOYUV_H__999B80FB_D5EF_4955_895A_8189521E2D93__INCLUDED_)
#define AFX_RGBTOYUV_H__999B80FB_D5EF_4955_895A_8189521E2D93__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include<stdio.h>
#include "gpac/isomedia.h"
#include "gpac/setup.h"


// Conversion from RGB24 to YUV420
int  RGB24I420(int x_dim, int y_dim, void *bmp, void *y_out, void *u_out, void *v_out, int flip);
void YUY2toI420(int inWidth, int inHeight, unsigned char* pSrc, unsigned char* pDest);
void ResizeYUY2Data(byte*indata,int scrwidth,int scrheight,byte*outdata,int width,int height);
void ResizeYUY2toI420(byte*indata,int inWidth,int inHeight,byte*outdata,int outWidth,int outHeight);
int ResizeI420(int nType,BYTE *indata,int srcWidth,int srcHeight,BYTE *outdata,int outWidth,int outHeight);
void ResizeYUY2toI420_Scale(byte*indata,int inWidth,int inHeight,byte*outdata,int outWidth,int outHeight,int nInSize=0,int nType=4);
#endif