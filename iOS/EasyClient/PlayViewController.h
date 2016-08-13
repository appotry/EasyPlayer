//
//  ViewController.h
//  H264DecodeDemo
//
//  Created by Yao Dong on 15/8/6.
//  Copyright (c) 2015年 duowan. All rights reserved.
//

#import <UIKit/UIKit.h>
@class CallBySocket;
@class EasyUrl;
@interface PlayViewController : UIViewController
@property (nonatomic,strong)UIButton   *doneBtn,*picBtn,*tempBtn,*midBtn;
@property(nonatomic,strong)EasyUrl *urlModel;
@property(strong,nonatomic)CallBySocket *callSocket;
- (void) play;
//- (void) pause;
//关闭通话
- (void)disconnect;
//提示框
-(void)hudNotice:(NSString *)message;
@end

