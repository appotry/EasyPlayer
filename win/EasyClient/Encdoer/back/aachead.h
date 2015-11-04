#ifndef AACADSTHEAD_H
#define AACADSTHEAD_H

#define MPEG2 1
#define MPEG4 0

/* AAC object types */
#define MAIN 1
#define LOW  2
#define SSR  3
#define LTP  4

int GetSRIndex(unsigned int sampleRate);

struct FLVAACADST{
	unsigned char SamplIndex1:3;
	unsigned char OBjecttype:5;//2

	unsigned char other:3;//000
	unsigned char channel:4;
	unsigned char SamplIndex2:1;
};

struct AACADST{
	unsigned	char check1;
	
	unsigned    char protection:1;//误码校验1
	unsigned    char layer:2;//哪个播放器被使用0x00
	unsigned	char ver:1;//版本 0 for MPEG-4, 1 for MPEG-2
	unsigned	char check2:4;
	
	
	unsigned	char channel1:1;
	unsigned    char privatestream:1;//0
	unsigned	char SamplingIndex:4;//采样率
	unsigned	char ObjectType:2;
	
	unsigned	char length1:2;
	unsigned	char copyrightstart:1;//0
	unsigned    char copyrightstream:1;//0
	unsigned    char home:1;//0
	unsigned    char originality:1;//0
	unsigned	char channel2:2;
	
	
	unsigned	char length2;
	
	unsigned	char check3:5;
	unsigned	char length3:3;
	
	unsigned	char frames:2;//超过一块写
	unsigned	char check4:6;
};


struct RTPAACADST{
	unsigned char check1:8;//0x00

	unsigned char check2:8;//0x10

	unsigned char length1:8;

	unsigned char check3:3;//0x00
	unsigned char length2:5;
};

#endif