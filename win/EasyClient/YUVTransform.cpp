/*
	Copyright (c) 2013-2014 EasyDarwin.ORG.  All rights reserved.
	Github: https://github.com/EasyDarwin
	WEChat: EasyDarwin
	Website: http://www.EasyDarwin.org
*/
// Add by SwordTwelve

#include "StdAfx.h"
#include "YUVTransform.h"


#define uint8_t BYTE

void YUY2toI420(int inWidth, int inHeight, uint8_t *pSrc, uint8_t *pDest)
{
	int i, j;
	long temp=inWidth*inHeight;
	uint8_t *u = pDest + temp;
	uint8_t *v = u + (temp>>2);
	
	int halfWidth=inWidth>>1;
	int halfHeight=inHeight>>1;
	uint8_t *src_l1;
	uint8_t *src_l2;
	uint8_t *y_l1;
	uint8_t *y_l2;
	for (i = 0; i < halfHeight; i++)
	{
		
		src_l1 = pSrc + (inWidth<<2)*i;
		src_l2 = src_l1 + (inWidth<<1);
		y_l1 = pDest + (inWidth<<1)*i;
		y_l2 = y_l1 + inWidth;
		for (j = 0; j < halfWidth; j++)
		{
			// two pels in one go
			*y_l1++ = src_l1[0];
			*u++ = src_l1[1];
			*y_l1++ = src_l1[2];
			*v++ = src_l1[3];
			*y_l2++ = src_l2[0];
			*y_l2++ = src_l2[2];
			src_l1 += 4;
			src_l2 += 4;
		}
	}  
}

void YUY2toYV12(int inWidth, int inHeight, uint8_t *pSrc, uint8_t *pDest)
{
	int i, j;
	long temp=inWidth*inHeight;
	uint8_t *u = pDest + temp;
	uint8_t *v = u + (temp>>2);
	
	int halfWidth=inWidth>>1;
	int halfHeight=inHeight>>1;
	uint8_t *src_l1;
	uint8_t *src_l2;
	uint8_t *y_l1;
	uint8_t *y_l2;
	for (i = 0; i < halfHeight; i++)
	{
		
		src_l1 = pSrc + (inWidth<<2)*i;
		src_l2 = src_l1 + (inWidth<<1);
		y_l1 = pDest + (inWidth<<1)*i;
		y_l2 = y_l1 + inWidth;
		for (j = 0; j < halfWidth; j++)
		{
			// two pels in one go
			*y_l1++ = src_l1[0];
			*u++ = src_l1[3];
			*y_l1++ = src_l1[2];
			*v++ = src_l1[1];
			*y_l2++ = src_l2[0];
			*y_l2++ = src_l2[2];
			src_l1 += 4;
			src_l2 += 4;
		}
	}  
}