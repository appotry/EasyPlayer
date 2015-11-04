
#include "StdAfx.h"
#include "ZMP4ByGPAC.h"
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
ZMP4ByGPAC::ZMP4ByGPAC()
{
	m_videtrackid=-1;
	m_audiotrackid=-1;
	m_audiostartimestamp=-1;
	m_videostartimestamp=-1;

	m_flag=0;
	p_file=NULL;
	p_config=NULL;
	p_videosample=NULL;
	p_audiosample=NULL;
}

ZMP4ByGPAC::~ZMP4ByGPAC()
{
	SaveFile();

}
bool ZMP4ByGPAC::CreateFile(char*filename,int flag)
{
	SaveFile();
	m_audiostartimestamp=-1;
	m_videostartimestamp=-1;
	if(filename==NULL)
	{
		char filename2[256]={0};
		sprintf(filename2,"%d-gpac%d.mp4",time(NULL),rand());
		p_file=gf_isom_open(filename2,GF_ISOM_OPEN_WRITE,NULL);//打开文件
	}else
		p_file=gf_isom_open(filename,GF_ISOM_OPEN_WRITE,NULL);//打开文件

	if (p_file==NULL)
	{
		return false;
	}

	gf_isom_set_brand_info(p_file,GF_ISOM_BRAND_MP42,0);

	if(flag&ZOUTFILE_FLAG_VIDEO)
	{
		m_videtrackid=gf_isom_new_track(p_file,0,GF_ISOM_MEDIA_VISUAL,1000);
		gf_isom_set_track_enabled(p_file,m_videtrackid,1);
	}
	if(flag&ZOUTFILE_FLAG_AUDIO)
	{
		m_audiotrackid=gf_isom_new_track(p_file,0,GF_ISOM_MEDIA_AUDIO,1000);
		gf_isom_set_track_enabled(p_file,m_audiotrackid,1);
	}
	return true;
}
//sps,pps第一个字节为0x67或68,
bool ZMP4ByGPAC::WriteH264SPSandPPS(unsigned char*sps,int spslen,unsigned char*pps,int ppslen,int width,int height)
{	

	p_videosample=gf_isom_sample_new();
	p_videosample->data=(char*)malloc(1024*1024);


	p_config=gf_odf_avc_cfg_new();	
	gf_isom_avc_config_new(p_file,m_videtrackid,p_config,NULL,NULL,&i_videodescidx);
	gf_isom_set_visual_info(p_file,m_videtrackid,i_videodescidx,width,height);
	
	GF_AVCConfigSlot m_slotsps={0};
	GF_AVCConfigSlot m_slotpps={0};
	
	p_config->configurationVersion = 1;
	p_config->AVCProfileIndication = sps[1];
	p_config->profile_compatibility = sps[2];
	p_config->AVCLevelIndication = sps[3];
	
	m_slotsps.size=spslen;
	m_slotsps.data=(char*)malloc(spslen);
	memcpy(m_slotsps.data,sps,spslen);	
	gf_list_add(p_config->sequenceParameterSets,&m_slotsps);
	
	m_slotpps.size=ppslen;
	m_slotpps.data=(char*)malloc(ppslen);
	memcpy(m_slotpps.data,pps,ppslen);
	gf_list_add(p_config->pictureParameterSets,&m_slotpps);
	
	gf_isom_avc_config_update(p_file,m_videtrackid,1,p_config);

	free(m_slotsps.data);
	free(m_slotpps.data);

	return true;
}
//写入AAC信息
bool ZMP4ByGPAC::WriteAACInfo(unsigned char*info,int len)
{
	p_audiosample=gf_isom_sample_new();
	p_audiosample->data=(char*)malloc(1024*10);

	GF_ESD*esd=	gf_odf_desc_esd_new(0);
	esd->ESID=gf_isom_get_track_id(p_file,m_audiotrackid);
	esd->OCRESID=gf_isom_get_track_id(p_file,m_audiotrackid);
	esd->decoderConfig->streamType=0x05;
	esd->decoderConfig->objectTypeIndication=0x40;//0x40;
	esd->slConfig->timestampResolution=1000;//1000;//时间单元	
	esd->decoderConfig->decoderSpecificInfo=(GF_DefaultDescriptor*)gf_odf_desc_new(GF_ODF_DSI_TAG);

	esd->decoderConfig->decoderSpecificInfo->data=(char*)malloc(len);
	memcpy(esd->decoderConfig->decoderSpecificInfo->data,info,len);
	esd->decoderConfig->decoderSpecificInfo->dataLength=len;
	

	GF_Err gferr=gf_isom_new_mpeg4_description(p_file, m_audiotrackid, esd,  NULL, NULL, &i_audiodescidx);
	if (gferr!=0)
	{
//		TRACE("mpeg4_description:%d\n",gferr);
	}
	gferr=gf_isom_set_audio_info(p_file,m_audiotrackid,i_audiodescidx,44100,2,16);
	if (gferr!=0)
	{
//		TRACE("gf_isom_set_audio:%d\n",gferr);
	}

	free(esd->decoderConfig->decoderSpecificInfo->data);


	return true;
}
//写入一帧，前四字节为该帧NAL长度
bool ZMP4ByGPAC::WriteH264Frame(unsigned char*data,int len,bool keyframe,long timestamp)
{		
	if (m_videostartimestamp==-1&&keyframe)
	{
		m_videostartimestamp=timestamp;
	}
	if (m_videostartimestamp!=-1)
	{
		p_videosample->IsRAP=keyframe;
		p_videosample->dataLength=len;
		memcpy(p_videosample->data,data,len);
		p_videosample->DTS=timestamp-m_videostartimestamp;
		p_videosample->CTS_Offset=0;	
		GF_Err gferr=gf_isom_add_sample(p_file,m_videtrackid,i_videodescidx,p_videosample);			
		if (gferr==-1)
		{
			p_videosample->DTS=timestamp-m_videostartimestamp+15;
			gf_isom_add_sample(p_file,m_videtrackid,i_videodescidx,p_videosample);
		}
	}
	
	return true;
}
//写入aac数据，只有raw数据
bool ZMP4ByGPAC::WriteAACFrame(unsigned char*data,int len,long timestamp)
{
		if (m_audiostartimestamp==-1)
		{
			m_audiostartimestamp=timestamp;
		}
		p_audiosample->IsRAP=1;
		p_audiosample->dataLength=len;
		memcpy(	p_audiosample->data,data,len);
		p_audiosample->DTS=timestamp-m_audiostartimestamp;
		p_audiosample->CTS_Offset=0;	
		GF_Err gferr=gf_isom_add_sample(p_file,m_audiotrackid,i_audiodescidx,p_audiosample);			
		if (gferr==-1)
		{
			p_audiosample->DTS=timestamp-m_audiostartimestamp+15;
			gf_isom_add_sample(p_file,m_audiotrackid,i_audiodescidx,p_audiosample);
		}
	return true;
}
//保存文件
bool ZMP4ByGPAC::SaveFile()
{

	m_audiostartimestamp=-1;
	m_videostartimestamp=-1;
	if (p_file)
	{
		gf_isom_close(p_file);
		p_file=NULL;
	}
	if(p_config)
	{
	//	delete p_config->pictureParameterSets;
		p_config->pictureParameterSets=NULL;
	//	delete p_config->sequenceParameterSets;
		p_config->sequenceParameterSets=NULL;
		gf_odf_avc_cfg_del(p_config);
		p_config=NULL;
	}
	if(	p_audiosample)
	{
		if(	p_audiosample->data)
		{
			free(p_audiosample->data);
			p_audiosample->data=NULL;
		}
		gf_isom_sample_del(&p_audiosample);
		p_audiosample=NULL;
	}

	if(	p_videosample)
	{
		if(	p_videosample->data)
		{
			free(p_videosample->data);
			p_videosample->data=NULL;
		}
		gf_isom_sample_del(&p_videosample);
		p_audiosample=NULL;
	}
	return true;
}

bool ZMP4ByGPAC::CanWrite()
{
	return true;
}