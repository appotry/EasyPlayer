#define CAMERA_TURN_SERVICE_PORT 6899
#define RTSP_CONN_TIMEOUT 5
#define RTSP_CLOSE_DETECT_INTERVAL_MICSEC 500
#define Cloud_Stop 0


#import "PlayViewController.h"
#import <VideoToolbox/VideoToolbox.h>
#import <AudioToolbox/AudioToolbox.h>
#import <MediaPlayer/MediaPlayer.h>
#import <AVFoundation/AVFoundation.h>
#include <pthread.h>

#import "VideoPacket.h"
#import "AAPLEAGLLayer.h"
#import "EasyRTSPClientAPI.h"
#import "ToolBar.h"
#import "EasyUrl.h"
#import "GTMBase64.h"
#import "g711codec.h"

PlayViewController* pvc = nil;
BOOL isSaveAudioAddVideo; //是否开始保存视频
UIActivityIndicatorView *_activityIndicatorView;
BOOL isTakePhoto;//是否抓拍

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

//
VTDecompressionSessionRef _deocderSession;
CMVideoFormatDescriptionRef _decoderFormatDescription;

AAPLEAGLLayer *_glLayer;

CVPixelBufferRef decode(VideoPacket* vp);

static void didDecompress( void *decompressionOutputRefCon, void *sourceFrameRefCon, OSStatus status, VTDecodeInfoFlags infoFlags, CVImageBufferRef pixelBuffer, CMTime presentationTimeStamp, CMTime presentationDuration ){
    
    CVPixelBufferRef *outputPixelBuffer = (CVPixelBufferRef *)sourceFrameRefCon;
    *outputPixelBuffer = CVPixelBufferRetain(pixelBuffer);
}
void creatImgs(UIImage *img);
BOOL initH264Decoder(EASY_MEDIA_INFO_T* pMediaInfo);
void destroyAudio();
BOOL initAudio(EASY_MEDIA_INFO_T* pMediaInfo);


// 音频
#define MIN_SIZE_PER_FRAME 2000 //每侦最小数据长度//penggy:之前2000有点小，会闪退,改为20000
AudioQueueRef audioQueue = NULL;//音频播放队列
AudioQueueBufferRef outQB;//音频缓存
void readPCMAndPlay();

//播放
void configPlayer(NSString *urlString);

int RTSPClientCallBack( int _chid, int *_chPtr, int _frameType, char *_pBuf, RTSP_FRAME_INFO *_frameInfo)
{
    if (_frameType == EASY_SDK_AUDIO_FRAME_FLAG && _frameInfo != NULL && _pBuf != NULL)
    {
        if(!audioQueue || _frameInfo->length >= MIN_SIZE_PER_FRAME || _frameInfo->length <= 0){
            return 0;
        }
        memset(outQB->mAudioData,0,MIN_SIZE_PER_FRAME);
        outQB->mAudioDataByteSize = _frameInfo->length;
        memcpy(outQB->mAudioData,_pBuf,outQB->mAudioDataByteSize);
        AudioQueueEnqueueBuffer(audioQueue, outQB, 0, NULL);
    }
    else if (_frameType == EASY_SDK_VIDEO_FRAME_FLAG && _frameInfo != NULL && _pBuf != NULL)
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
                    memcpy(vp.buffer, _pBuf + [as[count] integerValue]-3, NaluSize);
                    
                    uint32_t nalSize = (uint32_t)(vp.size - 4);
                    uint8_t *pNalSize = (uint8_t*)(&nalSize);
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
                
                if(initH264Decoder(nil)) {
                    pixelBuffer =decode(vp);
                }
                free(vp.buffer);
                vp.buffer = NULL;
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
                
                if(initH264Decoder(nil)) {
                    pixelBuffer =decode(vp);
                }
               free(vp.buffer);
                vp.buffer = NULL;
                
            }
            if(pixelBuffer) {
                dispatch_sync(dispatch_get_main_queue(), ^{
                    _glLayer.pixelBuffer = pixelBuffer;
                });
                
                CVPixelBufferRelease(pixelBuffer);
                if(_pps)
                {
                    free(_pps);
                    _pps=NULL;
                }
                if(_sps)
                {
                    free(_sps);
                    _sps=NULL;
                }
            }
            
        }
    }
    else if (_frameType == EASY_SDK_MEDIA_INFO_FLAG && _pBuf != NULL)//回调出媒体信息
    {
        NSLog(@"got media info , now can init decoder");
        EASY_MEDIA_INFO_T mediainfo;
        memset(&mediainfo, 0x00, sizeof(EASY_MEDIA_INFO_T));
        memcpy(&mediainfo, _pBuf, sizeof(EASY_MEDIA_INFO_T));
        NSLog(@"RTSP DESCRIBE Get Media Info: video:%u fps:%u audio:%u channel:%u sampleRate:%u \n",
              mediainfo.u32VideoCodec, mediainfo.u32VideoFps, mediainfo.u32AudioCodec, mediainfo.u32AudioChannel, mediainfo.u32AudioSamplerate);
        //        initH264Decoder(&mediainfo);
        initAudio(&mediainfo);
    }
    return 0;
}



BOOL initH264Decoder(EASY_MEDIA_INFO_T* pMediaInfo)
{
    if(_deocderSession){
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
    
    if(status != noErr) {
        NSLog(@"init video erro : %d",(int)status);
        return NO;
    }
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

void configPlayer(NSString *urlString) {
    const  char *url =  [urlString UTF8String];
    const  char *username = "admin";
    const   char *password = "admin";
    EasyRTSP_Init(&rtspHandle);
    EasyRTSP_SetCallback(rtspHandle, RTSPClientCallBack);
    EasyRTSP_OpenStream(rtspHandle, 1, (char *)url, RTP_OVER_TCP, EASY_SDK_VIDEO_FRAME_FLAG|EASY_SDK_AUDIO_FRAME_FLAG, (char *)username,(char *)password, NULL, 1000, 0,1);
}

@interface PlayViewController()
{
    
    BOOL                _fullscreen;
    BOOL                _isControlsShow;
    
    AudioStreamBasicDescription     _audioFormat;//声音格式，用于喊话
    
    AudioQueueBufferRef     _inputBuffers[kNumberAudioQueueBuffers];
    AudioQueueBufferRef     _outputBuffers[kNumberAudioQueueBuffers];
    
    
    NSTimer *_isControlsShowTimer;
    MBProgressHUD * _hud;
}
@property (assign, nonatomic) AudioQueueRef                 inputQueue;//录音对列
@property (assign)BOOL isVH; //横竖屏
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

- (void)viewWillDisappear:(BOOL)animated
{

    [super viewWillDisappear:animated];
    dispatch_queue_t globalQueue=dispatch_get_global_queue(DISPATCH_QUEUE_PRIORITY_DEFAULT, 0);
    dispatch_async(globalQueue, ^{
        EasyRTSP_CloseStream(rtspHandle);
        EasyRTSP_Deinit(&rtspHandle);
        [self clearH264Deocder];
        if(self.midBtn.selected){//stop talk if talking
            [self talkStop];
        }
        destroyAudio();
    });
    if (pvc) {
        pvc = nil;
    }
}

- (void)viewDidLoad {
    [super viewDidLoad];
    self.view.backgroundColor = [UIColor blackColor];
    self.title = @"视频播放";
    _glLayer = [[AAPLEAGLLayer alloc] initWithFrame:CGRectMake(0, 0, ScreenWidth, ScreenHeight)];
    _glLayer.backgroundColor = [UIColor blackColor].CGColor;
    [self.view.layer addSublayer:_glLayer];
    
    [self initSomeView];
    configPlayer(self.urlModel.url);
    pvc = self;

}

- (BOOL)prefersStatusBarHidden{
    return YES;
}

- (void)initSomeView
{
    self.isVH = YES;
    _isControlsShow = YES; //默认是显示的
    CGFloat botH = 50;
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
    
    _activityIndicatorView = [[UIActivityIndicatorView alloc] initWithActivityIndicatorStyle: UIActivityIndicatorViewStyleWhiteLarge];
    _activityIndicatorView.center = self.view.center;
    _activityIndicatorView.autoresizingMask = UIViewAutoresizingFlexibleTopMargin | UIViewAutoresizingFlexibleBottomMargin | UIViewAutoresizingFlexibleLeftMargin | UIViewAutoresizingFlexibleRightMargin;
    
    [self.view addSubview:_activityIndicatorView];
    
    [self.botBar setItems:@[self.upBtnItem, spaceItem1, self.downBtnItem, spaceItem2,
                            self.leftBtnItem, spaceItem3,self.rightBtnItem, spaceItem4,self.jirtuItem] animated:NO];
    _hud = [[MBProgressHUD alloc] initWithView:self.view];
    _hud.mode = MBProgressHUDModeIndeterminate;
    [_hud.button addTarget:self action:@selector(hidenMBView) forControlEvents:UIControlEventTouchUpInside];
    [_hud.button setTitle:NSLocalizedString(@"确定",@"") forState:UIControlStateNormal];
    [self.view addSubview:_hud];
    [self resetControlsShowTimer];
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
    //    free(_sps);
    //    free(_pps);
    _spsSize = _ppsSize = 0;
}

void destroyAudio()
{
    if(audioQueue){
        AudioQueueStop(audioQueue, YES);
        AudioQueueFreeBuffer(audioQueue, outQB);
        AudioQueueDispose(audioQueue, YES);
        audioQueue = NULL;
    }
}

//**************** 音频  *******************
BOOL initAudio(EASY_MEDIA_INFO_T* pMediaInfo)
{
    NSError *audioSessionError;
    AVAudioSession *audioSession = [AVAudioSession sharedInstance];
    [audioSession setCategory:AVAudioSessionCategoryAmbient error:&audioSessionError];
    if(audioSessionError)
    {
        NSLog(@"AVAudioSession error setting category:%@",audioSessionError);
    }
    [audioSession setActive:YES error:&audioSessionError];
    if(audioSessionError){
        NSLog(@"AVAudioSession error activating: %@",audioSessionError);
    }
    
    AudioStreamBasicDescription audioDescription = {0};
    switch (pMediaInfo->u32AudioCodec) {
        case EASY_SDK_AUDIO_CODEC_AAC:
            NSLog(@"初始化AAC解码器");
            audioDescription.mFormatID = kAudioFormatMPEG4AAC;
            audioDescription.mFramesPerPacket = 1024;//每一个packet一侦数据
            audioDescription.mSampleRate =  pMediaInfo->u32AudioSamplerate;
            audioDescription.mChannelsPerFrame = pMediaInfo->u32AudioChannel;//1;///单声道
            audioDescription.mFormatFlags = 0;
            break;
        case EASY_SDK_AUDIO_CODEC_G711U:

        case EASY_SDK_AUDIO_CODEC_G711A:

        case EASY_SDK_AUDIO_CODEC_G726:

        default:
            audioDescription.mSampleRate = pMediaInfo->u32AudioSamplerate;//8000;//采样率
            audioDescription.mFormatID = kAudioFormatALaw;
            audioDescription.mFramesPerPacket = 1;//每一个packet一侦数据
            audioDescription.mFormatFlags = kLinearPCMFormatFlagIsSignedInteger | kAudioFormatFlagIsPacked;
            audioDescription.mChannelsPerFrame = pMediaInfo->u32AudioChannel;//1;///单声道
            audioDescription.mBitsPerChannel = 16;//每个采样点16bit量化
            audioDescription.mBytesPerFrame = (audioDescription.mBitsPerChannel/8) * audioDescription.mChannelsPerFrame;
            audioDescription.mBytesPerPacket = audioDescription.mBytesPerFrame ;
            break;
    }
    OSStatus err = AudioQueueNewOutput(&audioDescription, AudioPlayerAQInputCallback, (__bridge void * _Nullable)(pvc), nil, nil, 0, &audioQueue);//使用player的内部线程播
    if(err != noErr){
        NSLog(@"AudioQueueNewOutput 不成功");
        return NO;
    }
    
    UInt32 val = kAudioQueueHardwareCodecPolicy_PreferSoftware;//在软解码不可用的情况下用硬解码
    err = AudioQueueSetProperty(audioQueue, kAudioQueueProperty_HardwareCodecPolicy, &val, sizeof(UInt32));
    
    err = AudioQueueAllocateBuffer(audioQueue, MIN_SIZE_PER_FRAME, &outQB);
    if(err != noErr){
        NSLog(@"AudioQueueAllocateBuffer 不成功");
        AudioQueueDispose(audioQueue, TRUE);
        audioQueue = NULL;
        return NO;
    }

    AudioQueueSetParameter(audioQueue, kAudioQueueParam_Volume, 1.0);
    err = AudioQueueStart(audioQueue, NULL);
    if(err != noErr){
        destroyAudio();
        NSLog(@"音频播放失败, err = %d",(int)err);
        return NO;
    }
    return YES;
}

static void AudioPlayerAQInputCallback(void *input, AudioQueueRef outQ, AudioQueueBufferRef outQB)
{
}

- (void) handleTap
{
    [self setControlsVisable:!_isControlsShow animated:YES];
}

- (void)resetControlsShowTimer
{
    if (!_isControlsShowTimer)
        _isControlsShowTimer = [NSTimer scheduledTimerWithTimeInterval:4.
                                                                target:self
                                                              selector:@selector(conrolsShowTimerExceeded)
                                                              userInfo:nil
                                                               repeats:NO];
    [_isControlsShowTimer setFireDate:[NSDate dateWithTimeIntervalSinceNow:4.]];
}

- (void)conrolsShowTimerExceeded
{
    _isControlsShowTimer = nil;
    [self setControlsVisable:FALSE animated:YES];
}

- (void)setControlsVisable:(BOOL)visable animated:(BOOL)animated
{
    _isControlsShow = visable;
    [UIView animateWithDuration:0.1
                          delay:0.0
                        options:UIViewAnimationOptionCurveEaseInOut | UIViewAnimationOptionTransitionNone
                     animations:^{
                         CGFloat alpha = visable ? 1 : 0;
                         self.topBar.alpha = alpha;
                         self.botBar.alpha = alpha;
                     }
                     completion:nil];
    
    if(visable){
        [self resetControlsShowTimer];
    }
    
}

- (void) doneDidTouch: (id) sender
{
    [self dismissViewControllerAnimated:YES completion:nil];
}

// 录像功能
- (void)videotapeBtnClick:(id)sender
{
    isSaveAudioAddVideo = !isSaveAudioAddVideo;
}
// 横屏 竖屏
- (void)tapscreen
{
    if (self.isVH) {
        [UIView animateWithDuration:0.5 animations:^{
            self.view.transform =CGAffineTransformMakeRotation(M_PI_2);
            self.view.bounds = CGRectMake(0, 0,ScreenHeight,ScreenWidth);
            _glLayer.frame = CGRectMake(0, 0,ScreenHeight,ScreenWidth);
            self.topBar.frame = CGRectMake(self.topBar.frame.origin.x,self.topBar.frame.origin.y,ScreenHeight,50);
            self.botBar.frame = CGRectMake(0,ScreenWidth-50,ScreenHeight,50);
            
            self.isVH = NO;
            self.picBtn.selected = NO;
        }];
        
    }else if (!self.isVH){
        
        [UIView animateWithDuration:0.5 animations:^{
            self.view.transform = CGAffineTransformIdentity;
            self.view.bounds = CGRectMake(0, 0, ScreenWidth, ScreenHeight);
            _glLayer.frame = CGRectMake(0, 0,ScreenWidth,ScreenHeight);
            self.topBar.frame = CGRectMake(self.topBar.frame.origin.x,self.topBar.frame.origin.y,self.topBar.frame.size.width,70);
            self.isVH = YES;
            self.picBtn.selected = YES;
        }];
    }
}

//云台控制方式按下情况
-(void)clickTounchDownButton:(UIButton *)sender
{
    self.tempBtn = sender;
    [self doControlStart];
    [self resetControlsShowTimer];
}
//云台控制松开的情况
-(void)clickTounchUpInside:(UIButton *)sender
{
    self.tempBtn = sender;
    [self doControlStop];
    [self resetControlsShowTimer];
}


//松开手指后，延迟发送请求
- (void)doControlStop
{
    if ( self.tempBtn.tag == 1000|| self.tempBtn.tag == 1001|| self.tempBtn.tag == 1002|| self.tempBtn.tag == 1003)
    {   //云台停止
        dispatch_time_t timeDelay = dispatch_time(DISPATCH_TIME_NOW, 100*NSEC_PER_MSEC);
        dispatch_queue_t queue = dispatch_get_main_queue();
        dispatch_after(timeDelay, queue, ^{
            [self controlCameraWidthCmd: @"stop"];
        });
    }
}

- (void)doControlStart{
    if ( self.tempBtn.tag == 1000)
    {   //云台上
        [self performSelectorOnMainThread:@selector(controlCameraWidthCmd:) withObject:@"up" waitUntilDone:YES];
    }
    else if ( self.tempBtn.tag == 1001)
    {   //云台下
        [self performSelectorOnMainThread:@selector(controlCameraWidthCmd:) withObject:@"down" waitUntilDone:YES];
    }
    else if ( self.tempBtn.tag == 1002)
    {   //云台左
        [self performSelectorOnMainThread:@selector(controlCameraWidthCmd:) withObject:@"left" waitUntilDone:YES];
    }
    else if ( self.tempBtn.tag == 1003)
    {   //云台右
        [self performSelectorOnMainThread:@selector(controlCameraWidthCmd:) withObject:@"right" waitUntilDone:YES];
    }
}

//控制摄像头
- (void)controlCameraWidthCmd:(NSString*) cmd
{
    NSLog(@"请求控件摄像头, cmd = %@", cmd);
    NSString *cmsIp = [[NSUserDefaults standardUserDefaults] stringForKey:@"cms_ip"];
    NSString *cmsPort = [[NSUserDefaults standardUserDefaults] stringForKey:@"cms_port"];
    NSString* url = [NSString stringWithFormat:@"http://%@:%@/api/ptzcontrol?device=%@&channel=%@&actiontype=Continuous&command=%@&speed=5&protocol=onvif",cmsIp,cmsPort,self.urlModel.serial,self.urlModel.channel,cmd];
    AFHTTPSessionManager * manager = [AFHTTPSessionManager manager];
    manager.responseSerializer.acceptableContentTypes = [NSSet setWithObjects:@"html/text",@"text/plain", nil];
    
    [manager GET:url parameters:nil progress:^(NSProgress * _Nonnull downloadProgress) {
    } success:^(NSURLSessionDataTask * _Nonnull task, id  _Nullable responseObject) {
    } failure:^(NSURLSessionDataTask * _Nullable task, NSError * _Nonnull error) {
        NSLog(@"error====%@",error);
    }];
}

- (void)callDidTouch: (id) sender
{
    UIButton *btn = (UIButton *)sender;
    if (!btn.selected) {
        AVAudioSession* audioSession = [AVAudioSession sharedInstance];
        [audioSession requestRecordPermission:^(BOOL granted) {
            if (granted) {
                [self talkStart];
            }
            else
            {
                UIAlertView *alet = [[UIAlertView alloc] initWithTitle:@"提示" message:@"麦克风未授权"delegate:self cancelButtonTitle:@"确定" otherButtonTitles:@"取消", nil];
                [alet show];
                return;
            }
        }];
    }
    else
    {
        [self talkStop];
    }
    
}
/**
 *  数据提示
 *
 *  @param message 提示文字
 */
- (void)hudNotice:(NSString *)message
{
    [self hudNoticeMsg:message];
}

-(void)hudNoticeMsg:(NSString *)message
{
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

- (void)talkStart
{
    NSString *cmsIp = [[NSUserDefaults standardUserDefaults] stringForKey:@"cms_ip"];
    NSString *cmsPort = [[NSUserDefaults standardUserDefaults] stringForKey:@"cms_port"];
//        MPMusicPlayerController *musicPlayer = [MPMusicPlayerController applicationMusicPlayer];
//        musicPlayer.volume = 0;
    
    NSError *audioSessionError;
    AVAudioSession *audioSession = [AVAudioSession sharedInstance];
    [audioSession setCategory:AVAudioSessionCategoryPlayAndRecord error:&audioSessionError];
    if(audioSessionError)
    {
        NSLog(@"AVAudioSession error setting category:%@",audioSessionError);
    }
    [audioSession setActive:YES error:&audioSessionError];
    if(audioSessionError){
        NSLog(@"AVAudioSession error activating: %@",audioSessionError);
    }
    
    memset(&_audioFormat, 0, sizeof(_audioFormat));
    _audioFormat.mSampleRate = 8000;
    _audioFormat.mChannelsPerFrame = 1;//(UInt32)[[AVAudioSession sharedInstance] inputNumberOfChannels];
    _audioFormat.mFormatID = kAudioFormatLinearPCM;
    if (_audioFormat.mFormatID == kAudioFormatLinearPCM){
        _audioFormat.mFormatFlags = kLinearPCMFormatFlagIsSignedInteger | kLinearPCMFormatFlagIsPacked;
        _audioFormat.mBitsPerChannel = 16;
        _audioFormat.mBytesPerPacket = _audioFormat.mBytesPerFrame = (_audioFormat.mBitsPerChannel / 8) * _audioFormat.mChannelsPerFrame;
        _audioFormat.mFramesPerPacket = 1;
    }
    
    //创造一个录制音频队列
    AudioQueueNewInput(&_audioFormat,AudioRecordCallback,(__bridge void *)self,NULL,NULL,0,&_inputQueue);
    //创建录制音频队列缓冲区
    for (int i = 0; i < kNumberAudioQueueBuffers; i++) {
        AudioQueueAllocateBuffer(self.inputQueue, kDefaultInputBufferSize, &_inputBuffers[i]);
        AudioQueueEnqueueBuffer(self.inputQueue,_inputBuffers[i],0,NULL);
    }
    
    NSString* params = [NSString stringWithFormat:@"{\"EasyDarwin\":{\"Header\":{\"CSeq\":\"1\",\"MessageType\":\"MSG_CS_TALKBACK_CONTROL_REQ\",\"Version\":\"1.0\"},\"Body\":{\"Channel\":\"%@\",\"Command\":\"START\",\"AudioType\":\"G711A\",\"AudioData\":\"\",\"Protocol\":\"%@\",\"Reserve\":\"1\",\"Pts\":\"\",\"Serial\":\"%@\"}}}",pvc.urlModel.channel,@"ONVIF",pvc.urlModel.serial];
    
    NSURL *url = [NSURL URLWithString:[NSString stringWithFormat:@"http://%@:%@/",cmsIp, cmsPort]];
    NSMutableURLRequest *request = [[NSMutableURLRequest alloc]initWithURL:url cachePolicy:NSURLRequestUseProtocolCachePolicy timeoutInterval:10];
    [request setHTTPMethod:@"POST"];//设置请求方式为POST，默认为GET
    [request setHTTPBody:[params dataUsingEncoding:NSISOLatin1StringEncoding]];
    NSData *received = [NSURLConnection sendSynchronousRequest:request returningResponse:nil error:nil];
    NSString *res = [[NSString alloc]initWithData:received encoding:NSISOLatin1StringEncoding];
    AudioQueueStart(self.inputQueue, NULL);
    self.midBtn.selected = YES;
    
}

- (void)talkStop
{
    NSString *cmsIp = [[NSUserDefaults standardUserDefaults] stringForKey:@"cms_ip"];
    NSString *cmsPort = [[NSUserDefaults standardUserDefaults] stringForKey:@"cms_port"];
    AudioQueueStop(self.inputQueue, YES);//停止
    
    NSString* params = [NSString stringWithFormat:@"{\"EasyDarwin\":{\"Header\":{\"CSeq\":\"1\",\"MessageType\":\"MSG_CS_TALKBACK_CONTROL_REQ\",\"Version\":\"1.0\"},\"Body\":{\"Channel\":\"%@\",\"Command\":\"STOP\",\"AudioType\":\"G711A\",\"AudioData\":\"\",\"Protocol\":\"%@\",\"Reserve\":\"1\",\"Pts\":\"\",\"Serial\":\"%@\"}}}",pvc.urlModel.channel,@"ONVIF",pvc.urlModel.serial];
    
    NSURL *url = [NSURL URLWithString:[NSString stringWithFormat:@"http://%@:%@/",cmsIp, cmsPort]];
    NSMutableURLRequest *request = [[NSMutableURLRequest alloc]initWithURL:url cachePolicy:NSURLRequestUseProtocolCachePolicy timeoutInterval:10];
    [request setHTTPMethod:@"POST"];//设置请求方式为POST，默认为GET
    [request setHTTPBody:[params dataUsingEncoding:NSISOLatin1StringEncoding]];
    NSData *received = [NSURLConnection sendSynchronousRequest:request returningResponse:nil error:nil];
    NSString *res = [[NSString alloc]initWithData:received encoding:NSISOLatin1StringEncoding];
    self.midBtn.selected = NO;
    
    NSError *audioSessionError;
    AVAudioSession *audioSession = [AVAudioSession sharedInstance];
    [audioSession setCategory:AVAudioSessionCategoryAmbient error:&audioSessionError];
    if(audioSessionError)
    {
        NSLog(@"AVAudioSession error setting category:%@",audioSessionError);
    }
    [audioSession setActive:YES error:&audioSessionError];
    if(audioSessionError){
        NSLog(@"AVAudioSession error activating: %@",audioSessionError);
    }
}


- (void)didReceiveMemoryWarning {
    [super didReceiveMemoryWarning];
}

void AudioRecordCallback (
                          void                                *inUserData,
                          AudioQueueRef                       inAQ,
                          AudioQueueBufferRef                 inBuffer,
                          const AudioTimeStamp                *inStartTime,
                          UInt32                              inNumberPackets,
                          const AudioStreamPacketDescription  *inPacketDescs
                          )
{
    NSString *cmsIp = [[NSUserDefaults standardUserDefaults] stringForKey:@"cms_ip"];
    NSString *cmsPort = [[NSUserDefaults standardUserDefaults] stringForKey:@"cms_port"];
    if (inNumberPackets > 0) {
        NSData *pcmData = [[NSData alloc] initWithBytes:inBuffer->mAudioData length:inBuffer->mAudioDataByteSize];
        //pcm数据不为空，编码为g711a
        char buffer[8000] = {0};
        
        int size = PCM2G711a((char*)[pcmData bytes], buffer, (int)pcmData.length, 0);
        if (size <= 0) {
            return;
        }
        if (pcmData && pcmData.length > 0) {
            
            NSString* params = [NSString stringWithFormat:@"{\"EasyDarwin\":{\"Header\":{\"CSeq\":\"1\",\"MessageType\":\"MSG_CS_TALKBACK_CONTROL_REQ\",\"Version\":\"1.0\"},\"Body\":{\"Channel\":\"%@\",\"Command\":\"SENDDATA\",\"AudioType\":\"G711A\",\"AudioData\":\"%@\",\"Protocol\":\"%@\",\"Reserve\":\"1\",\"Pts\":\"%@\",\"Serial\":\"%@\"}}}",pvc.urlModel.channel,[GTMBase64 stringByEncodingBytes:buffer length:size],@"ONVIF",[NSString stringWithFormat:@"%d",pvc.pts.intValue],pvc.urlModel.serial];
            
            NSURL *url = [NSURL URLWithString:[NSString stringWithFormat:@"http://%@:%@/",cmsIp, cmsPort]];
            NSMutableURLRequest *request = [[NSMutableURLRequest alloc]initWithURL:url cachePolicy:NSURLRequestUseProtocolCachePolicy timeoutInterval:10];
            [request setHTTPMethod:@"POST"];
            [request setHTTPBody:[params dataUsingEncoding:NSISOLatin1StringEncoding]];
            NSData *received = [NSURLConnection sendSynchronousRequest:request returningResponse:nil error:nil];
            NSString *res = [[NSString alloc]initWithData:received encoding:NSISOLatin1StringEncoding];
        }
    }
    AudioQueueEnqueueBuffer(inAQ,inBuffer,0,NULL);
}

@end



