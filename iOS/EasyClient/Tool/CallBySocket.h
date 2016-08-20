#import <Foundation/Foundation.h>
#import <AudioToolbox/AudioToolbox.h>
#import <AVFoundation/AVFoundation.h>
#import "GCDAsyncUdpSocket.h"
#import "GCDAsyncSocket.h"
#import "EasyInfo.h"
#import "AsyncSocket.h"
#import "PlayViewController.h"

/**
 *  缓存区的个数，一般3个
 */
#define kNumberAudioQueueBuffers 1

#define kNumberAudioQueueBuffers1 100
/**
 *  采样率，要转码为G711U的话必须为8000
 */
#define kDefaultSampleRate 8000

#define kDefaultInputBufferSize 1600
#define kDefaultInputBufferSizeL 320
#define kDefaultOutputBufferSize 7040


#define kDefaultPort  8080

@interface CallBySocket : NSObject<GCDAsyncUdpSocketDelegate,GCDAsyncSocketDelegate,AsyncSocketDelegate>
{
    AudioQueueRef                   _inputQueue;
    AudioQueueRef                   _outputQueue;
    AudioStreamBasicDescription     _audioFormat;//声音格式
    
    AudioQueueBufferRef     _inputBuffers[kNumberAudioQueueBuffers];
    AudioQueueBufferRef     _outputBuffers[kNumberAudioQueueBuffers];
    int run;
    
}

@property (strong,nonatomic) PlayViewController     *kMovie;
@property (strong, nonatomic) EasyInfo                 *camerModel;//摄像头Model
@property (assign, nonatomic) NSInteger                         countNum;
@property (assign, nonatomic) NSInteger                         carmelType;//摄像头型号
@property (assign, nonatomic) AudioQueueRef                 inputQueue;//录音对列
@property (assign, nonatomic) AudioQueueRef                 outputQueue;
@property (strong, nonatomic) AsyncSocket                   * socekt;
@property (strong, nonatomic) NSData                           *sendData;//发送的数据
@property (nonatomic) BOOL                                         runLoopThreadDidFinishFlag;//子线程是否停止
@property (strong, nonatomic)NSThread                         *thread;
@property(strong,nonatomic)NSString *deviceSerial;
@property(strong,nonatomic)NSString *cameraSerial;
@property(strong,nonatomic)NSString *sessionId;

//开始对讲
- (void)startIntercom;
- (void)cmsOpen;
//关闭对讲
- (void)stopIntercom;

//断开连接
- (void)disconnect;


// 互联网关闭喊话
- (void)stopCall;

@end
