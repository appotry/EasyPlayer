//
//  SocketCommunication.h
//  UIOT-VideoPlayer
//
//  Created by unisiot on 16/6/3.
//  Copyright © 2016年 YinXX. All rights reserved.
//

#import <Foundation/Foundation.h>
#import "AsyncSocket.h"
#define SRV_CONNECTED 0
#define SRV_CONNECT_SUC 1
#define SRV_CONNECT_FAIL 2
#define HOST_IP  @"116.255.207.67" //@"115.29.139.20"
#define HOST_PORT 10000

@interface SocketCommunication : NSObject
@property (nonatomic, strong) AsyncSocket *client;

+(SocketCommunication *)instance;
//建立连接
- (int) bulidSocketConnectServer;
//写入数据
-(void)socketWriteDate:(NSData *)writeDate;
@end
