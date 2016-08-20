//
//  CallBySocket.m
//  UIOT-SmartHome-Mobile
//
//  Created by TC on 15/11/21.
//  Copyright © 2015年 UIOT_YF. All rights reserved.
//

#import "CallBySocket.h"
#import "g711codec.h"
#import <netinet/tcp.h>
#import <netinet/in.h>
#import <MediaPlayer/MediaPlayer.h>
#import "GTMBase64.h"

NSMutableArray *receiveData;//接收数据的数组

char buffer[800];
char newBu[800];
char allData[1500];
GCDAsyncSocket             *_gcdsocekt;
@implementation CallBySocket

- (void)dealloc
{
    AudioQueueStop(_inputQueue, YES);
    run = 0;
    AudioQueueDispose(_inputQueue, YES);//销毁
}
-(NSDictionary *)parseJSONStringToNSDictionary:(NSString *)JSONString {
    NSData *JSONData = [JSONString dataUsingEncoding:NSUTF8StringEncoding];
    NSDictionary *responseJSON = [NSJSONSerialization JSONObjectWithData:JSONData options:NSJSONReadingMutableLeaves error:nil];
    return responseJSON;
}
- (void)cmsOpen
{

        //创建音频Socket
        BOOL connnect = [self CMSsenddata];
        if (!connnect) {
            [self.kMovie hudNotice:@"开启语音通话失败"];
            return;
        }
//        [self startIntercom];
    NSMutableData *sendData = [[NSMutableData alloc]init];
    NSString *shoutData =  @"";
    NSString *bodyStr = [NSString stringWithFormat:@"{\"EasyDarwin\":{\"Header\":{\"Version\":\"1.0\",\"CSeq\":\"123456\",\"MessageType\":\"MSG_CS_TALKBACK_CONTROL_REQ\"},\"Body\":{\"Channel\":\"0\",\"Command\":\"START\",\"AudioType\":\"G711A\",\"Protocol\":\"ONVIF\",\"Reserve\":\"1\",\"Serial\":\"%@\",\"AudioData\":\"%@\",\"Pts\":\"0\"}}}",@"001001000008",shoutData];
    NSData *bodyData = [bodyStr dataUsingEncoding:NSUTF8StringEncoding];
    //
    NSString *dataStr = [NSString stringWithFormat:@"POST /HTTP/1.1\r\nContent-Type:text/plain;charset=utf-8\r\nHost:%@:%@\r\nContent-Length:%d\r\nConnection:Keep-Alive\r\nAccept-Encoding:gzip\r\nUser-Agent:okhttp/3.2.0\r\n\r\n",LOGIN_CMS_ADDRESS,LOGIN_CMS_ADDRESS_PORT,bodyStr.length];
    //
    [sendData appendData:[dataStr dataUsingEncoding:NSUTF8StringEncoding]];
    [sendData appendData :bodyData];
    
    [_gcdsocekt performBlock:^{
        int fd = [_gcdsocekt socketFD];
        int on = 1;
        
        if (setsockopt(fd, IPPROTO_TCP, TCP_NODELAY, (char *)&on, sizeof(on))== -1) {
            //                        NSLog(@"设置TCP延迟失败");
        }
        [_gcdsocekt  writeData:sendData withTimeout:-1 tag:0];
    }];


}

//gcd socket 创建
- (BOOL)CMSsenddata
{
    NSError *error = nil;
    _gcdsocekt = [[GCDAsyncSocket alloc] initWithDelegate:self delegateQueue:dispatch_get_main_queue() socketQueue:nil];
  
        if (![_gcdsocekt connectToHost:LOGIN_CMS_ADDRESS onPort:[LOGIN_CMS_ADDRESS_PORT intValue] error:&error]) {
            NSLog(@"配置失败");
            [self.kMovie.midBtn setTitle:@"通话" forState:UIControlStateNormal];
            return NO;
        }

    return YES;
}

//初始化会话
- (void)initSession
{
    NSError *audioSessionError;
    AVAudioSession *audioSession = [AVAudioSession sharedInstance];
    [audioSession setCategory:AVAudioSessionCategoryPlayAndRecord error:&audioSessionError];
    
    if(audioSessionError)
    {
        NSLog(@"AVAudioSession error setting category:%@",audioSessionError);
    }
    else
    {
        [audioSession setActive:YES error:&audioSessionError];
        if(audioSessionError)
            NSLog(@"AVAudioSession error activating: %@",audioSessionError);
    }
}
//开始对讲
- (void)startIntercom
{
    //设置录音参数
    [self setupAudioFormat:kAudioFormatLinearPCM SampleRate:kDefaultSampleRate];
    _audioFormat.mSampleRate = kDefaultSampleRate;
    //设置话筒属性等
    [self initSession];
    //创造一个录制音频队列
    AudioQueueNewInput(&_audioFormat,GenericInputCallback,(__bridge void *)self,NULL,NULL,0,&_inputQueue);
    //创建录制音频队列缓冲区
    for (int i = 0; i < kNumberAudioQueueBuffers; i++) {
        if (self.carmelType == 81 || self.carmelType == 113) {
            AudioQueueAllocateBuffer(_inputQueue, kDefaultInputBufferSizeL, &_inputBuffers[i]);
        }else
        {
            AudioQueueAllocateBuffer(_inputQueue, kDefaultInputBufferSize, &_inputBuffers[i]);
        }
        AudioQueueEnqueueBuffer(_inputQueue,_inputBuffers[i],0,NULL);
    }
    run = 1;
    
    //开启录制队列
    AudioQueueStart(self.inputQueue, NULL);
}

//关闭对讲
- (void)stopIntercom
{
    AudioQueueStop(_inputQueue, YES);//停止
}

- (void)stopCall
{
    NSString *bodyStr = [NSString stringWithFormat:@"{\"UiotDarwin\":{\"Header\":{\"Version\":\"1.0\",\"CSeq\":\"654321\",\"MessageType\":\"MSG_CS_SHOUT_END_REQ\"},\"Body\":{\"UserID\":\"%@\",\"DeviceSerial\":\"%@\",\"CameraSerial\":\"%@\"}}}",@"",@"",@""];
    NSMutableData *sendData = [[NSMutableData alloc]init];
    NSLog(@"bodyStr.length:%d",(int)bodyStr.length);
    NSData *bodyData = [bodyStr dataUsingEncoding:NSUTF8StringEncoding];
    NSString *dataStr = [NSString stringWithFormat:@"POST http://%@:%@/clientinfo?sign=1&sessionid=%@    HTTP/1.1\r\nContent-Length:%d\r\n\r\n",LOGIN_CMS_ADDRESS,LOGIN_CMS_ADDRESS_PORT,@"" ,12];

    [sendData appendData:[dataStr dataUsingEncoding:NSUTF8StringEncoding]];
    [sendData appendData :bodyData];


     [_gcdsocekt  writeData:sendData withTimeout:-1 tag:0];

    MPMusicPlayerController *musicPlayer = [MPMusicPlayerController applicationMusicPlayer];
    musicPlayer.volume = 1.0;
    
}
#pragma mark - 私有方法
// 设置录音格式
- (void)setupAudioFormat:(UInt32) inFormatID SampleRate:(int)sampeleRate
{
    //重置下
    memset(&_audioFormat, 0, sizeof(_audioFormat));
    
    //设置采样率，这里先获取系统默认的测试下 //TODO:
    //采样率的意思是每秒需要采集的帧数 立体声8000
    _audioFormat.mSampleRate = sampeleRate;//[[AVAudioSession sharedInstance] sampleRate];
    
    //设置通道数,这里先使用系统的测试下 //TODO:（1 单声道  2 立体声）
    _audioFormat.mChannelsPerFrame = 1;//(UInt32)[[AVAudioSession sharedInstance] inputNumberOfChannels];
    
    //设置format，怎么称呼不知道。
    _audioFormat.mFormatID = inFormatID;
    
    if (inFormatID == kAudioFormatLinearPCM){
        //这个属性不知道干啥的。，
        _audioFormat.mFormatFlags = kLinearPCMFormatFlagIsSignedInteger | kLinearPCMFormatFlagIsPacked;
        //每个通道里，一帧采集的bit数目
        _audioFormat.mBitsPerChannel = 16;
        //结果分析: 8bit为1byte，即为1个通道里1帧需要采集2byte数据，再*通道数，即为所有通道采集的byte数目。
        //所以这里结果赋值给每帧需要采集的byte数目，然后这里的packet也等于一帧的数据。
        //至于为什么要这样。。。不知道。。。字节数目
        _audioFormat.mBytesPerPacket = _audioFormat.mBytesPerFrame = (_audioFormat.mBitsPerChannel / 8) * _audioFormat.mChannelsPerFrame;
        //packet中包含的frame数目，无压缩时为1，可变比特率时，一个达点儿的固定值例如在ACC中1024。
        _audioFormat.mFramesPerPacket = 1;
    }
}


//录音回调
void GenericInputCallback (
                           void                                *inUserData,
                           AudioQueueRef                       inAQ,
                           AudioQueueBufferRef                 inBuffer,
                           const AudioTimeStamp                *inStartTime,
                           UInt32                              inNumberPackets,
                           const AudioStreamPacketDescription  *inPacketDescs
                           )
{
    NSLog(@"录音回调方法");
    if (inNumberPackets > 0) {
        
        NSData *pcmData = [[NSData alloc] initWithBytes:inBuffer->mAudioData length:inBuffer->mAudioDataByteSize];
        //pcm数据不为空，编码为g711a
        memset(buffer, 0, 800);
        memset(newBu, 0, 800);
        memset(allData, 0, 1500);
        
        char *c = (char*)[pcmData bytes];
        int size = PCM2G711a(c, buffer, (int)pcmData.length, 0);
        
        if (size <= 0) {
            return;
        }
        if (pcmData && pcmData.length > 0) {
            
            NSString *rstp = [NSString string];
          
                //平台 CMS
                NSMutableData *sendData = [[NSMutableData alloc]init];
                NSString *shoutData =  [GTMBase64 stringByEncodingBytes:buffer length:size];
                NSString *bodyStr = [NSString stringWithFormat:@"{\"EasyDarwin\":{\"Header\":{\"Version\":\"1.0\",\"CSeq\":\"123456\",\"MessageType\":\"MSG_CS_TALKBACK_CONTROL_REQ\"},\"Body\":{\"Channel\":\"0\",\"Command\":\"START\",\"AudioType\":\"G711A\",\"Protocol\":\"ONVIF\",\"Reserve\":\"1\",\"Serial\":\"%@\",\"Preset\":\"3\",\"AudioData\":\"%@\",\"Pts\":\"0\"}}}",@"001001000008",shoutData];
              NSData *bodyData = [bodyStr dataUsingEncoding:NSUTF8StringEncoding];
//
              NSString *dataStr = [NSString stringWithFormat:@"POST http://%@:%@/HTTP/1.1\r\nCSeq:1\r\nContent-Length:%d\r\n\r\n",LOGIN_CMS_ADDRESS,LOGIN_CMS_ADDRESS_PORT,bodyData.length];
//
                [sendData appendData:[dataStr dataUsingEncoding:NSUTF8StringEncoding]];
                [sendData appendData :bodyData];
            
                [_gcdsocekt performBlock:^{
                    int fd = [_gcdsocekt socketFD];
                    int on = 1;
                    
                    if (setsockopt(fd, IPPROTO_TCP, TCP_NODELAY, (char *)&on, sizeof(on))== -1) {
//                        NSLog(@"设置TCP延迟失败");
                    }
                    [_gcdsocekt  writeData:sendData withTimeout:-1 tag:0];
                }];
                
             
            
        }
    }
    AudioQueueEnqueueBuffer(inAQ,inBuffer,0,NULL);
    
}


//链接成功
- (void)socket:(GCDAsyncSocket *)sock didConnectToHost:(NSString *)host port:(uint16_t)port
{
    NSLog(@"音频socket连接成功");
    
}

//连接失败
- (void)socketDidDisconnect:(GCDAsyncSocket *)sock withError:(NSError *)err
{
    NSLog(@"音频socket连接失败：%@",err);
    [self.kMovie disconnect];
    self.kMovie.midBtn.selected = NO;
}
- (void)writeData:(NSData *)data withTimeout:(NSTimeInterval)timeout tag:(long)tag
{
    
}
- (void)socket:(GCDAsyncSocket *)sock didWriteDataWithTag:(long)tag
{
    NSLog(@"发送数据");
        [_gcdsocekt readDataWithTimeout:-1 tag:100];
}
- (void)socket:(GCDAsyncSocket *)sock didReadData:(NSData *)data withTag:(long)tag
{

    NSString* aStr=[[NSString alloc] initWithData:data encoding:NSASCIIStringEncoding];
       NSLog(@"============%@",aStr)
}

- (void)disconnect
{
    _gcdsocekt.delegate = nil;
    _gcdsocekt = nil;
}

@end
