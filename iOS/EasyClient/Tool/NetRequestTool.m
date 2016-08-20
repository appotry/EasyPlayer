//
//  NetRequestTool.m
//  EasyDarwinPlayer
//
//  Created by Amber on 16/8/1.
//  Copyright © 2016年 Amber. All rights reserved.
//

#import "NetRequestTool.h"
#import "AFNetworking.h"
#import "EasyCamera.h"


@implementation NetRequestTool

-(NSMutableData *)responseData
{
    if (_responseData == nil) {
        _responseData = [NSMutableData data];
    }
    return _responseData;
}

// 请求数据
- (void)requestListData:(NSString *)urlStr
{
    AFHTTPSessionManager * manager = [AFHTTPSessionManager manager];
    manager.responseSerializer.acceptableContentTypes = [NSSet setWithObjects:@"text/html",@"text/plain", nil];
    manager.requestSerializer.timeoutInterval = 8;
    
    [manager POST:urlStr parameters:nil success:^(NSURLSessionDataTask * _Nonnull task, id  _Nullable responseObject) {
        NSDictionary *easyDic = [responseObject objectForKey:@"EasyDarwin"];
        NSDictionary *headerDic =  [easyDic objectForKey:@"Header"];
        NSDictionary *bodyDic =  [easyDic objectForKey:@"Body"];
        if ([[headerDic objectForKey:@"ErrorNum"] isEqualToString:@"200"]) {
            NSArray *devices = [bodyDic objectForKey:@"Devices"] ;
            NSMutableArray *dataArr = [[NSMutableArray alloc]init];
                for (NSDictionary *deviceDic in devices) {
                    
                    EasyCamera *model = [[EasyCamera alloc]init];
                    model.appType = [deviceDic objectForKey:@"AppType"];
                    model.name = [deviceDic objectForKey:@"Name"];
                    model.serial = (NSNumber *)[deviceDic objectForKey:@"Serial"];
                    model.snapURL = [deviceDic objectForKey:@"SnapURL"];
                    model.tag = [deviceDic objectForKey:@"Tag"];
                    model.terminalType = [deviceDic objectForKey:@"TerminalType"];
                    [dataArr addObject:model];
            }
            if ([self.delegate respondsToSelector:@selector(receiveData:)]) {
                [self.delegate receiveData:dataArr];
            }
        }
        
    } failure:^(NSURLSessionDataTask * _Nullable task, NSError * _Nonnull error) {
        NSLog(@"error====%@",error);
        if ([self.delegate respondsToSelector:@selector(receiveData:)]) {
            [self.delegate receiveData:[[NSMutableArray alloc]init]];
        }
    }];
}

// 请求控制摄像头数据
- (void)requestControlCamera:(NSString *)urlStr
{
    AFHTTPSessionManager * manager = [AFHTTPSessionManager manager];
    manager.responseSerializer.acceptableContentTypes = [NSSet setWithObjects:@"text/html",@"text/plain", nil];
    [manager GET:urlStr parameters:nil success:^(NSURLSessionDataTask * _Nonnull task, id  _Nullable responseObject) {
    } failure:^(NSURLSessionDataTask * _Nullable task, NSError * _Nonnull error) {
         NSLog(@"error====%@",error);
    }];
    
    
}
@end
