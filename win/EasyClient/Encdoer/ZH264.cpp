// ZH264.cpp: implementation of the ZH264 class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ZH264.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

ZH264::ZH264()
{
h=NULL;
m_flv=NULL;
m_mp4=NULL;
}

ZH264::~ZH264()
{
	Clean();
}

void ZH264::Init(ZFLV*flv,ZMP4*mp4,int weidh,int hight,int fps,int bitrate,int leave,int mode)
{
	Clean();
	m_flv=flv;
	m_mp4=mp4;
	yw=weidh;
	yh=hight;
	x264_param_default(&param);
	X264_CONFIG_SET(&param,leave);	
	param.i_width=weidh;
	param.i_height=hight;
	param.i_log_level=X264_LOG_NONE;
//	param.b_repeat_headers=0;

	if (bitrate>0)
	{
		param.rc.i_rc_method=X264_RC_ABR;//平均码率//X264_RC_CRF;//恒定码率
		param.rc.i_bitrate=bitrate;
		param.rc.i_vbv_max_bitrate=bitrate;
	}

//	param.rc.i_rc_method=X264_RC_CRF;//平均码率
//	param.rc.i_bitrate=bitrate;
//	param.rc.i_vbv_max_bitrate=bitrate;
//	param.rc.i_vbv_buffer_size=bitrate;
//	param.i_nal_hrd=X264_NAL_HRD_VBR;
	
	h=x264_encoder_open(&param);
	x264_picture_init(&m_pic);
	x264_picture_alloc(&m_pic,X264_CSP_I420,weidh,hight);
	int i_nal=0;
	x264_encoder_headers(h,&nal,&i_nal);
	if(m_flv)
	m_flv->InitH264((unsigned char*)nal[0].p_payload+4,nal[0].i_payload-4,(unsigned char*)nal[1].p_payload+4,nal[1].i_payload-4,clock());
	if(m_mp4)
	m_mp4->InitH264((unsigned char*)nal[0].p_payload+4,nal[0].i_payload-4,(unsigned char*)nal[1].p_payload+4,nal[1].i_payload-4,clock());
}

void ZH264::Clean()
{
	if (h!=NULL)
	{
		x264_encoder_close(h);
		x264_picture_clean(&m_pic);	
		h=NULL;
		m_flv=NULL;
		m_mp4=NULL;
	}
}


void ZH264::H264Encode(BYTE*indata,int inlen)
{
	int i_nal=0;
	int i;

//	TRACE("ZH264GetFrame");
	
	memcpy(m_pic.img.plane[0],indata,yw*yh);
	memcpy(m_pic.img.plane[1],indata+yw*yh,yw*yh/4);
	memcpy(m_pic.img.plane[2],indata+yw*yh/4*5,yw*yh/4);
	long size=x264_encoder_encode(h,&nal,&i_nal,&m_pic,&m_picout);
	m_pic.i_pts++;

	if(size<1)
		return;
//	if(m_mp4&&size>0)
//	m_mp4->GetH264Data((unsigned char*)nal[0].p_payload,size,false,clock());

//	return;


//	if (m_mp4)
//	{
//		m_mp4->GetH264Data((unsigned char*)nal[0].p_payload,size,false,clock());
//	}


	for (i=0;i<i_nal;i++)
	{
		if (m_mp4)
		{
			if (nal[i].p_payload[2]==0x01)
			{
				BYTE*tempbuff=(byte*)malloc(nal[i].i_payload+4);
				UINT size=nal[i].i_payload-3;
				UINT size2=HTON32(1);
				memcpy(tempbuff,&size2,4);
				memcpy(tempbuff+4,nal[i].p_payload+3,size);
				m_mp4->GetH264Data(tempbuff,size+4,false,clock());
				free(tempbuff);
			//	m_mp4->GetH264Data((unsigned char*)nal[i].p_payload,nal[i].i_payload,false,clock());
			}else
			{
				BYTE*tempbuff=(byte*)malloc(nal[i].i_payload+4);
				UINT size=nal[i].i_payload-4;
				UINT size2=HTON32(1);
				memcpy(tempbuff,&size2,4);
				memcpy(tempbuff+4,nal[i].p_payload+4,size);
				m_mp4->GetH264Data(tempbuff,size+4,false,clock());
				free(tempbuff);
			//	m_mp4->GetH264Data((unsigned char*)nal[i].p_payload+1,nal[i].i_payload-1,false,clock());
			}
		}
		


		if (nal[i].i_type==NAL_SLICE)
		{
			if(m_flv)
			m_flv->GetH264Data((unsigned char*)nal[i].p_payload+4,nal[i].i_payload-4,false,clock());
		//	if(m_mp4)
		//	m_mp4->GetH264Data((unsigned char*)nal[i].p_payload,nal[i].i_payload,false,clock());
		}else
		if (nal[i].i_type==NAL_SLICE_IDR)
		{	if(m_flv)
			m_flv->GetH264Data((unsigned char*)nal[i].p_payload+3,nal[i].i_payload-3,true,clock());
		//	m_flv->GetH264Data((unsigned char*)nal[i].p_payload+4,nal[i].i_payload-4,true,clock());
		//	if(m_mp4)
		//	m_mp4->GetH264Data((unsigned char*)nal[i].p_payload,nal[i].i_payload,true,clock());
		}
	}
}

void X264_CONFIG_SET(x264_param_t*param,int mode)
{
	switch (mode)
	{
	case ZH264_ULTRAFAST:
		{
			param->i_frame_reference = 1;
			param->i_scenecut_threshold = 0;
			param->b_deblocking_filter = 0;
			param->b_cabac = 0;
			param->i_bframe = 0;
			param->analyse.intra = 0;
			param->analyse.inter = 0;
			param->analyse.b_transform_8x8 = 0;
			param->analyse.i_me_method = X264_ME_DIA;
			param->analyse.i_subpel_refine = 0;
			param->rc.i_aq_mode = 0;
			param->analyse.b_mixed_references = 0;
			param->analyse.i_trellis = 0;
			param->i_bframe_adaptive = X264_B_ADAPT_NONE;
			param->rc.b_mb_tree = 0;
			param->analyse.i_weighted_pred = X264_WEIGHTP_NONE;
			param->analyse.b_weighted_bipred = 0;
			param->rc.i_lookahead = 0;
		}
		break;
	case ZH264_SUPERFAST:
		{
			param->analyse.inter = X264_ANALYSE_I8x8|X264_ANALYSE_I4x4;
			param->analyse.i_me_method = X264_ME_DIA;
			param->analyse.i_subpel_refine = 1;
			param->i_frame_reference = 1;
			param->analyse.b_mixed_references = 0;
			param->analyse.i_trellis = 0;
			param->rc.b_mb_tree = 0;
			param->analyse.i_weighted_pred = X264_WEIGHTP_SIMPLE;
			param->rc.i_lookahead = 0;
		}
		break;
	case ZH264_VERYFAST:
		{
			param->analyse.i_me_method = X264_ME_HEX;
			param->analyse.i_subpel_refine = 2;
			param->i_frame_reference = 1;
			param->analyse.b_mixed_references = 0;
			param->analyse.i_trellis = 0;
			param->analyse.i_weighted_pred = X264_WEIGHTP_SIMPLE;
			param->rc.i_lookahead = 10;
		}
		break;
	case ZH264_FASTER:
		{
			param->analyse.b_mixed_references = 0;
			param->i_frame_reference = 2;
			param->analyse.i_subpel_refine = 4;
			param->analyse.i_weighted_pred = X264_WEIGHTP_SIMPLE;
			param->rc.i_lookahead = 20;			
		}
		break;
	case ZH264_FAST:
		{
			param->i_frame_reference = 2;
			param->analyse.i_subpel_refine = 6;
			param->analyse.i_weighted_pred = X264_WEIGHTP_SIMPLE;
			param->rc.i_lookahead = 30;
		}
		break;
	}
}
