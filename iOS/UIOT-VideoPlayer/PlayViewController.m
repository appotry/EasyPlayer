//
//  ViewController.m
//  H264DecodeDemo
//
//  Created by Yao Dong on 15/8/6.
//  Copyright (c) 2015年 duowan. All rights reserved.
//



#define CAMERA_TURN_SERVICE_PORT 6899
#define RTSP_CONN_TIMEOUT 5
#define RTSP_CLOSE_DETECT_INTERVAL_MICSEC 500
#define Cloud_Stop 0

#define Cloud_Up 1
#define Cloud_Down 2
#define Cloud_Left 3
#define Cloud_Right 4

#define Cloud_Foucs 7
#define Cloud_Spread 8
#define Cloud_StopFoucs 9
#define Cloud_StopSpread 10
#define Cloud_Photo 11
#define Cloud_Time 5 //倒计时


#import "PlayViewController.h"
#import <VideoToolbox/VideoToolbox.h>
#import <AudioToolbox/AudioToolbox.h>
#import <MediaPlayer/MediaPlayer.h>
#import <AVFoundation/AVFoundation.h>

#import "VideoPacket.h"
#import "AAPLEAGLLayer.h"
#import "EasyRTSPClientAPI.h"
#import "ToolBar.h"
#import "EasyUrl.h"
#import "CallBySocket.h"





BOOL isSaveAudioAddVideo; //是否开始保存视频
UIActivityIndicatorView *_activityIndicatorView;
BOOL isTakePhoto;//是否抓拍
UIImageView *_imgView;//显示图片img

//视频
Easy_RTSP_Handle	rtspHandle = NULL;
uint8_t *_sps=NULL;
NSInteger _spsSize;
uint8_t *_pps=NULL;
NSInteger _ppsSize;

uint8_t *_spsTemp=NULL;
NSInteger _spsTempSize;
uint8_t *_ppsTemp=NULL;
NSInteger _ppsTempSize;
//添加缓冲队列实现两个回调函数的数据传递
char sIfHaveDone=0;
struct Node
{
    char *data;//音频数据
    int len;//音频数据长度
    struct Node *pNext;//指向下一个结点的指针
};
NSLock *synlockEx ;///同步控制
struct List//以单链表实现队列
{
    int NodeLen;//结点个数
    struct Node *pHead;//队列头，每次从队列头获取数据
    struct Node *pTail;//队列尾，每次从队列尾添加数据
};
void Init(struct List * plist)//队列初始化
{
    synlockEx = [[NSLock alloc] init];//互斥锁
    plist->NodeLen=0;
    plist->pHead=NULL;
    plist->pTail=NULL;
}
void  AddToTail(struct List * plist,struct Node *pNode)//将结点插入到队列尾部
{
    [synlockEx lock];
    pNode->pNext=NULL;
    if(plist->NodeLen==0)//空
    {
        plist->pHead=pNode;
        plist->pTail=pNode;
    }
    else
    {
        plist->pTail->pNext=pNode;
        plist->pTail=pNode;
    }
    plist->NodeLen=plist->NodeLen+1;
    [synlockEx unlock];
}
void ClearAllData(struct List * plist)
{
    [synlockEx lock];
    struct Node *pTemp=NULL;
    while(plist->NodeLen>0)
    {
        plist->NodeLen=plist->NodeLen-1;
        pTemp=plist->pHead;
        plist->pHead=plist->pHead->pNext;
        free(pTemp->data);
        free(pTemp);
    }
    [synlockEx unlock];
}
struct Node* GetFromHead(struct List * plist)//从队列头部获取数据
{
    struct Node *pTemp=NULL;
    [synlockEx lock];
    if(plist->NodeLen==0)
    {
        pTemp= NULL;
    }
    else if(plist->NodeLen==1)
    {
        /*
         plist->NodeLen=0;
         pTemp=plist->pHead;
         plist->pHead=NULL;
         plist->pTail=NULL;
         */
        //add,当队列为1时不移除这个元素
        struct Node * dataNode=plist->pHead;;
        pTemp=malloc(sizeof(struct Node));
        pTemp->len=dataNode->len;
        pTemp->data= malloc(dataNode->len);
        memcpy(pTemp->data, dataNode->data, dataNode->len);
    }
    else
    {
        plist->NodeLen=plist->NodeLen-1;
        pTemp=plist->pHead;
        plist->pHead=plist->pHead->pNext;
    }
    [synlockEx unlock];
    return pTemp;
}
struct List  MyList;
//
VTDecompressionSessionRef _deocderSession;
CMVideoFormatDescriptionRef _decoderFormatDescription;

AAPLEAGLLayer *_glLayer;

CVPixelBufferRef decode(VideoPacket* vp);
const uint8_t KStartCode[3] = {0,0,1};

static void didDecompress( void *decompressionOutputRefCon, void *sourceFrameRefCon, OSStatus status, VTDecodeInfoFlags infoFlags, CVImageBufferRef pixelBuffer, CMTime presentationTimeStamp, CMTime presentationDuration ){
    
    CVPixelBufferRef *outputPixelBuffer = (CVPixelBufferRef *)sourceFrameRefCon;
    *outputPixelBuffer = CVPixelBufferRetain(pixelBuffer);
}
void creatImgs(UIImage *img);
BOOL initH264Decoder( );


// 音频
#define QUEUE_BUFFER_SIZE 5 //队列缓冲个数
#define MIN_SIZE_PER_FRAME 2000 //每侦最小数据长度

AudioStreamBasicDescription audioDescription;///音频参数
AudioQueueRef audioQueue;//音频播放队列

AudioQueueBufferRef outQB[QUEUE_BUFFER_SIZE];//音频缓存

NSLock *synlock ;///同步控制
//Byte pcmDataBuffer[EVERY_READ_LENGTH];//pcm的读文件数据区
FILE *file;//pcm源文件

BOOL isSetBuffSize = YES;
static void AudioPlayerAQInputCallback(void *input, AudioQueueRef outQ, AudioQueueBufferRef outQB);
void readPCMAndPlay(AudioQueueRef outQ,AudioQueueBufferRef outQB);


//播放
void configPlayer(NSString *urlString);



int __RTSPClientCallBack( int _chid, int *_chPtr, int _frameType, char *_pBuf, RTSP_FRAME_INFO *_frameInfo)
{
    
    //    NSLog(@"*************************************************************");
    
    if ( NULL != _frameInfo && NULL != _pBuf)
    {
        if (_frameType == EASY_SDK_AUDIO_FRAME_FLAG)
        {
            /*
             要处理的音频
             */
            
            
            
            struct Node *pNode=malloc(sizeof(struct Node));
            pNode->len=_frameInfo->length;
            pNode->data= malloc(pNode->len);
            memcpy(pNode->data, _pBuf, pNode->len);
 
            AddToTail(&MyList, pNode);
            if(sIfHaveDone==0&&MyList.NodeLen>QUEUE_BUFFER_SIZE)//接收的音频数据已经足够多了，现在可以进行播放了
            {
                sIfHaveDone=1;
                for(int i=0;i<QUEUE_BUFFER_SIZE;i++)//需要
                {
                    readPCMAndPlay(audioQueue, outQB[i]);
                }
                
            }
            //            NSLog(@"Add[A] [%d]\tsize:%d\ttm:%u.%u\n", _frameInfo->type, _frameInfo->length, _frameInfo->timestamp_sec, _frameInfo->timestamp_usec);
//            NSLog(@"----------- [%d]-----------------size:%d------------------%u-----------------%u\n", _frameInfo->type, _frameInfo->length, _frameInfo->timestamp_sec, _frameInfo->timestamp_usec);
        }
        
        else if (_frameType == EASY_SDK_VIDEO_FRAME_FLAG)
        {
            
            if (_frameInfo->codec == EASY_SDK_VIDEO_CODEC_H264)
            {
                CVPixelBufferRef pixelBuffer = NULL;
                
                if (_frameInfo->type == EASY_SDK_VIDEO_FRAME_I)
                {
                    
                    
                    VideoPacket *vp;
                    _spsSize = _frameInfo->reserved1;
                    _ppsSize =_frameInfo->reserved2 - _frameInfo->reserved1;
                    if(_spsSize==0||_ppsSize==0)//异常
                    {
                        
                        NSMutableArray *as = [NSMutableArray array];//01位置
                        NSMutableArray *as2= [NSMutableArray array];//00 00 00 01还是00 00 01，查找00 00 01并判断前3个字符是否是00
                        for(int i=2;i<_frameInfo->length;i++)
                        {
                            if(*(_pBuf+i)==1&&*(_pBuf+i-1)==0&&*(_pBuf+i-2)==0)
                                //if(memcmp(_pBuf+i- 2, KStartCode, 3) == 0)//找到起始码00 00 01
                            {
                                if(i>=3&&*(_pBuf+i-3)==0)// 00 00 00 01
                                {
                                    [as addObject:[NSNumber numberWithInt:i]];
                                    [as2 addObject:[NSNumber numberWithInt:0]];
                                }
                                else
                                {
                                    [as addObject:[NSNumber numberWithInt:i]];
                                    [as2 addObject:[NSNumber numberWithInt:1]];
                                }
                            }
                            //NSLog(@"++++++%d-------%@",as.count,as);
                        }
                        
                        if (as.count < 3) {
                            return 0;
                        }
                        _spsSize= [as[1] integerValue]-3+[as2[1] integerValue];
                        _ppsSize=[as[2] integerValue]-3+[as2[2] integerValue]-_spsSize;
                        
                        _sps = malloc(_spsSize);
                        memcpy(_sps, _pBuf+4-[as2[0] integerValue], _spsSize-4+[as2[0] integerValue]);
                        _pps = malloc(_ppsSize);
                        memcpy(_pps, _pBuf+_spsSize+4-[as2[1] integerValue], _ppsSize-4+[as2[1] integerValue]);
                        
                        
                        
                        NSInteger count=as.count-1;//获取最后一个01即I帧的起始码
                        // NSInteger NaluSize=_frameInfo->length-([as[count] integerValue]-3+[as2[count] integerValue]);
                        NSInteger NaluSize=_frameInfo->length-([as[count] integerValue]-3);
                        vp= [[VideoPacket alloc]initWithSize:NaluSize];//分配空间
                        //memcpy(vp.buffer, _pBuf + [as[count] integerValue]-3+[as2[count] integerValue], NaluSize);
                        memcpy(vp.buffer, _pBuf + [as[count] integerValue]-3, NaluSize);
                        
                        //uint32_t nalSize = (uint32_t)(vp.size - 4+[as2[count] integerValue]);
                        uint32_t nalSize = (uint32_t)(vp.size - 4);
                        uint8_t *pNalSize = (uint8_t*)(&nalSize);
                        /*
                         for(int i=0;i<4-[as2[count] integerValue];i++)
                         {
                         vp.buffer[i] = *(pNalSize + 3-i);
                         }*/
                        vp.buffer[0] = *(pNalSize + 3);
                        vp.buffer[1] = *(pNalSize + 2);
                        vp.buffer[2] = *(pNalSize + 1);
                        vp.buffer[3] = *(pNalSize);
                        
                        
                        
                        
                    }
                    else//正确
                    {
                        
                        NSInteger NaluSize=_frameInfo->length-_spsSize-_ppsSize;
                        vp= [[VideoPacket alloc]initWithSize:NaluSize];//分配空间
                        memcpy(vp.buffer, _pBuf + _spsSize + _ppsSize, NaluSize);
                        
                        uint32_t nalSize = (uint32_t)(vp.size - 4);
                        uint8_t *pNalSize = (uint8_t*)(&nalSize);
                        vp.buffer[0] = *(pNalSize + 3);
                        vp.buffer[1] = *(pNalSize + 2);
                        vp.buffer[2] = *(pNalSize + 1);
                        vp.buffer[3] = *(pNalSize);
                        
                        
                        
                        _sps = malloc(_spsSize);
                        memcpy(_sps, _pBuf+4, _spsSize-4);
                        _pps = malloc(_ppsSize);
                        memcpy(_pps, _pBuf+_spsSize+4, _ppsSize-4);

                    }
                    
                    
                    
                    if(initH264Decoder()) {
                        pixelBuffer =decode(vp);
                        //                        NSLog(@"************************************");
                    }
                    
                    
                }
                else if (_frameInfo->type == EASY_SDK_VIDEO_FRAME_P)
                {
                    NSInteger NaluSize=_frameInfo->length;
                    VideoPacket *vp = [[VideoPacket alloc] initWithSize:NaluSize];//分配空间
                    memcpy(vp.buffer, _pBuf, NaluSize);
                    
                    uint32_t nalSize = (uint32_t)(vp.size - 4);
                    uint8_t *pNalSize = (uint8_t*)(&nalSize);
                    vp.buffer[0] = *(pNalSize + 3);
                    vp.buffer[1] = *(pNalSize + 2);
                    vp.buffer[2] = *(pNalSize + 1);
                    vp.buffer[3] = *(pNalSize);
                    
                    //vp.buffer=(uint8_t*)_pBuf ;
                    //vp.size=_frameInfo->length;
                    
                    pixelBuffer =decode(vp);
                    
                }
                if(pixelBuffer) {
                    dispatch_sync(dispatch_get_main_queue(), ^{
                        _glLayer.pixelBuffer = pixelBuffer;
                    });
                    
                    CVPixelBufferRelease(pixelBuffer);
                    if(!_pps)
                    {
                        free(_pps);
                        _pps=NULL;
                    }
                    if(!_sps)
                    {
                        free(_sps);
                        _sps=NULL;
                    }
                }
                
            }
            
//            NSLog(@"Add[V] [%d]\tsize:%d\ttm:%u.%u\n", _frameInfo->type, _frameInfo->length, _frameInfo->timestamp_sec, _frameInfo->timestamp_usec);
        }
        
        else if (_frameType == EASY_SDK_RTP_FRAME_FLAG)
        {
//            NSLog(@"RTP Packet[Track%d] %d [%d]: %02X %02X %02X %02X %02X\n", _frameInfo->channels, _chid, _frameInfo->length, (unsigned char)_pBuf[0], (unsigned char)_pBuf[1], (unsigned char)_pBuf[2], (unsigned char)_pBuf[3], (unsigned char)_pBuf[4]);
        }
        else if (_frameType == EASY_SDK_SDP_FRAME_FLAG)
        {
//            NSLog(@"SDP: %s\n", _pBuf);
            FILE *f = fopen("sdp.txt", "wb");
            if (NULL != f)
            {
                fwrite(_pBuf, 1, _frameInfo->length, f);
                fclose(f);
            }
        }
        
        if (_frameType == 0x01 && _frameInfo->type == 0x01)
        {
            
//            NSLog(@"Channel[%d] Codec[%d] %s/%d FPS[%d]BPS[%.2f] frametype[%d][%dx%d] framesize[%d] %02X %02X %02X %02X %02X\n", _chid, _frameInfo->codec, _frameType==EASY_SDK_VIDEO_FRAME_FLAG?"VIDEO":"AUDIO", _frameInfo->codec,
//                  _frameInfo->fps, _frameInfo->bitrate,	 _frameInfo->type, _frameInfo->width, _frameInfo->height,	_frameInfo->length,
//                  (unsigned char)_pBuf[0], (unsigned char)_pBuf[1], (unsigned char)_pBuf[2], (unsigned char)_pBuf[3], (unsigned char)_pBuf[4]);
            
        }
    }else{
//        NSLog(@"*************************************************************");
        
    }
    
    
    
    return 0;
}



BOOL initH264Decoder()
{
    if(_deocderSession) {
        return YES;
    }
    
    const uint8_t* const parameterSetPointers[2] = { _sps, _pps };
    const size_t parameterSetSizes[2] = { _spsSize, _ppsSize };
    OSStatus status = CMVideoFormatDescriptionCreateFromH264ParameterSets(kCFAllocatorDefault,
                                                                          2, //param count
                                                                          parameterSetPointers,
                                                                          parameterSetSizes,
                                                                          4, //nal start code size
                                                                          &_decoderFormatDescription);
    
    if(status == noErr) {
        CFDictionaryRef attrs = NULL;
        
        //kCVPixelBufferPixelFormatTypeKey，指定解码后的图像格式，必须指定成NV12，苹果的硬解码器只支持NV12。
        const void *keys[] = { kCVPixelBufferPixelFormatTypeKey };
        
        //      kCVPixelFormatType_420YpCbCr8Planar is YUV420
        //      kCVPixelFormatType_420YpCbCr8BiPlanarFullRange is NV12
        uint32_t v = kCVPixelFormatType_420YpCbCr8BiPlanarFullRange;
        const void *values[] = { CFNumberCreate(NULL, kCFNumberSInt32Type, &v) };
        attrs = CFDictionaryCreate(NULL, keys, values, 1, NULL, NULL);
        
        // callBackRecord 是用来指定回调函数的，解码器支持异步模式，解码后会调用这里的回调函数。
        VTDecompressionOutputCallbackRecord callBackRecord;
        callBackRecord.decompressionOutputCallback = didDecompress;
        callBackRecord.decompressionOutputRefCon = NULL;
        
        //__________________________创建解码 session
        // decoderFormatDescription 是 CMVideoFormatDescriptionRef 类型的视频格式描述，这个需要用H.264的 sps 和 pps数据来创建，调用以下函数创建 decoderFormatDescription
        //  CMVideoFormatDescriptionCreateFromH264ParameterSets
        status = VTDecompressionSessionCreate(kCFAllocatorDefault,
                                              _decoderFormatDescription,
                                              NULL, attrs,
                                              &callBackRecord,
                                              &_deocderSession);
        CFRelease(attrs);
    } else {
        //        NSLog(@"IOS8VT: reset decoder session failed status=%d", status);
    }
    
    return YES;
}


//    如果 decoderSession创建成功就可以开始解码了。
CVPixelBufferRef decode(VideoPacket* vp) {
    CVPixelBufferRef outputPixelBuffer = NULL;
    
    
    CMBlockBufferRef blockBuffer = NULL;
    // 先用CMBlockBufferCreateWithMemoryBlock 从H.264数据创建一个CMBlockBufferRef实例。
    OSStatus status  = CMBlockBufferCreateWithMemoryBlock(kCFAllocatorDefault,
                                                          (void*)vp.buffer, vp.size,
                                                          kCFAllocatorNull,
                                                          NULL, 0, vp.size,
                                                          0, &blockBuffer);
    if(status == kCMBlockBufferNoErr) {
        CMSampleBufferRef sampleBuffer = NULL; // 其中 sampleBuffer是输入的H.264视频数据，每次输入一个frame。
        const size_t sampleSizeArray[] = {vp.size};
        // 然后用 CMSampleBufferCreateReady创建CMSampleBufferRef实例。
        status = CMSampleBufferCreateReady(kCFAllocatorDefault,
                                           blockBuffer,
                                           _decoderFormatDescription ,
                                           1, 0, NULL, 1, sampleSizeArray,
                                           &sampleBuffer);
        if (status == kCMBlockBufferNoErr && sampleBuffer) {
            VTDecodeFrameFlags flags = 0;//其中 flags 用0 表示使用同步解码，这样比较简单。
            VTDecodeInfoFlags flagOut = 0;
            //_____________________________解码一个frame
            // 解码成功之后，outputPixelBuffer里就是一帧 NV12格式的YUV图像了。
            OSStatus decodeStatus = VTDecompressionSessionDecodeFrame(_deocderSession,
                                                                      sampleBuffer,
                                                                      flags,
                                                                      &outputPixelBuffer,
                                                                      &flagOut);
            
            if(decodeStatus == kVTInvalidSessionErr) {
                //                NSLog(@"IOS8VT: Invalid session, reset decoder session");
            } else if(decodeStatus == kVTVideoDecoderBadDataErr) {
                //                NSLog(@"IOS8VT: decode failed status=%d(Bad data)", decodeStatus);
            } else if(decodeStatus != noErr) {
                //                NSLog(@"IOS8VT: decode failed status=%d", decodeStatus);
            }
            
            CFRelease(sampleBuffer);
        }
        CFRelease(blockBuffer);
    }
    
    return outputPixelBuffer;
}



// 音频

void readPCMAndPlay(AudioQueueRef outQ,AudioQueueBufferRef outQB)
{
    [synlock lock];
    struct Node * pNode=GetFromHead(&MyList);
    if(pNode==NULL)
    {
        [synlock unlock];
        return;
    }
//     NSLog(@"读取的数据大小为 = %d",pNode->len);
    
    //outQB->mAudioDataByteSize = (int)readLength;
    outQB->mAudioDataByteSize = (int)pNode->len;
    Byte *audiodata = (Byte *)outQB->mAudioData;
    for(int i=0;i<pNode->len;i++)
    {
        audiodata[i] = pNode->data[i];
    }
    free(pNode->data);
    free(pNode);
    /*
     将创建的buffer区添加到audioqueue里播放
     AudioQueueBufferRef用来缓存待播放的数据区，AudioQueueBufferRef有两个比较重要的参数，AudioQueueBufferRef->mAudioDataByteSize用来指示数据区大小，AudioQueueBufferRef->mAudioData用来保存数据区
     */
    
    //* 向缓冲区中添加新的数据（数据一般从file或网络中获取）
//    NSLog(@"给buffer填充数据");
    AudioQueueEnqueueBuffer(audioQueue, outQB, 0, NULL);
    [synlock unlock];
}



void configPlayer(NSString *urlString) {
    const  char *url =  [urlString UTF8String];
    const  char *username = "admin";
    const   char *password = "admin";
    // ------------------
    
    EasyRTSP_Init(&rtspHandle);
    EasyRTSP_SetCallback(rtspHandle, __RTSPClientCallBack);
    EasyRTSP_OpenStream(rtspHandle, 1, (char *)url, RTP_OVER_TCP, EASY_SDK_VIDEO_FRAME_FLAG|EASY_SDK_AUDIO_FRAME_FLAG, (char *)username,(char *)password, NULL, 1000, 0,1);
    
    
    
}



@interface PlayViewController ()
{
    
    BOOL                _fullscreen;
    BOOL                _isShow;
    
    int secondsCountDown;//倒计时消失
    NSTimer *countDownTimer;
    
    NSTimer *_isShowTimer;
    NSTimer *_pushStream;
    NSTimer *_isVideotape;
    
    NSString *_palyURL;//播放地址
    MBProgressHUD * _hud;
    //    MBProgressHUD * _HUD;
    //  抓拍
    NSString *photoImagePath;
    UIImage *photoImage;
    
    // 录像显示计时
    UILabel *_showTimeLab;
    UIView *_showRedView;
    

    NSString *cameraSerial;
    NSString *deviceSerial;
    BOOL isOpenCMS;
    int isNumber; //拉流次数
    
//    G711ToAAC *_g711Toaac;
//    G711addH264 *_g711Addh264;
    NetRequestTool *_requestTool;
    
    int seconds;
    int  percens;
    int  minus;
}
@property(assign)BOOL isVH; //横竖屏
@property (nonatomic,strong)ToolBar   *topBar,*botBar;
@property (nonatomic,strong)UIBarButtonItem   *midBtnItem,*upBtnItem,*downBtnItem,*leftBtnItem,*rightBtnItem,*spaceItem,*jirtuItem,*picItem,*videotapeItem;
@property (nonatomic,strong)UIButton *upBtn,*downBtn,*leftBtn,*rightBtn,*jirtuBtn,*videotapeBtn;


@property (nonatomic,strong)NSString *sessionStr;
@end




@implementation PlayViewController

- (void)viewWillAppear:(BOOL)animated
{
    [super viewWillAppear:animated];
}
- (void)dealloc
{
    [self clearH264Deocder];
    ClearAllData(&MyList);

  

}
- (void)viewWillDisappear:(BOOL)animated
{
    [super viewWillDisappear:animated];
    dispatch_queue_t globalQueue=dispatch_get_global_queue(DISPATCH_QUEUE_PRIORITY_DEFAULT, 0);
    
    //异步执行队列任务
    dispatch_async(globalQueue, ^{
        EasyRTSP_CloseStream(rtspHandle);/////////////////
        EasyRTSP_Deinit(&rtspHandle);
    });
  
}

- (void)viewDidLoad {
    [super viewDidLoad];
    self.view.backgroundColor = [UIColor blackColor];
    self.title = @"视频播放";
    _requestTool = [[NetRequestTool alloc]init];
    self.callSocket = [[CallBySocket alloc]init];
    _glLayer = [[AAPLEAGLLayer alloc] initWithFrame:CGRectMake(0, 0, ScreenWidth, ScreenHeight)];
    _glLayer.backgroundColor = [UIColor blackColor].CGColor;
    [self.view.layer addSublayer:_glLayer];
    if (audioQueue==nil) {
        [self initAudio];
        NSLog(@"开始播放");
        AudioQueueStart(audioQueue, NULL);
        //开始播放音频,(如果没有准备好)准备音频硬件,音频队列开始
    }
      [self initSomeView];
       configPlayer(self.urlModel.url);
}

- (void)initSomeView
{
    
    
    self.isVH = YES;
    _isShow = YES; //默认是显示的
    CGFloat botH = 50;
    
    
    //录像显示时间
    
    
    _showTimeLab = [[UILabel alloc] initWithFrame:CGRectMake(0,70, ScreenWidth, 40)];
    _showTimeLab.font = [UIFont systemFontOfSize:15];
    _showTimeLab.textAlignment = NSTextAlignmentCenter;
    [_showTimeLab setTextColor:[UIColor whiteColor]];
    //    _showTimeLab.backgroundColor = [UIColor orangeColor];
    [self.view addSubview:_showTimeLab];
    [self.view bringSubviewToFront:_showTimeLab];
    
    
    _showRedView = [[UIView alloc] initWithFrame:CGRectMake(0,70, 10, 10)];
    _showRedView.center = CGPointMake(_showTimeLab.center.x-40, _showTimeLab.center.y);
    _showRedView.backgroundColor = [UIColor redColor];
    _showRedView.layer.cornerRadius = 5;
    [self.view addSubview:_showRedView];
    [self.view bringSubviewToFront:_showRedView];
    _showTimeLab.hidden = YES;
    _showRedView.hidden = YES;
    
    // 弹簧
    self.spaceItem = [[UIBarButtonItem alloc] initWithBarButtonSystemItem:UIBarButtonSystemItemFlexibleSpace
                                                                   target:nil
                                                                   action:nil];
    UIBarButtonItem *spaceItem1 = [[UIBarButtonItem alloc] initWithBarButtonSystemItem:UIBarButtonSystemItemFlexibleSpace
                                                                                target:nil
                                                                                action:nil];
    UIBarButtonItem *spaceItem2 = [[UIBarButtonItem alloc] initWithBarButtonSystemItem:UIBarButtonSystemItemFlexibleSpace
                                                                                target:nil
                                                                                action:nil];
    UIBarButtonItem *spaceItem3 = [[UIBarButtonItem alloc] initWithBarButtonSystemItem:UIBarButtonSystemItemFlexibleSpace
                                                                                target:nil
                                                                                action:nil];
    UIBarButtonItem *spaceItem4 = [[UIBarButtonItem alloc] initWithBarButtonSystemItem:UIBarButtonSystemItemFlexibleSpace
                                                                                target:nil
                                                                                action:nil];
    
    
    // 上下遮屏
    self.topBar  = [[ToolBar alloc] initWithFrame:CGRectMake(0, 0, ScreenWidth, 70)];
    self.botBar = [[ToolBar alloc] initWithFrame:CGRectMake(0, ScreenHeight-botH, ScreenWidth, botH)];
    self.botBar.tintColor = [UIColor blackColor];
    self.topBar.tintColor = [UIColor blackColor];
    self.topBar.autoresizingMask = UIViewAutoresizingFlexibleWidth;
    self.botBar.autoresizingMask = UIViewAutoresizingFlexibleTopMargin | UIViewAutoresizingFlexibleWidth;
    [self.view addSubview:self.topBar];
    
    [self.view addSubview:self.botBar];
    
    
    // 喊话功能按钮
    self.midBtn = [UIButton buttonWithType:UIButtonTypeCustom];
    self.midBtn.frame = CGRectMake(0, 1,50,50);
    self.midBtn.backgroundColor = [UIColor clearColor];
    self.midBtn.showsTouchWhenHighlighted = YES;
    
    [self.midBtn setImage:[UIImage imageNamed:@"video_talk"] forState:UIControlStateNormal];
    [self.midBtn setImage:[UIImage imageNamed:@"video_notalk"] forState:UIControlStateSelected];
    
    self.midBtnItem = [[UIBarButtonItem alloc] init ];
    self.midBtnItem.customView = self.midBtn;
    
    
    // 录像功能按钮
    self.videotapeBtn = [UIButton buttonWithType:UIButtonTypeCustom];
    self.videotapeBtn.frame = CGRectMake(0, 1,50,50);
    self.videotapeBtn.backgroundColor = [UIColor clearColor];
    self.videotapeBtn.showsTouchWhenHighlighted = YES;
    
    [self.videotapeBtn setImage:[UIImage imageNamed:@"videotapeBtn"] forState:UIControlStateNormal];
    [self.videotapeBtn setImage:[UIImage imageNamed:@"videotapeBtn"] forState:UIControlStateSelected];
    
    self.videotapeItem = [[UIBarButtonItem alloc] init ];
    self.videotapeItem.customView = self.videotapeBtn;
    
    
    
    // top hud
    self.doneBtn = [UIButton buttonWithType:UIButtonTypeCustom];
    self.doneBtn.frame = CGRectMake(0, 1, 50, botH);
    self.doneBtn.backgroundColor = [UIColor clearColor];
    self.doneBtn.showsTouchWhenHighlighted = YES;
    [self.doneBtn setBackgroundImage:[UIImage imageNamed:@"backvidoBtn"] forState:UIControlStateNormal];
    
    UIBarButtonItem *doneItem = [[UIBarButtonItem alloc] init];
    doneItem.customView = self.doneBtn;
    
    
    // pic
    self.picBtn = [UIButton buttonWithType:UIButtonTypeCustom];
    self.picBtn.frame = CGRectMake(0, 0,50, 50);
    self.picBtn.backgroundColor = [UIColor clearColor];
    self.picBtn.showsTouchWhenHighlighted = YES;
    
    [self.picBtn setBackgroundImage:[UIImage imageNamed:@"transS"] forState:UIControlStateNormal];
    [self.picBtn setBackgroundImage:[UIImage imageNamed:@"transL"] forState:UIControlStateSelected];
    
    self.picItem = [[UIBarButtonItem alloc] init];
    self.picItem.customView = self.picBtn;
    self.picBtn.selected = YES;
    [self.topBar setItems:@[doneItem,self.spaceItem,self.midBtnItem,self.spaceItem,self.videotapeItem,self.spaceItem,self.picItem] animated:NO];
    
    
    //    上
    self.upBtn = [UIButton buttonWithType:UIButtonTypeCustom];
    [self.upBtn setFrame:CGRectMake(0, 0, 76, 38)];
    self.upBtn.showsTouchWhenHighlighted = YES;
    [self.upBtn setBackgroundImage:[UIImage imageNamed:@"shangvidoBtn"] forState:UIControlStateNormal];
    self.upBtn.tag = 1000;
    
    self.upBtnItem = [[UIBarButtonItem alloc]init];
    self.upBtnItem.width = 50;
    self.upBtnItem.customView = self.upBtn;
    
    //    下
    self.downBtn = [UIButton buttonWithType:UIButtonTypeCustom];
    [self.downBtn setFrame:CGRectMake(0, 0, 76, 38)];
    self.downBtn.showsTouchWhenHighlighted = YES;
    [self.downBtn setBackgroundImage:[UIImage imageNamed:@"xiavidoBtn"] forState:UIControlStateNormal];
    self.downBtn.tag = 1001;
    
    self.downBtnItem = [[UIBarButtonItem alloc] init];
    self.downBtnItem.customView = self.downBtn;
    self.downBtnItem.width = 50;
    
    //    左
    self.leftBtn = [UIButton buttonWithType:UIButtonTypeCustom];
    [self.leftBtn setFrame:CGRectMake(0, 0, 76, 38)];
    self.leftBtn.showsTouchWhenHighlighted = YES;
    [self.leftBtn setBackgroundImage:[UIImage imageNamed:@"zuovidoBtn"] forState:UIControlStateNormal];
    self.leftBtn.tag = 1002;
    
    self.leftBtnItem = [[UIBarButtonItem alloc] init];
    self.leftBtnItem.customView = self.leftBtn;
    self.leftBtnItem.width = 50;
    
    //    右
    self.rightBtn = [UIButton buttonWithType:UIButtonTypeCustom];
    [self.rightBtn setFrame:CGRectMake(0, 0, 76, 38)];
    self.rightBtn.showsTouchWhenHighlighted = YES;
    [self.rightBtn setBackgroundImage:[UIImage imageNamed:@"youvidoBtn"] forState:UIControlStateNormal];
    self.rightBtn.tag = 1003;
    
    self.rightBtnItem = [[UIBarButtonItem alloc] init];
    self.rightBtnItem.width = 50;
    self.rightBtnItem.customView = self.rightBtn;
    
    //    抓拍
    self.jirtuBtn = [UIButton buttonWithType:UIButtonTypeCustom];
    [self.jirtuBtn setFrame:CGRectMake(0, 0, 76, 38)];
    self.jirtuBtn.showsTouchWhenHighlighted = YES;
    [self.jirtuBtn setBackgroundImage:[UIImage imageNamed:@"jietuvidoBtn"] forState:UIControlStateNormal];
    self.jirtuBtn.tag = 1004;
    
    self.jirtuItem = [[UIBarButtonItem alloc] init];
    self.jirtuItem.width = 50;
    self.jirtuItem.customView = self.jirtuBtn;
    
    [self.botBar setItems:@[self.upBtnItem, spaceItem1, self.downBtnItem, spaceItem2,
                            self.leftBtnItem, spaceItem3,self.rightBtnItem, spaceItem4,self.jirtuItem] animated:NO];
    
    UITapGestureRecognizer *tapGesture = [[UITapGestureRecognizer alloc] initWithTarget:self action:@selector(handleTap)];
    tapGesture.numberOfTapsRequired = 1;
    
    
    [self.view addGestureRecognizer:tapGesture];
    
    [self.midBtn addTarget:self action:@selector(callDidTouch:) forControlEvents:UIControlEventTouchUpInside];
    [self.doneBtn addTarget:self action:@selector(doneDidTouch:)
           forControlEvents:UIControlEventTouchUpInside];
    [self.videotapeBtn addTarget:self action:@selector(videotapeBtnClick:)
                forControlEvents:UIControlEventTouchUpInside];
    
    [self.upBtn addTarget:self action:@selector(clickTounchDownButton:) forControlEvents:UIControlEventTouchDown];
    [self.upBtn addTarget:self action:@selector(clickTounchUpInside:) forControlEvents:UIControlEventTouchUpInside|UIControlEventTouchUpOutside];
    [self.downBtn addTarget:self action:@selector(clickTounchDownButton:) forControlEvents:UIControlEventTouchDown];
    [self.downBtn addTarget:self action:@selector(clickTounchUpInside:) forControlEvents:UIControlEventTouchUpInside|UIControlEventTouchUpOutside];
    
//    [self.jirtuBtn addTarget:self action:@selector(takePhoto:) forControlEvents:UIControlEventTouchUpInside];
    [self.rightBtn addTarget:self action:@selector(clickTounchDownButton:) forControlEvents:UIControlEventTouchDown];
    [self.rightBtn addTarget:self action:@selector(clickTounchUpInside:) forControlEvents:UIControlEventTouchUpInside|UIControlEventTouchUpOutside];
    [self.leftBtn addTarget:self action:@selector(clickTounchDownButton:) forControlEvents:UIControlEventTouchDown];
    [self.leftBtn addTarget:self action:@selector(clickTounchUpInside:) forControlEvents:UIControlEventTouchUpInside|UIControlEventTouchUpOutside];
    [self.picBtn addTarget:self action:@selector(tapscreen)
          forControlEvents:UIControlEventTouchUpInside];
    
    

    
    //查看图片的
    
    _imgView = [[UIImageView alloc]initWithFrame:CGRectMake(ScreenWidth-60, ScreenHeight-49-80, 50, 50)];
    _imgView.backgroundColor = [UIColor blackColor];
    _imgView.userInteractionEnabled = YES;
    _imgView.hidden = YES;
    _imgView.autoresizingMask = UIViewAutoresizingFlexibleTopMargin ;
    
    
//    UITapGestureRecognizer  *singleTapGestureRecognizer = [[UITapGestureRecognizer alloc]initWithTarget:self action:@selector(tapTakePhotoImageView:)];
//    singleTapGestureRecognizer.numberOfTapsRequired = 1;
//    [_imgView addGestureRecognizer:singleTapGestureRecognizer];
    
    [self.view addSubview:_imgView];
    [self.view bringSubviewToFront:_imgView];
    
    _activityIndicatorView = [[UIActivityIndicatorView alloc] initWithActivityIndicatorStyle: UIActivityIndicatorViewStyleWhiteLarge];
    _activityIndicatorView.center = self.view.center;
    _activityIndicatorView.autoresizingMask = UIViewAutoresizingFlexibleTopMargin | UIViewAutoresizingFlexibleBottomMargin | UIViewAutoresizingFlexibleLeftMargin | UIViewAutoresizingFlexibleRightMargin;
    
    [self.view addSubview:_activityIndicatorView];
//    [_activityIndicatorView startAnimating];
    
    [self.botBar setItems:@[self.upBtnItem, spaceItem1, self.downBtnItem, spaceItem2,
                            self.leftBtnItem, spaceItem3,self.rightBtnItem, spaceItem4,self.jirtuItem] animated:NO];
    _hud = [[MBProgressHUD alloc] initWithView:self.view];
    _hud.mode = MBProgressHUDModeIndeterminate;
    [_hud.button addTarget:self action:@selector(hidenMBView) forControlEvents:UIControlEventTouchUpInside];
    [_hud.button setTitle:NSLocalizedString(@"确定",@"") forState:UIControlStateNormal];
    [self.view addSubview:_hud];
    
    //    _HUD = [[MBProgressHUD alloc] initWithView:self.view];
    //    _HUD.mode = MBProgressHUDModeIndeterminate;
    //    [self.view addSubview:_HUD];
}

-(void)clearH264Deocder {
    if(_deocderSession) {
        // ____________________销毁解码 session
        VTDecompressionSessionInvalidate(_deocderSession);
        CFRelease(_deocderSession);
        _deocderSession = NULL;
    }
    
    if(_decoderFormatDescription) {
        CFRelease(_decoderFormatDescription);
        _decoderFormatDescription = NULL;
    }
    AudioQueueStop(audioQueue, YES);
    
    for (NSInteger m = 0; m < QUEUE_BUFFER_SIZE;++m) {
        AudioQueueFreeBuffer(audioQueue, outQB[m]);
    }
//    free(_sps);
//    free(_pps);
    _spsSize = _ppsSize = 0;
}

//**************** 音频  *******************

-(void)initAudio
{
    NSLog(@"开始设置音频参数");
    ///设置音频参数 AudioStreamBasicDescription audioDescription;音频参数
    audioDescription.mSampleRate = 8000;//采样率
    audioDescription.mFormatID = kAudioFormatALaw;
    audioDescription.mFormatFlags = kLinearPCMFormatFlagIsSignedInteger | kAudioFormatFlagIsPacked;
    audioDescription.mChannelsPerFrame = 1;///单声道
    audioDescription.mFramesPerPacket = 1;//每一个packet一侦数据
    audioDescription.mBitsPerChannel = 16;//每个采样点16bit量化
    audioDescription.mBytesPerFrame = (audioDescription.mBitsPerChannel/8) * audioDescription.mChannelsPerFrame;
    audioDescription.mBytesPerPacket = audioDescription.mBytesPerFrame ;
    ///创建一个新的从audioqueue到硬件层的通道
//   	AudioQueueNewOutput(&audioDescription, AudioPlayerAQInputCallback, (__bridge void * _Nullable)(self), CFRunLoopGetCurrent(), kCFRunLoopCommonModes, 0, &audioQueue);///使用当前线程播
    //callback一个回调函数的指针，当音频队列已完成播放时调用
    NSLog(@"创建音频队列");
    AudioQueueNewOutput(&audioDescription, AudioPlayerAQInputCallback, (__bridge void * _Nullable)(self), nil, nil, 0, &audioQueue);//使用player的内部线程播
    ////添加buffer区
    for(int i=0;i<QUEUE_BUFFER_SIZE;i++)
    {
        NSLog(@"音频队列分配缓冲");
        //音频队列分配buffer。
        AudioQueueAllocateBuffer(audioQueue, MIN_SIZE_PER_FRAME, &outQB[i]);///创建buffer区，MIN_SIZE_PER_FRAME为每一侦所需要的最小的大小，该大小应该比每次往buffer里写的最大的一次还大
    }
    //add.初始化list
    Init(&MyList);
}


-(void)checkUsedQueueBuffer:(AudioQueueBufferRef) qbuf
{
//    NSLog(@"dddddddddddddddddddddddddddddd");
}
#pragma mark player call back
/*
 试了下其实可以不用静态函数，但是c写法的函数内是无法调用[self ***]这种格式的写法，所以还是用静态函数通过void *input来获取原类指针
 这个回调存在的意义是为了重用缓冲buffer区，当通过AudioQueueEnqueueBuffer(outQ, outQB, 0, NULL);函数放入queue里面的音频文件播放完以后，通过这个函数通知
 调用者，这样可以重新再使用回调传回的AudioQueueBufferRef
 */
static void AudioPlayerAQInputCallback(void *input, AudioQueueRef outQ, AudioQueueBufferRef outQB)
{
//    NSLog(@"进入回调函数 AudioPlayerAQInputCallback");
    PlayViewController *mainviewcontroller = (__bridge  PlayViewController *)input;
    readPCMAndPlay(outQ, outQB);
}

#pragma mark - Amber
/**
 *  判定是否隐藏按钮调
 * @param  hidden 判定是否是要自动隐藏
 */

- (void) handleTap
{
    
    [self setControlsHidden:!_isShow animated:YES];
    [self resetIdleTimer];
}
- (void)resetIdleTimer
{
    if (!_isShowTimer)
        _isShowTimer = [NSTimer scheduledTimerWithTimeInterval:4.
                                                        target:self
                                                      selector:@selector(idleTimerExceeded)
                                                      userInfo:nil
                                                       repeats:NO];
    else {
        if (fabs([_isShowTimer.fireDate timeIntervalSinceNow]) < 4.)
            [_isShowTimer setFireDate:[NSDate dateWithTimeIntervalSinceNow:4.]];
    }
}
- (void)idleTimerExceeded
{
    _isShowTimer = nil;
    if (!_isShow)
        [self handleTap];
}
- (void)setControlsHidden:(BOOL)hidden animated:(BOOL)animated
{
    _isShow = hidden;
    [[UIApplication sharedApplication] setIdleTimerDisabled:_isShow];
    
    [UIView animateWithDuration:0.1
                          delay:0.0
                        options:UIViewAnimationOptionCurveEaseInOut | UIViewAnimationOptionTransitionNone
                     animations:^{
                         CGFloat alpha = _isShow ? 0 : 1;
                         self.topBar.alpha = alpha;
                         //                         self.topHUD.alpha = alpha;
                         self.botBar.alpha = alpha;
                     }
                     completion:nil];
    
}

/**
 *  返回
 *
 *  @param sender Btn
 */
#pragma mark  - ClickBtn BackRootVC
- (void) doneDidTouch: (id) sender
{
    [self dismissViewControllerAnimated:YES completion:nil];
}

// 录像功能
- (void)videotapeBtnClick:(id)sender
{
    //    if (isSaveAudioAddVideo) {
    isSaveAudioAddVideo = !isSaveAudioAddVideo;
    

    
}

- (void)showTimeView
{
    percens++;
    //没过１００毫秒，就让秒＋１，然后让毫秒在归零
    if(percens==60){
        seconds++;
        percens = 0;
    }
    if (seconds == 60) {
        minus++;
        seconds = 0;
    }
    // 让不断变量的时间数据进行显示到label上面。
    _showTimeLab.text = [NSString stringWithFormat:@"%02d:%02d:%02d",minus,seconds,percens];
}
#pragma mark  - full Screen
// 横屏 竖屏
- (void)tapscreen
{
    
    if (self.isVH) {
        
        [UIView animateWithDuration:0.5 animations:^{
            self.view.transform =CGAffineTransformMakeRotation(M_PI_2);
            self.view.bounds = CGRectMake(0, 0,ScreenHeight,ScreenWidth);
            _glLayer.frame = CGRectMake(0, 0,ScreenHeight,ScreenWidth);
            _imgView.frame = CGRectMake(ScreenHeight-80,ScreenWidth-49-80,70,70);
            self.topBar.frame = CGRectMake(self.topBar.frame.origin.x,self.topBar.frame.origin.y,ScreenHeight,50);
            self.botBar.frame = CGRectMake(0,ScreenWidth-50,ScreenHeight,50);
            _showTimeLab.frame =CGRectMake(0, 70, ScreenHeight, 40);
            _showRedView.center = CGPointMake(_showTimeLab.center.x-40, _showTimeLab.center.y);
            _showRedView.layer.cornerRadius = 5;
            
            self.isVH = NO;
            self.picBtn.selected = NO;
        }];
        
    }else if (!self.isVH){
        
        [UIView animateWithDuration:0.5 animations:^{
            
            self.view.transform = CGAffineTransformIdentity;
            self.view.bounds = CGRectMake(0, 0, ScreenWidth, ScreenHeight);
            _glLayer.frame = CGRectMake(0, 0,ScreenWidth,ScreenHeight);
            self.topBar.frame = CGRectMake(self.topBar.frame.origin.x,self.topBar.frame.origin.y,self.topBar.frame.size.width,70);
            _imgView.frame = CGRectMake(ScreenWidth-60, ScreenHeight-49-80, 50, 50);
            _showTimeLab.frame =CGRectMake(0, 70, ScreenWidth, 40);
            _showRedView.center = CGPointMake(_showTimeLab.center.x-40, _showTimeLab.center.y);
            self.isVH = YES;
            self.picBtn.selected = YES;
        }];
    }
}


#pragma mark - click Left-Right-UP-Down
//云台控制方式按下情况
-(void)clickTounchDownButton:(UIButton *)sender
{
    self.tempBtn = sender;
    [self doSomething];
    
}
//云台控制松开的情况
-(void)clickTounchUpInside:(UIButton *)sender
{
    self.tempBtn = sender;
    [self doNothing];
}


//松开手指后，延迟发送请求
- (void)doNothing
{
    secondsCountDown = Cloud_Time;
    [countDownTimer fire];
    NSLog(@"松开－－－－－－");
    if ( self.tempBtn.tag == 1000|| self.tempBtn.tag == 1001|| self.tempBtn.tag == 1002|| self.tempBtn.tag == 1003)
    {   //云台停止
        dispatch_time_t timeDelay = dispatch_time(DISPATCH_TIME_NOW, 100*NSEC_PER_MSEC);
        dispatch_queue_t queue = dispatch_get_main_queue();
        dispatch_after(timeDelay, queue, ^{
            
            [self sendUp];
        });
        
    }
    
    else
    {
        return;
    }
}
/**
 *  有云台的发上下左右控制协议
 *
 *  @param -- {\"TYPE\":\"CAMERA_COM\",\"ID\":%d,\"VAL\":\"%d:%d\"}
 */
- (void)doSomething{
    NSLog(@"按下--------");
    NSString *upString;
    if ( self.tempBtn.tag == 1000)
    {   //云台控制上
        
        upString = @"up";
    
        [self performSelectorOnMainThread:@selector(sendDownWithStr:) withObject:upString waitUntilDone:YES];
    }
    else if ( self.tempBtn.tag == 1001)
    {   //云台下
        upString = @"down";
  
        [self performSelectorOnMainThread:@selector(sendDownWithStr:) withObject:upString waitUntilDone:YES];
    }
    else if ( self.tempBtn.tag == 1002)
    {   //云台左
        
        upString = @"left";
       
        [self performSelectorOnMainThread:@selector(sendDownWithStr:) withObject:upString waitUntilDone:YES];
    }
    else if ( self.tempBtn.tag == 1003)
    {   //云台右
        
        upString = @"right";
      
        [self performSelectorOnMainThread:@selector(sendDownWithStr:) withObject:upString waitUntilDone:YES];
    }
    
    else
    {
        return;
    }
}


//按下
- (void)sendDownWithStr:(NSString *)str
{
    NSLog(@"按下------%@",str);
  
    [_requestTool requestControlCamera:[NSString stringWithFormat:@"http://121.40.50.44:10000/api/ptzcontrol?device=%@&channel=%@&actiontype=Continuous&command=%@&speed=5&protocol=onvif",self.urlModel.serial,self.urlModel.channel,str]];

}


//松开
- (void)sendUp
{
     [_requestTool requestControlCamera:[NSString stringWithFormat:@"http://121.40.50.44:10000/api/ptzcontrol?device=%@&channel=%@&actiontype=Continuous&command=%@&speed=5&protocol=onvif",self.urlModel.serial,self.urlModel.channel,@"stop"]];

}

#pragma mark - ClickBtn -- CallActions
//语音通话
- (void)callDidTouch: (id) sender
{
    
    UIButton *btn = (UIButton *)sender;
    if (!btn.selected) {
        [[AVAudioSession sharedInstance]requestRecordPermission:^(BOOL granted) {
            if (granted) {
                //已同意
                //发送开启通化协议
               

                    self.callSocket.kMovie = self;
//
                    [self.callSocket  cmsOpen];
                
                    MPMusicPlayerController *musicPlayer = [MPMusicPlayerController applicationMusicPlayer];
                    NSLog(@"--------%f",musicPlayer.volume);
                    musicPlayer.volume = 0;
                    
                    self.midBtn.selected = YES;
              
                
                
            }else
            {
                UIAlertView *alet = [[UIAlertView alloc] initWithTitle:@"提示" message:@"麦克风未授权"delegate:self cancelButtonTitle:@"确定" otherButtonTitles:@"取消", nil];
                [alet show];
                return;
            }
        }];
    }else
    {
        
//            [[SHServer instance].callSocket  stopCall];
//            [[SHServer instance].callSocket  disconnect];
//            [[SHServer instance].callSocket stopIntercom];
            self.midBtn.selected = NO;
            self.midBtn.enabled = YES;
            
     
    }
    
}
#pragma mark player call back
//发送音频协议
- (void)connectAudioSocket
{
    /*
     * VOICE_TYPE 表示音频喊话方式： 1： 直连喊话 2： 通过平台的喊话 3. 通过CMS喊话
     */
    int type ;
    //    if ([SHServer instance].connectIsPlatform) {
    //        //平台
    //        type = 3 ;
    //    }else
    //    {
    //直连
    type = 1;
    //    }
    //初始化协议
//    NSString *upString = [NSString stringWithFormat:JSON_Call,self.currentCamera.cID,type];
//    [DataRequest dataRequestWithJsonSting:upString andSession:SESSION_VIDEO_CALL_OPEN andProtocolType:ProtocolType_COM andDataType:DataType_Custom];
//    self.midBtn.enabled = NO;
//    [self hudNoticeMsg:NSLocalizedStringFromTable(@"喊话请求中", VideoLanguage, nil)];
//    dispatch_after(dispatch_time(DISPATCH_TIME_NOW, (int64_t)(15 * NSEC_PER_SEC)), dispatch_get_main_queue(), ^{
//        if (!self.midBtn.selected) {
//            if (!self.midBtn.enabled) {
//                [self hudNoticeMsg:NSLocalizedStringFromTable(@"喊话失败", VideoLanguage, nil)];
//                self.midBtn.enabled = YES;
//            }
//        }
//    });
    
    
}

//关闭通话
- (void)disconnect
{
    [self.callSocket stopIntercom];
//    //初始化协议
//    NSString *upString = [NSString stringWithFormat:JSON_CallSTOP,self.currentCamera.cID];
//    [DataRequest dataRequestWithJsonSting:upString andSession:SESSION_VIDEO_CALL_STOP andProtocolType:ProtocolType_COM andDataType:DataType_Custom];
    self.midBtn.enabled = NO;
//    [self hudNoticeMsg:@"喊话关闭中"];
    //    _HUD.label.text = @"喊话关闭中";
    //    [_HUD showAnimated:YES];
    
    [UIView animateWithDuration:15 animations:^{
        
    } completion:^(BOOL finished) {
        //        [_HUD hideAnimated:YES];
        if (self.midBtn.selected) {
            if (!self.midBtn.enabled) {
                self.midBtn.selected = NO;
                self.midBtn.enabled = YES;
            }
        }
    }];
    
}
/**
 *  数据提示
 *
 *  @param message 提示文字
 */
#pragma mark  - hudNotice
-(void)hudNoticeMsg:(NSString *)message{
    dispatch_async(dispatch_get_main_queue(),^{
        _hud.mode = MBProgressHUDModeText;
        _hud.detailsLabel.text = message;
        _hud.label.text = nil;
        [_hud showAnimated:YES];
    });
    
}
- (void)hidenMBView
{
    dispatch_async(dispatch_get_main_queue(),^{
        
        [_hud hideAnimated:YES];
    });
}
- (void)showIndicatorView{
    dispatch_async(dispatch_get_main_queue(),^{
        [_activityIndicatorView startAnimating];
    });
}


- (void)didReceiveMemoryWarning {
    [super didReceiveMemoryWarning];
    // Dispose of any resources that can be recreated.
}

@end


