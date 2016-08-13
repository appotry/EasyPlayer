//
//  SocketCommunication.m
//  UIOT-VideoPlayer
//
//  Created by unisiot on 16/6/3.
//  Copyright © 2016年 YinXX. All rights reserved.
//

#import "SocketCommunication.h"

@implementation SocketCommunication

static SocketCommunication *socketMessage = nil;

#pragma mark - SingleObject

+(SocketCommunication *)instance
{
    static dispatch_once_t onceToken;
    dispatch_once(&onceToken, ^{
        socketMessage = [[SocketCommunication alloc] init];
    });
    return socketMessage;
    
}

- (int) bulidSocketConnectServer{
    
    if (self.client == nil) {
        self.client = [[AsyncSocket alloc] initWithDelegate:self];
        NSError *err = nil;

        if (![self.client connectToHost:HOST_IP onPort:HOST_PORT error:&err]) {
            NSLog(@"%@ %@", [err code], [err localizedDescription]);
            
            UIAlertView *alert = [[UIAlertView alloc] initWithTitle:[@"Connection failed to host "
                                                                     stringByAppendingString:HOST_IP]
                                                            message:[[[NSString alloc]initWithFormat:@"%@",[err code]] stringByAppendingString:[err localizedDescription]]
                                                           delegate:self
                                                  cancelButtonTitle:@"OK"
                                                  otherButtonTitles:nil];
            [alert show];
            //client = nil;
            return SRV_CONNECT_FAIL;
        } else {
            NSLog(@"Conectou!");
            return SRV_CONNECT_SUC;
        }
    }
    else {
        [self.client readDataWithTimeout:-1 tag:0];
        return SRV_CONNECTED;
    }
    
}

#pragma mark socket delegate

- (void)onSocket:(AsyncSocket *)sock didConnectToHost:(NSString *)host port:(UInt16)port{
    [self.client readDataWithTimeout:-1 tag:0];
    NSLog(@"%@",[NSString stringWithFormat:@"socket连接成功,地址：%@，端口：%d",host,port]);
    [[NSNotificationCenter defaultCenter]postNotificationName:NOTICE_SOCKETCONNECT object:[NSNumber numberWithBool:YES]];
}

- (void)onSocket:(AsyncSocket *)sock willDisconnectWithError:(NSError *)err
{
    NSLog(@"Error");
}

- (void)onSocketDidDisconnect:(AsyncSocket *)sock
{
    NSString *msg = @"Sorry this connect is failure";
    [self showMessage:msg];
    self.client = nil;
}

- (void)onSocketDidSecure:(AsyncSocket *)sock{
    
}

- (void)onSocket:(AsyncSocket *)sock didReadData:(NSData *)data withTag:(long)tag{
    
    NSString* aStr = [[NSString alloc] initWithData:data encoding:NSUTF8StringEncoding];
    NSLog(@"Hava received datas is :%@",aStr);
    [self.client readDataWithTimeout:-1 tag:0];
}

//写入数据
-(void)socketWriteDate:(NSData *)writeDate
{
    [self.client writeData:writeDate withTimeout:-1 tag:100];
    [self.client readDataWithTimeout:-1 tag:100];
}

- (void) showMessage:(NSString *) msg{
    UIAlertView * alert = [[UIAlertView alloc]initWithTitle:@"Alert!"
                                                    message:msg
                                                   delegate:nil
                                          cancelButtonTitle:@"OK"
                                          otherButtonTitles:nil];
    [alert show];
}

@end
