//
//  ViewController.m
//  scan_qrcode_demo
//
//  Created by strivingboy on 14/11/3.
//
//

#import "ScanViewContriller.h"

#import <AVFoundation/AVFoundation.h>

static const char *kScanQRCodeQueueName = "ScanQRCodeQueue";

@interface ScanViewContriller () <AVCaptureMetadataOutputObjectsDelegate>
{
    UIImageView * anmationLine;
    int num;
    BOOL upOrdown;
    NSTimer * timer;
}
@property (strong, nonatomic)  UIView *sanFrameView;
//@property (weak, nonatomic) IBOutlet UIButton *button;
//@property (weak, nonatomic) IBOutlet UIButton *lightButton;

@property (nonatomic) AVCaptureSession *captureSession;
@property (nonatomic) AVCaptureVideoPreviewLayer *videoPreviewLayer;
@property (nonatomic) BOOL lastResut;
@end

@implementation ScanViewContriller

- (void)viewDidLoad
{
    [super viewDidLoad];
    
    self.view.backgroundColor = [UIColor darkGrayColor];
  
    
    self.sanFrameView = [[UIView alloc]initWithFrame:CGRectMake(0, 0, [UIScreen mainScreen].bounds.size.width - 40,  [UIScreen mainScreen].bounds.size.width - 40)];
    self.sanFrameView.center = CGPointMake(self.view.center.x, self.view.center.y);
    self.sanFrameView.backgroundColor = [UIColor whiteColor];
    
    
    UIImageView *imageView = [[UIImageView alloc]initWithFrame:CGRectMake(0, 0,[UIScreen mainScreen].bounds.size.width - 20 ,[UIScreen mainScreen].bounds.size.width - 20)];
    imageView.image = [UIImage imageNamed:@"pick_bg"];
    imageView.center = CGPointMake(self.view.center.x, self.view.center.y);
  
    
    anmationLine = [[UIImageView alloc] initWithFrame:CGRectMake(0, 10,[UIScreen mainScreen].bounds.size.width - 30,2)];
    anmationLine.image = [UIImage imageNamed:@"line"];
    anmationLine.center = CGPointMake(self.view.center.x, anmationLine.center.y);

    UIButton *canBtn = [UIButton buttonWithType:UIButtonTypeCustom];
    canBtn.frame = CGRectMake([UIScreen mainScreen].bounds.size.width-70, 70, 100, 50);
    [canBtn setTitle:@"Cancel" forState:UIControlStateNormal];
    [canBtn setTitleColor:[UIColor whiteColor] forState:UIControlStateNormal];
    [canBtn addTarget:self action:@selector(cancleAction) forControlEvents:UIControlEventTouchUpInside];
    canBtn.center = CGPointMake(self.view.center.x, self.view.center.y+self.sanFrameView.frame.size.height/2+30);
    
    [self.view addSubview:canBtn];
    [self.view addSubview:imageView];
    [imageView addSubview:anmationLine];
    [self.view addSubview:self.sanFrameView];
    [self startReading];
    [self.view bringSubviewToFront:imageView];
    [self startAnmaiton];
    
        _lastResut = YES;
}


// 取消扫描
- (void)cancleAction
{
    [self dismissViewControllerAnimated:YES completion:^{
    _lastResut = YES;
    [self stopReading];
    }];
}

- (void)startAnmaiton
{   //开始播送动画
    //扫描动画初始值
    upOrdown = NO;
    num =0;
    
    timer = [NSTimer scheduledTimerWithTimeInterval:.02 target:self selector:@selector(scanAnmation) userInfo:nil repeats:YES];
    
}
- (void)scanAnmation
{
    if (upOrdown == NO) {
        num ++;
        anmationLine.frame = CGRectMake(0, 10+2*num, CGRectGetWidth(anmationLine.frame), 2);
        if (2*num == 250) {
            upOrdown = YES;
        }
    }
    else {
        num --;
        anmationLine.frame = CGRectMake(0, 10+2*num, CGRectGetWidth(anmationLine.frame), 2);
        if (num == 0) {
            upOrdown = NO;
        }
    }
}

- (BOOL)startReading
{
    //    [_button setTitle:@"停止" forState:UIControlStateNormal];
    // 获取 AVCaptureDevice 实例
    NSError * error;
    AVCaptureDevice *captureDevice = [AVCaptureDevice defaultDeviceWithMediaType:AVMediaTypeVideo];
    // 初始化输入流
    AVCaptureDeviceInput *input = [AVCaptureDeviceInput deviceInputWithDevice:captureDevice error:&error];
    if (!input) {
        NSLog(@"%@", [error localizedDescription]);
        return NO;
    }
    // 创建会话
    _captureSession = [[AVCaptureSession alloc] init];
    // 添加输入流
    [_captureSession addInput:input];
    // 初始化输出流
    AVCaptureMetadataOutput *captureMetadataOutput = [[AVCaptureMetadataOutput alloc] init];
    // 添加输出流
    [_captureSession addOutput:captureMetadataOutput];
    
    // 创建dispatch queue.
    dispatch_queue_t dispatchQueue;
    dispatchQueue = dispatch_queue_create(kScanQRCodeQueueName, NULL);
    [captureMetadataOutput setMetadataObjectsDelegate:self queue:dispatchQueue];
    // 设置元数据类型 AVMetadataObjectTypeQRCode
    [captureMetadataOutput setMetadataObjectTypes:[NSArray arrayWithObject:AVMetadataObjectTypeQRCode]];
    
    // 创建输出对象
    _videoPreviewLayer = [[AVCaptureVideoPreviewLayer alloc] initWithSession:_captureSession];
    [_videoPreviewLayer setVideoGravity:AVLayerVideoGravityResizeAspectFill];
    [_videoPreviewLayer setFrame:_sanFrameView.layer.bounds];
    [_sanFrameView.layer addSublayer:_videoPreviewLayer];
    // 开始会话
    [_captureSession startRunning];
    
    return YES;
}

- (void)stopReading
{
    //    [_button setTitle:@"开始" forState:UIControlStateNormal];
    // 停止会话
    [_captureSession stopRunning];
    _captureSession = nil;
}

- (void)reportScanResult:(NSString *)result
{
    [self stopReading];
    if (!_lastResut) {
        return;
    }
    _lastResut = NO;
    // 以及处理了结果，下次扫描
    [self dismissViewControllerAnimated:YES completion:^{
        if ([self.delegate respondsToSelector:@selector(getUrlStr:)]) {
            [self.delegate getUrlStr:result];
        }
    }];
    
 
}


#pragma AVCaptureMetadataOutputObjectsDelegate

-(void)captureOutput:(AVCaptureOutput *)captureOutput didOutputMetadataObjects:(NSArray *)metadataObjects
      fromConnection:(AVCaptureConnection *)connection
{
    if (metadataObjects != nil && [metadataObjects count] > 0) {
        AVMetadataMachineReadableCodeObject *metadataObj = [metadataObjects objectAtIndex:0];
        NSString *result;
        if ([[metadataObj type] isEqualToString:AVMetadataObjectTypeQRCode]) {
            result = metadataObj.stringValue;
        } else {
            NSLog(@"不是二维码");
        }
        [self performSelectorOnMainThread:@selector(reportScanResult:) withObject:result waitUntilDone:NO];
    }
}


- (void)didReceiveMemoryWarning {
    [super didReceiveMemoryWarning];
    // Dispose of any resources that can be recreated.
}

- (void)dealloc
{
    [self stopReading];
}

@end
