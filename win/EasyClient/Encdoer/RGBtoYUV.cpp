// RGBtoYUV.cpp: implementation of the RGBtoYUV class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "RGBtoYUV.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

// Conversion from RGB to YUV420

/**************************************************************************
* *
* This code is developed by Adam Li. This software is an *
* implementation of a part of one or more MPEG-4 Video tools as *
* specified in ISO/IEC 14496-2 standard. Those intending to use this *
* software module in hardware or software products are advised that its *
* use may infringe existing patents or copyrights, and any such use *
* would be at such party's own risk. The original developer of this *
* software module and his/her company, and subsequent editors and their *
* companies (including Project Mayo), will have no liability for use of *
* this software or modifications or derivatives thereof. *
* *
* Project Mayo gives users of the Codec a license to this software *
* module or modifications thereof for use in hardware or software *
* products claiming conformance to the MPEG-4 Video Standard as *
* described in the Open DivX license. *
* *
* The complete Open DivX license can be found at *
* http://www.projectmayo.com/opendivx/license.php . *
* *
**************************************************************************/

/**************************************************************************
*
* rgb2yuv.c, 24-bit RGB bitmap to YUV converter
*
* Copyright (C) 2001 Project Mayo
*
* Adam Li
*
* DivX Advance Research Center <darc@projectmayo.com>
*
**************************************************************************/

/* This file contains RGB to YUV transformation functions. */

#include "stdlib.h"
//#include "rgb2yuv.h"
#include "../stdafx.h"
static float RGBYUV02990[256], RGBYUV05870[256], RGBYUV01140[256];
static float RGBYUV01684[256], RGBYUV03316[256];
static float RGBYUV04187[256], RGBYUV00813[256];

void InitLookupTable();

/************************************************************************
*
* int RGB2YUV (int x_dim, int y_dim, void *bmp, YUV *yuv)
*
* Purpose : It takes a 24-bit RGB bitmap and convert it into
* YUV (4:2:0) format
*
* Input : x_dim the x dimension of the bitmap
* y_dim the y dimension of the bitmap
* bmp pointer to the buffer of the bitmap
* yuv pointer to the YUV structure
*
* Output : 0 OK
* 1 wrong dimension
* 2 memory allocation error
*
* Side Effect :
* None
*
* Date : 09/28/2000
*
* Contacts:
*
* Adam Li
*
* DivX Advance Research Center <darc@projectmayo.com>
*
************************************************************************/




int RGB24I420 (int x_dim, int y_dim, void *bmp, void *y_out, void *u_out, void *v_out, int flip)
{
static int init_done = 0;

long i, j, size;
unsigned char *r, *g, *b;
unsigned char *y, *u, *v;
unsigned char *pu1, *pu2, *pv1, *pv2, *psu, *psv;
unsigned char *y_buffer, *u_buffer, *v_buffer;
unsigned char *sub_u_buf, *sub_v_buf;

if (init_done == 0)
{
InitLookupTable();
init_done = 1;
}

// check to see if x_dim and y_dim are divisible by 2
if ((x_dim % 2) || (y_dim % 2)) return 1;
size = x_dim * y_dim;

// allocate memory
y_buffer = (unsigned char *)y_out;
sub_u_buf = (unsigned char *)u_out;
sub_v_buf = (unsigned char *)v_out;
u_buffer = (unsigned char *)malloc(size * sizeof(unsigned char));
v_buffer = (unsigned char *)malloc(size * sizeof(unsigned char));
if (!(u_buffer && v_buffer))
{
if (u_buffer) free(u_buffer);
if (v_buffer) free(v_buffer);
return 2;
}

b = (unsigned char *)bmp;
y = y_buffer;
u = u_buffer;
v = v_buffer;

// convert RGB to YUV
if (!flip) {
for (j = 0; j < y_dim; j ++)
{
y = y_buffer + (y_dim - j - 1) * x_dim;
u = u_buffer + (y_dim - j - 1) * x_dim;
v = v_buffer + (y_dim - j - 1) * x_dim;

for (i = 0; i < x_dim; i ++) {
g = b + 1;
r = b + 2;
*y = (unsigned char)( RGBYUV02990[*r] + RGBYUV05870[*g] + RGBYUV01140[*b]);
*u = (unsigned char)(- RGBYUV01684[*r] - RGBYUV03316[*g] + (*b)/2 + 128);
*v = (unsigned char)( (*r)/2 - RGBYUV04187[*g] - RGBYUV00813[*b] + 128);
b += 3;
y ++;
u ++;
v ++;
}
}
} else {
for (i = 0; i < size; i++)
{
g = b + 1;
r = b + 2;
*y = (unsigned char)( RGBYUV02990[*r] + RGBYUV05870[*g] + RGBYUV01140[*b]);
*u = (unsigned char)(- RGBYUV01684[*r] - RGBYUV03316[*g] + (*b)/2 + 128);
*v = (unsigned char)( (*r)/2 - RGBYUV04187[*g] - RGBYUV00813[*b] + 128);
b += 3;
y ++;
u ++;
v ++;
}
}

// subsample UV
for (j = 0; j < y_dim/2; j ++)
{
psu = sub_u_buf + j * x_dim / 2;
psv = sub_v_buf + j * x_dim / 2;
pu1 = u_buffer + 2 * j * x_dim;
pu2 = u_buffer + (2 * j + 1) * x_dim;
pv1 = v_buffer + 2 * j * x_dim;
pv2 = v_buffer + (2 * j + 1) * x_dim;
for (i = 0; i < x_dim/2; i ++)
{
*psu = (*pu1 + *(pu1+1) + *pu2 + *(pu2+1)) / 4;
*psv = (*pv1 + *(pv1+1) + *pv2 + *(pv2+1)) / 4;
psu ++;
psv ++;
pu1 += 2;
pu2 += 2;
pv1 += 2;
pv2 += 2;
}
}

free(u_buffer);
free(v_buffer);

return 0;
}


void InitLookupTable()
{
	int i;
	for (i = 0; i < 256; i++) RGBYUV02990[i] = (float)0.2990 * i;
	for (i = 0; i < 256; i++) RGBYUV05870[i] = (float)0.5870 * i;
	for (i = 0; i < 256; i++) RGBYUV01140[i] = (float)0.1140 * i;
	for (i = 0; i < 256; i++) RGBYUV01684[i] = (float)0.1684 * i;
	for (i = 0; i < 256; i++) RGBYUV03316[i] = (float)0.3316 * i;
	for (i = 0; i < 256; i++) RGBYUV04187[i] = (float)0.4187 * i;
	for (i = 0; i < 256; i++) RGBYUV00813[i] = (float)0.0813 * i;
}

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

void ResizeYUY2Data(byte*indata,int scrwidth,int scrheight,byte*outdata,int width,int height)
{
	int h,w;
	int hb=(scrheight<<10)/height;
	int wb=(scrwidth<<10)/width;
	byte*pscr;
	byte*pdes;
	int sh,sw;
	for (h=0;h<height;h++)
	{
		sh=(h*hb)>>10;
		for (w=0;w<width/2;w++)
		{
			sw=(w*wb)>>10;
			pscr=indata+sh*scrwidth*2+sw*4;
			pdes=outdata+h*width*2+w*4;
			pdes[0]=pscr[0];
			pdes[1]=pscr[1];
			pdes[2]=pscr[2];
			pdes[3]=pscr[3];
		}
	}
}


void ResizeYUY2toI420(byte*indata,int inWidth,int inHeight,byte*outdata,int outWidth,int outHeight)
{
	if(inWidth==outWidth&&inHeight==outHeight)
	{
		YUY2toI420(inWidth, inHeight, indata,outdata);
		return ;
	}

	int h,w;
	int hb=(inHeight<<10)/outHeight;
	int wb=(inWidth<<10)/outWidth;
	byte*pscr;
	
	byte*py,*pu,*pv;
	
	int nArea=outWidth*outHeight;
	byte*posy=outdata;
	byte*posu=outdata+nArea;
	byte*posv=outdata+((nArea*5)>>2);
	
	int noutWidthH=(outWidth>>1);
	int sh,sw;
	for (h=0;h<outHeight;h++)
	{
		sh=(h*hb)>>10;
		for (w=0;w<noutWidthH;w++)
		{
			int noutWidthTmp=h*outWidth;
			sw=(w*wb)>>10;
			pscr=indata+sh*(inWidth<<1)+(sw<<2);
			py=posy=outdata+noutWidthTmp+(w<<1);
			py[0]=pscr[0];
			py[1]=pscr[2];
			if (h%2==0)
			{
				pu=posu+(noutWidthTmp>>2)+w;
				pv=posv+(noutWidthTmp>>2)+w;
				
				pu[0]=pscr[1];
				pv[0]=pscr[3];
			}
		}
	}
}

#define ABS_Risze(x) (x > 0 ? (x) : (-x))
#define clip(a, min, max) (a > max ? max : (a > min ? a : min))
#define INTEGER 1
int gcd(int a, int b)
{
    if(b) 
    {
        return gcd(b, a%b);
    }
    else  
    {
        return a;
    }
}

float S(float x)
{
    if(ABS_Risze(x) < 1.0)
    {
        //1-2*Abs(x)^2+Abs(x)^3
        return (1.0 - 2.0*x*x + ABS_Risze(x*x*x));
    }
    else if(ABS(x) >= 1.0 && ABS_Risze(x) < 2)
    {
        //4-8*Abs(x)+5*Abs(x)^2-Abs(x)^3
        return (4.0 - 8.0*ABS_Risze(x) + 5.0*x*x - ABS_Risze(x*x*x));
    }
    else
    {
        return 0;
    }
}
void Convolution1441(int *A, int *B, int *C, int *D)
{
    int a, b, c, d;
    a = A[0]*B[0] + A[1]*B[4] + A[2]*B[8]  + A[3]*B[12];
    b = A[0]*B[1] + A[1]*B[5] + A[2]*B[9]  + A[3]*B[13];
    c = A[0]*B[2] + A[1]*B[6] + A[2]*B[10] + A[3]*B[14];
    d = A[0]*B[3] + A[1]*B[7] + A[2]*B[11] + A[3]*B[15];
    *D = a*C[0] + b*C[1] + c*C[2] + d*C[3];
}
 
int ResizeI420(int nType,BYTE *indata,int srcWidth,int srcHeight,BYTE *outdata,int outWidth,int outHeight)
{
	unsigned char *pYIn=indata;
	unsigned char *pCbIn=indata+srcWidth*srcHeight;
	unsigned char *pCrIn=indata+((srcWidth*srcHeight*5)>>2);///4;

	int lineY=srcWidth;
	int lineCb=srcWidth>>1;///2;
	int lineCr=lineCb;//srcWidth/2;

	unsigned char *pYOut= outdata;
    unsigned char *pCbOut=outdata+outWidth*outHeight;
	unsigned char *pCrOut=outdata+((outWidth*outHeight*5)>>2);///4;


     float rx = (float)outWidth/(float)srcWidth;
    float ry = (float)outHeight/(float)srcHeight;
    int x = 0, y = 0;
    int xx = 0, yy = 0;
    int outWidth_cr =outWidth>>1;// outWidth / 2;
    int outHeight_cr =outHeight>>1;// outHeight / 2;
    int srcWidth_cr =srcWidth>>1;// srcWidth / 2;
    int srcHeight_cr =srcHeight>>1;// srcHeight / 2;
    float c0, c1, c2, c3;
    float u, v;
//    int uu, vv;
    unsigned char *p = NULL, *p1 = NULL;

   // float tmp1[4], tmp2[1],A[4], B[16], C[4], 
    int   AA[4], BB[16], CC[4],tt2;;//, tt1[4],
    int *SSx1=NULL, *SSy1=NULL;
    int *SSx2=NULL, *SSy2=NULL;
    int *SSx3=NULL, *SSy3=NULL;
    int *SSx4=NULL, *SSy4=NULL;
    int *C0=NULL, *C1=NULL, *C2=NULL, *C3=NULL;
    unsigned char* pend = pYIn + lineY * srcHeight - 1;
    
    int x_ratio = (int) (((srcWidth - 1) << 8) / outWidth);
	int y_ratio = (int) (((srcHeight - 1) << 8) / outHeight);
   // int i, j;

    //int yr, one_min_y_diff, y_diff,y_dif,; 
   // int  one_min_x_diff,  index,y_index, Mult1,xr,Mult2,   Mult3, Mult4, x_diff,
    int *T1=NULL, *T2=NULL, *T3=NULL, *T4=NULL;

    // Rational ratio
    int gcdw = gcd(outWidth, srcWidth);
    int gcdh = gcd(outHeight, srcHeight);
    int rx_num = outWidth/gcdw;
    int rx_den = srcWidth/gcdw;
    int ry_num = outHeight/gcdh;
    int ry_den = srcHeight/gcdh;

    switch(nType)
    {
        case 0:
            // cubic
            for(y=0; y<outHeight; y++)
            {
                for(x=0; x<outWidth; x++)
                {
                    //g(x, y) = f(x/r, y/r) = f(x*rx_den/rx_num, y*ry_den/ry_num)
                    xx = clip((int)((float)x / rx), 0, srcWidth-1);
                    yy = clip((int)((float)y / ry), 0, srcHeight-1);
                    pYOut[y*outWidth+x] = pYIn[yy*lineY + xx];
                }
            }
            for(y=0; y<outHeight_cr; y++)
            {
                for(x=0; x<outWidth_cr; x++)
                {
                    //g(x, y) = f(x/r, y/r) = f(x*rx_den/rx_num, y*ry_den/ry_num)
                    xx = clip((int)((float)x / rx), 0, srcWidth_cr-1);
                    yy = clip((int)((float)y / ry), 0, srcHeight_cr-1);
                    pCbOut[y*outWidth_cr+x] = pCbIn[yy*lineCb + xx];
                }
            }
            for(y=0; y<outHeight_cr; y++)
            {
                for(x=0; x<outWidth_cr; x++)
                {
                    //g(x, y) = f(x/r, y/r) = f(x*rx_den/rx_num, y*ry_den/ry_num)
                    xx = clip((int)((float)x / rx), 0, srcWidth_cr-1);
                    yy = clip((int)((float)y / ry), 0, srcHeight_cr-1);
                    pCrOut[y*outWidth_cr+x] = pCrIn[yy*lineCr + xx];
                }
            }
            break;
        case 1:
            // bilinear
            for(y=0; y<outHeight; y++)
            {
                for(x=0; x<outWidth; x++)
                {
                    //g(x, y) = (1-c0)(1-c2)f(xx,yy) + (1-c2)(1-c1)f(xx,yy+1) + (1-c1)(1-c3)f(xx+1,yy) + (1-c0)(1-c3)f(xx+1,yy+1)
                    c0 = (((float)y / ry) - (int)((float)y / ry));
                    c1 = ((int)((float)y / ry) + 1 - ((float)y / ry));
                    c2 = (((float)x / rx) - (int)((float)x / rx));
                    c3 = ((int)((float)x / rx) + 1 - ((float)x / rx));
                    xx = clip((int)((float)x / rx), 0, srcWidth-1);
                    yy = clip((int)((float)y / ry), 0, srcHeight-1);
                    pYOut[y*outWidth+x] = (float)((1.0-c0)*(1.0-c2)*pYIn[yy*lineY+xx] 
                                                + (1.0-c1)*(1.0-c2)*pYIn[clip(yy+1,0,srcHeight-1)*lineY+xx] 
                                                + (1.0-c1)*(1.0-c3)*pYIn[yy*lineY+clip(xx,0,srcWidth-1)]
                                                + (1.0-c0)*(1.0-c3)*pYIn[clip(yy+1,0,srcHeight-1)*lineY+clip(xx,0,srcWidth-1)]);
                }
            }
            for(y=0; y<outHeight_cr; y++)
            {
                for(x=0; x<outWidth_cr; x++)
                {
                    c0 = (((float)y / ry) - (int)((float)y / ry));
                    c1 = ((int)((float)y / ry) + 1 - ((float)y / ry));
                    c2 = (((float)x / rx) - (int)((float)x / rx));
                    c3 = ((int)((float)x / rx) + 1 - ((float)x / rx));
                    xx = clip((int)((float)x / rx), 0, srcWidth_cr-1);
                    yy = clip((int)((float)y / ry), 0, srcHeight_cr-1);
                    pCbOut[y*outWidth_cr+x] = (float)((1.0-c0)*(1.0-c2)*pCbIn[yy*lineCb+xx] 
                                                + (1.0-c1)*(1.0-c2)*pCbIn[clip(yy+1,0,srcHeight_cr-1)*lineCb+xx] 
                                                + (1.0-c1)*(1.0-c3)*pCbIn[yy*lineCb+clip(xx,0,srcWidth_cr-1)]
                                                + (1.0-c0)*(1.0-c3)*pCbIn[clip(yy+1,0,srcHeight_cr-1)*lineCb+clip(xx,0,srcWidth_cr-1)]);
                }
            }
            for(y=0; y<outHeight_cr; y++)
            {
                for(x=0; x<outWidth_cr; x++)
                {
                    c0 = (((float)y / ry) - (int)((float)y / ry));
                    c1 = ((int)((float)y / ry) + 1 - ((float)y / ry));
                    c2 = (((float)x / rx) - (int)((float)x / rx));
                    c3 = ((int)((float)x / rx) + 1 - ((float)x / rx));
                    xx = clip((int)((float)x / rx), 0, srcWidth_cr-1);
                    yy = clip((int)((float)y / ry), 0, srcHeight_cr-1);
                    pCrOut[y*outWidth_cr+x] = (float)((1.0-c0)*(1.0-c2)*pCrIn[yy*lineCr+xx] 
                                                + (1.0-c1)*(1.0-c2)*pCrIn[clip(yy+1,0,srcHeight_cr-1)*lineCr+xx] 
                                                + (1.0-c1)*(1.0-c3)*pCrIn[yy*lineCr+clip(xx,0,srcWidth_cr-1)]
                                                + (1.0-c0)*(1.0-c3)*pCrIn[clip(yy+1,0,srcHeight_cr-1)*lineCr+clip(xx,0,srcWidth_cr-1)]);
                }
            }
            break;
        case 2:
            // smooth + bilinear
            for(y=0; y<outHeight; y++)
            {
                for(x=0; x<outWidth; x++)
                {
                    //g(x, y) = 10(1-c0)(1-c2)f(xx,yy) + 10(1-c2)(1-c1)f(xx,yy+1) + 10(1-c1)(1-c3)f(xx+1,yy) + 10(1-c0)(1-c3)f(xx+1,yy+1)
                    //        + (2-c0)(1-c2)f(xx, yy-1) + (1-c3)(2-c0)f(xx+1,yy-1) + (1-c0)(2-c3)f(xx+2,yy) + (1-c1)(2-c3)f(xx+2,yy+1)
                    //        + (2-c1)(1-c3)f(xx+1,yy+2) + (2-c1)(1-c2)f(xx,yy+2) + (2-c2)(1-c1)f(xx-1,yy+1) + (2-c2)(1-c0)f(xx-1,yy)
                    c0 = (((float)y / ry) - (int)((float)y / ry));
                    c1 = ((int)((float)y / ry) + 1 - ((float)y / ry));
                    c2 = (((float)x / rx) - (int)((float)x / rx));
                    c3 = ((int)((float)x / rx) + 1 - ((float)x / rx));
                    xx = clip((int)((float)x / rx), 0, srcWidth-1);
                    yy = clip((int)((float)y / ry), 0, srcHeight-1);
                    pYOut[y*outWidth+x] = (float)(16.0*(1.0-c0)*(1.0-c2)*pYIn[yy*lineY+xx] 
                                                + 16.0*(1.0-c1)*(1.0-c2)*pYIn[clip(yy+1,0,srcHeight-1)*lineY+xx] 
                                                + 16.0*(1.0-c1)*(1.0-c3)*pYIn[yy*lineY+clip(xx,0,srcWidth-1)]
                                                + 16.0*(1.0-c0)*(1.0-c3)*pYIn[clip(yy+1,0,srcHeight-1)*lineY+clip(xx,0,srcWidth-1)]
                                                + (2-c0)*(1-c2)*pYIn[clip(yy-1,0,srcHeight-1)*lineY+xx]
                                                + (2-c0)*(1-c3)*pYIn[clip(yy-1,0,srcHeight-1)*lineY+clip(xx+1,0,srcWidth-1)]
                                                + (2-c3)*(1-c0)*pYIn[yy*lineY+clip(xx+2,0,srcWidth-1)]
                                                + (2-c3)*(1-c1)*pYIn[clip(yy+1,0,srcHeight-1)*lineY+clip(xx+2,0,srcWidth-1)]
                                                + (2-c1)*(1-c3)*pYIn[clip(yy+2,0,srcHeight-1)*lineY+clip(xx+1,0,srcWidth-1)]
                                                + (2-c1)*(1-c2)*pYIn[clip(yy+2,0,srcHeight-1)*lineY+xx]
                                                + (2-c2)*(1-c1)*pYIn[clip(yy+1,0,srcHeight-1)*lineY+clip(xx-1,0,srcWidth-1)]
                                                + (2-c2)*(1-c0)*pYIn[yy*lineY+clip(xx-1,0,srcWidth-1)]) / 22.0;
                }
            }
        #if 1
            for(y=0; y<outHeight_cr; y++)
            {
                for(x=0; x<outWidth_cr; x++)
                {
                    c0 = (((float)y / ry) - (int)((float)y / ry));
                    c1 = ((int)((float)y / ry) + 1 - ((float)y / ry));
                    c2 = (((float)x / rx) - (int)((float)x / rx));
                    c3 = ((int)((float)x / rx) + 1 - ((float)x / rx));
                    xx = clip((int)((float)x / rx), 0, srcWidth_cr-1);
                    yy = clip((int)((float)y / ry), 0, srcHeight_cr-1);
                    pCbOut[y*outWidth_cr+x] = (float)(16.0*(1.0-c0)*(1.0-c2)*pCbIn[yy*lineCb+xx] 
                                                + 16.0*(1.0-c1)*(1.0-c2)*pCbIn[clip(yy+1,0,srcHeight_cr-1)*lineCb+xx] 
                                                + 16.0*(1.0-c1)*(1.0-c3)*pCbIn[yy*lineCb+clip(xx,0,srcWidth_cr-1)]
                                                + 16.0*(1.0-c0)*(1.0-c3)*pCbIn[clip(yy+1,0,srcHeight_cr-1)*lineCb+clip(xx,0,srcWidth_cr-1)]
                                                + (2-c0)*(1-c2)*pCbIn[clip(yy-1,0,srcHeight_cr-1)*lineCb+xx]
                                                + (2-c0)*(1-c3)*pCbIn[clip(yy-1,0,srcHeight_cr-1)*lineCb+clip(xx+1,0,srcWidth_cr-1)]
                                                + (2-c3)*(1-c0)*pCbIn[yy*lineCb+clip(xx+2,0,srcWidth_cr-1)]
                                                + (2-c3)*(1-c1)*pCbIn[clip(yy+1,0,srcHeight_cr-1)*lineCb+clip(xx+2,0,srcWidth_cr-1)]
                                                + (2-c1)*(1-c3)*pCbIn[clip(yy+2,0,srcHeight_cr-1)*lineCb+clip(xx+1,0,srcWidth_cr-1)]
                                                + (2-c1)*(1-c2)*pCbIn[clip(yy+2,0,srcHeight_cr-1)*lineCb+xx]
                                                + (2-c2)*(1-c1)*pCbIn[clip(yy+1,0,srcHeight_cr-1)*lineCb+clip(xx-1,0,srcWidth_cr-1)]
                                                + (2-c2)*(1-c0)*pCbIn[yy*lineCb+clip(xx-1,0,srcWidth_cr-1)]) / 22.0;
                }
            }
            for(y=0; y<outHeight_cr; y++)
            {
                for(x=0; x<outWidth_cr; x++)
                {
                    c0 = (((float)y / ry) - (int)((float)y / ry));
                    c1 = ((int)((float)y / ry) + 1 - ((float)y / ry));
                    c2 = (((float)x / rx) - (int)((float)x / rx));
                    c3 = ((int)((float)x / rx) + 1 - ((float)x / rx));
                    xx = clip((int)((float)x / rx), 0, srcWidth_cr-1);
                    yy = clip((int)((float)y / ry), 0, srcHeight_cr-1);
                    pCrOut[y*outWidth_cr+x] = (float)(16.0*(1.0-c0)*(1.0-c2)*pCrIn[yy*lineCr+xx] 
                                                + 16.0*(1.0-c1)*(1.0-c2) *pCrIn[clip(yy+1,0,srcHeight_cr-1)*lineCr+xx] 
                                                + 16.0*(1.0-c1)*(1.0-c3) *pCrIn[yy*lineCr+clip(xx,0,srcWidth_cr-1)]
                                                + 16.0*(1.0-c0)*(1.0-c3) *pCrIn[clip(yy+1,0,srcHeight_cr-1)*lineCr+clip(xx,0,srcWidth_cr-1)]
                                                + (2-c0)*(1-c2)*pCrIn[clip(yy-1,0,srcHeight_cr-1)*lineCr+xx]
                                                + (2-c0)*(1-c3)*pCrIn[clip(yy-1,0,srcHeight_cr-1)*lineCr+clip(xx+1,0,srcWidth_cr-1)]
                                                + (2-c3)*(1-c0)*pCrIn[yy*lineCr+clip(xx+2,0,srcWidth_cr-1)]
                                                + (2-c3)*(1-c1)*pCrIn[clip(yy+1,0,srcHeight_cr-1)*lineCr+clip(xx+2,0,srcWidth_cr-1)]
                                                + (2-c1)*(1-c3)*pCrIn[clip(yy+2,0,srcHeight_cr-1)*lineCr+clip(xx+1,0,srcWidth_cr-1)]
                                                + (2-c1)*(1-c2)*pCrIn[clip(yy+2,0,srcHeight_cr-1)*lineCr+xx]
                                                + (2-c2)*(1-c1)*pCrIn[clip(yy+1,0,srcHeight_cr-1)*lineCr+clip(xx-1,0,srcWidth_cr-1)]
                                                + (2-c2)*(1-c0)*pCrIn[yy*lineCr+clip(xx-1,0,srcWidth_cr-1)]) / 22.0;
                }
            }
        #endif
            break;
        case 3:
            // Cubic Convolution Interpolation for luma
            // bilinear for chroma

        #if !INTEGER
            
            for(y=0; y<outHeight; y++)
            {
                for(x=0; x<outWidth; x++)
                {
                    /*
                    [A]= [ S(u + 1)　S(u + 0)　S(u - 1)　S(u - 2) ]

                    　　┏ f(i-1, j-1)　f(i-1, j+0)　f(i-1, j+1)　f(i-1, j+2) ┓

                    [B]=┃ f(i+0, j-1)　f(i+0, j+0)　f(i+0, j+1)　f(i+0, j+2) ┃

                    　　┃ f(i+1, j-1)　f(i+1, j+0)　f(i+1, j+1)　f(i+1, j+2) ┃

                    　　┗ f(i+2, j-1)　f(i+2, j+0)　f(i+2, j+1)　f(i+2, j+2) ┛

                    　　┏ S(v + 1) ┓

                    [C]=┃ S(v + 0) ┃

                    　　┃ S(v - 1) ┃

                    　　┗ S(v - 2) ┛
                    */
                
                    u = (((float)x / rx) - (int)((float)x / rx));
                    v = (((float)y / ry) - (int)((float)y / ry));
                    xx = clip((int)((float)x / rx), 0, srcWidth-1);
                    yy = clip((int)((float)y / ry), 0, srcHeight-1);
                    A[0] = S(u+1); 
                    A[1] = S(u); 
                    A[2] = S(u-1); 
                    A[3] = S(u-2);
                    B[0] = pYIn[clip(yy-1,0,srcHeight-1)*lineY+clip(xx-1,0,srcWidth-1)];
                    B[1] = pYIn[clip(yy  ,0,srcHeight-1)*lineY+clip(xx-1,0,srcWidth-1)];
                    B[2] = pYIn[clip(yy+1,0,srcHeight-1)*lineY+clip(xx-1,0,srcWidth-1)];
                    B[3] = pYIn[clip(yy+2,0,srcHeight-1)*lineY+clip(xx-1,0,srcWidth-1)];
                    B[4] = pYIn[clip(yy-1,0,srcHeight-1)*lineY+clip(xx  ,0,srcWidth-1)];
                    B[5] = pYIn[clip(yy  ,0,srcHeight-1)*lineY+clip(xx  ,0,srcWidth-1)];
                    B[6] = pYIn[clip(yy+1,0,srcHeight-1)*lineY+clip(xx  ,0,srcWidth-1)];
                    B[7] = pYIn[clip(yy+2,0,srcHeight-1)*lineY+clip(xx  ,0,srcWidth-1)];
                    B[8] = pYIn[clip(yy-1,0,srcHeight-1)*lineY+clip(xx+1,0,srcWidth-1)];
                    B[9] = pYIn[clip(yy  ,0,srcHeight-1)*lineY+clip(xx+1,0,srcWidth-1)];
                    B[10]= pYIn[clip(yy+1,0,srcHeight-1)*lineY+clip(xx+1,0,srcWidth-1)];
                    B[11]= pYIn[clip(yy+2,0,srcHeight-1)*lineY+clip(xx+1,0,srcWidth-1)];
                    B[12]= pYIn[clip(yy-1,0,srcHeight-1)*lineY+clip(xx+2,0,srcWidth-1)];
                    B[13]= pYIn[clip(yy  ,0,srcHeight-1)*lineY+clip(xx+2,0,srcWidth-1)];
                    B[14]= pYIn[clip(yy+1,0,srcHeight-1)*lineY+clip(xx+2,0,srcWidth-1)];
                    B[15]= pYIn[clip(yy+2,0,srcHeight-1)*lineY+clip(xx+2,0,srcWidth-1)];
                    C[0] = S(v+1); 
                    C[1] = S(v); 
                    C[2] = S(v-1); 
                    C[3] = S(v-2);
                    MatrixMultiply(A, B, tmp1, 1, 4, 4);
                    MatrixMultiply(tmp1, C, tmp2, 1, 4, 1);
                    pYOut[y*outWidth+x] = clip(tmp2[0], 0, 255);
                }
            }
            for(y=0; y<outHeight_cr; y++)
            {
                for(x=0; x<outWidth_cr; x++)
                {
                    c0 = (((float)y / ry) - (int)((float)y / ry));
                    c1 = ((int)((float)y / ry) + 1 - ((float)y / ry));
                    c2 = (((float)x / rx) - (int)((float)x / rx));
                    c3 = ((int)((float)x / rx) + 1 - ((float)x / rx));
                    xx = clip((int)((float)x / rx), 0, srcWidth_cr-1);
                    yy = clip((int)((float)y / ry), 0, srcHeight_cr-1);
                    pCbOut[y*outWidth_cr+x] = (float)((1.0-c0)*(1.0-c2)*pCbIn[yy*lineCb+xx] 
                                                + (1.0-c1)*(1.0-c2)*pCbIn[clip(yy+1,0,srcHeight_cr-1)*lineCb+xx] 
                                                + (1.0-c1)*(1.0-c3)*pCbIn[yy*lineCb+clip(xx,0,srcWidth_cr-1)]
                                                + (1.0-c0)*(1.0-c3)*pCbIn[clip(yy+1,0,srcHeight_cr-1)*lineCb+clip(xx,0,srcWidth_cr-1)]);
                }
            }
            for(y=0; y<outHeight_cr; y++)
            {
                for(x=0; x<outWidth_cr; x++)
                {
                    c0 = (((float)y / ry) - (int)((float)y / ry));
                    c1 = ((int)((float)y / ry) + 1 - ((float)y / ry));
                    c2 = (((float)x / rx) - (int)((float)x / rx));
                    c3 = ((int)((float)x / rx) + 1 - ((float)x / rx));
                    xx = clip((int)((float)x / rx), 0, srcWidth_cr-1);
                    yy = clip((int)((float)y / ry), 0, srcHeight_cr-1);
                    pCrOut[y*outWidth_cr+x] = (float)((1.0-c0)*(1.0-c2)*pCrIn[yy*lineCr+xx] 
                                                + (1.0-c1)*(1.0-c2)*pCrIn[clip(yy+1,0,srcHeight_cr-1)*lineCr+xx] 
                                                + (1.0-c1)*(1.0-c3)*pCrIn[yy*lineCr+clip(xx,0,srcWidth_cr-1)]
                                                + (1.0-c0)*(1.0-c3)*pCrIn[clip(yy+1,0,srcHeight_cr-1)*lineCr+clip(xx,0,srcWidth_cr-1)]);
                }
            }
        #else
            p = pYOut;
            T3 =   (int*)malloc(outWidth*sizeof(int));
            T4 =   (int*)malloc(outHeight*sizeof(int));
            SSx1 = (int*)malloc(outWidth*sizeof(int));
            SSx2 = (int*)malloc(outWidth*sizeof(int));
            SSx3 = (int*)malloc(outWidth*sizeof(int));
            SSx4 = (int*)malloc(outWidth*sizeof(int));
            SSy1 = (int*)malloc(outHeight*sizeof(int));
            SSy2 = (int*)malloc(outHeight*sizeof(int));
            SSy3 = (int*)malloc(outHeight*sizeof(int));
            SSy4 = (int*)malloc(outHeight*sizeof(int));
            for(x=0; x<outWidth; x++)
            {
                u = (((float)x / rx) - (int)((float)x / rx));
                T3[x] = clip((float)x / rx, 1, srcWidth-3);
                SSx1[x] = (int)((float)(S(u+1)*256.0));
                SSx2[x] = (int)((float)(S(u)*256.0));
                SSx3[x] = (int)((float)(S(u-1)*256.0));
                SSx4[x] = (int)((float)(S(u-2)*256.0));
            }
            for(y=0; y<outHeight; y++)
            {
                v = (((float)y / ry) - (int)((float)y / ry));
                T4[y] = clip((float)y / ry, 1, srcHeight-3);
                T4[y] *= lineY;
                SSy1[y] = (int)((float)(S(v+1)*256.0));
                SSy2[y] = (int)((float)(S(v)*256.0));
                SSy3[y] = (int)((float)(S(v-1)*256.0));
                SSy4[y] = (int)((float)(S(v-2)*256.0));
            }
            T1 =   (int*)malloc(outWidth_cr*sizeof(int));
            T2 =   (int*)malloc(outHeight_cr*sizeof(int));
            for(x=0; x<outWidth_cr; x++)
            {
                T1[x] = clip((float)x / rx, 0, srcWidth_cr-1);
            }
            for(y=0; y<outHeight_cr; y++)
            {
                T2[y] = clip((float)y / ry, 0, srcHeight_cr-1);
                T2[y] *= lineCb;
            }

            for(y=0; y<outHeight; y++)
            {
                yy    = T4[y];
                CC[0] = SSy1[y]; 
                CC[1] = SSy2[y]; 
                CC[2] = SSy3[y]; 
                CC[3] = SSy4[y];
                
                for(x=0;x<outWidth; x++)
                {
                    p1 = &pYIn[yy + T3[x]];
                    
                    AA[0] = SSx1[x]; 
                    AA[1] = SSx2[x]; 
                    AA[2] = SSx3[x]; 
                    AA[3] = SSx4[x];
                #if 0
                    BB[0] = *clip((p1-lineY-1), pYIn, pend);
                    BB[1] = *clip((p1-1), pYIn, pend);
                    BB[2] = *clip((p1+lineY-1), pYIn, pend);
                    BB[3] = *clip((p1+lineY+lineY-1), pYIn, pend);
                    BB[4] = *clip((p1-lineY), pYIn, pend);
                    BB[5] = * p1;
                    BB[6] = *clip((p1+lineY), pYIn, pend);
                    BB[7] = *clip((p1+lineY+lineY), pYIn, pend);
                    BB[8] = *clip((p1-lineY+1), pYIn, pend);
                    BB[9] = *clip((p1+1), pYIn, pend);
                    BB[10]= *clip((p1+lineY+1), pYIn, pend);
                    BB[11]= *clip((p1+lineY+lineY+1), pYIn, pend);
                    BB[12]= *clip((p1-lineY+2), pYIn, pend);
                    BB[13]= *clip((p1+2), pYIn, pend);
                    BB[14]= *clip((p1+lineY+2), pYIn, pend);
                    BB[15]= *clip((p1+lineY+lineY+2), pYIn, pend);
                #else
                    BB[0] = *(p1-lineY-1);
                    BB[1] = *(p1-1);
                    BB[2] = *(p1+lineY-1);
                    BB[3] = *(p1+lineY+lineY-1);
                    BB[4] = *(p1-lineY);
                    BB[5] = * p1;
                    BB[6] = *(p1+lineY);
                    BB[7] = *(p1+lineY+lineY);
                    BB[8] = *(p1-lineY+1);
                    BB[9] = *(p1+1);
                    BB[10]= *(p1+lineY+1);
                    BB[11]= *(p1+lineY+lineY+1);
                    BB[12]= *(p1-lineY+2);
                    BB[13]= *(p1+2);
                    BB[14]= *(p1+lineY+2);
                    BB[15]= *(p1+lineY+lineY+2);
                #endif
                    
                    //MatrixMultiply_Int(AA, BB, tt1, 1, 4, 4);
                    //MatrixMultiply_Int(tt1, CC, &tt2, 1, 4, 1);
                    //t4 = get_time();
                    Convolution1441(AA, BB, CC, &tt2);
                    //t_conv += get_time() - t4;
                    *p++ = clip((tt2>>16), 0, 255);
                }
            }
            //p = pCbOut;
            for(y=0; y<outHeight_cr; y++)
            {
                tt2 = T2[y];
                for(x=0; x<outWidth_cr; x++)
                {
                    *pCbOut++ = pCbIn[tt2 + T1[x]];
					*pCrOut++ = pCrIn[tt2 + T1[x]];
                }
            }
        #endif
            break;
        default:
            return -1;
    }
    if(T1)free(T1);
    if(T2)free(T2);
    if(T3)free(T3);
    if(T4)free(T4);
    if(SSx1)free(SSx1);
    if(SSx2)free(SSx2);
    if(SSx3)free(SSx3);
    if(SSx4)free(SSx4);
    if(SSy1)free(SSy1);
    if(SSy2)free(SSy2);
    if(SSy3)free(SSy3);
    if(SSy4)free(SSy4);
    if(C0)free(C0);
    if(C1)free(C1);
    if(C2)free(C2);
    if(C3)free(C3);
    
    return 0;
}

//4==原始缩放(抽点的方式) 0--cubic 1--bilinear 2--smooth+bilinear 3--cubic convolution
void ResizeYUY2toI420_Scale(byte*indata,int inWidth,int inHeight,byte*outdata,int outWidth,int outHeight,int nInSize,int nType)
{
	if(nType==4)
	{
		ResizeYUY2toI420(indata,inWidth,inHeight,outdata,outWidth,outHeight);
		return ;
	}
	if(inWidth==outWidth&&inHeight==outHeight)
	{
		YUY2toI420(inWidth, inHeight, indata,outdata);
		return ;
	}
	BYTE *pInDataI420=NULL;
	if(nInSize<=0)
	{
		nInSize=(inWidth*inHeight*3>>1);
	}
	pInDataI420=new BYTE[nInSize];
	memset(pInDataI420,0x00,nInSize);
	YUY2toI420(inWidth, inHeight, indata,pInDataI420);
	
	int nRet = ResizeI420(nType,pInDataI420,inWidth,inHeight,outdata,outWidth,outHeight);
	if(pInDataI420)
	{
		delete[] pInDataI420;
		pInDataI420=NULL;
	}
}

