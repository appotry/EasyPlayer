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

-(void)postRequest:(NSString*)sessionStr httpBody:(NSString *)sender
{

//    NSLog(@"body:%@",sender);
//    id<DataSigner> signer = CreateRSADataSigner(VIDO_RSAKay);
//    //NSString *signedString = [signer signString:@"123"];
//    NSString * signStr = [signer signMyStrWithPrivateKey:sender];
//    
//    //对请求路径的说明
//    //http://120.25.226.186:32812/login
//    //协议头+主机地址+接口名称
//    //协议头(http://)+主机地址(120.25.226.186:32812)+接口名称(login)
//    //POST请求需要修改请求方法为POST，并把参数转换为二进制数据设置为请求体
//    
//    //1.创建会话对象 sessionStr
//    NSURLSession *session = [NSURLSession sharedSession];
//    //2.根据会话对象创建task @"http://116.255.207.67:10000/clientinfo?sign=1
//    NSURL *url = [NSURL URLWithString:[NSString stringWithFormat:@"http://%@:%@/clientinfo?sign=%@&sessionid=%@",LOGIN_CMS_ADDRESS,LOGIN_CMS_ADDRESS_PORT,signStr,sessionStr]];
//    
//    //3.创建可变的请求对象
//    NSMutableURLRequest *request = [NSMutableURLRequest requestWithURL:url];
//    [request setTimeoutInterval:3.0];
//    //4.修改请求方法为POST
//    request.HTTPMethod = @"POST";
//    
////    NSString *asq = @"{\"EasyDarwin\":{\"Header\":{\"Version\":\"1.0\",\"CSeq\":\"6\",\"MessageType\":\"MSG_CS_LOGIN_REQ\"},\"Body\":{\"PhoneNumber\":\"admin\",\"Password\":\"admin\",\"SN\":\"6c53f7de30af4dcc8af8d1b1b1f190cb\",\"OemFirm\":\"0X8001\"}}}";
//    //5.设置请求体
//    char encryStr[1024*100] = {0};
//    
//    
//    char *needEncrypt = (char *)[sender UTF8String];
//    sprintf(encryStr,"%s",needEncrypt);
////
////    //加密的key需要读取
//    char KEY[16]={0x3B,0xCA,0x85,0x31,0xCB,0xB2,0xEB,0xB5,0xA1,0x14,0x43,0x31,0x35,0xB6,0xEA,0xD4};
//    int  length=strlen(encryStr);
////    NSLog(@"%d",length);
////    
//    tea_encrypt_fun(encryStr, length, KEY);
////
//    
////    int length = [DataEncrypt stringNeedEncrypt:sender charArray:encryStr];
//    NSData *sendData = [[NSData alloc]initWithBytes:(Byte *)encryStr length:length];
//
//    request.HTTPBody = sendData;//[sender dataUsingEncoding:NSUTF8StringEncoding];
//    //    [request setValue:@"" forHTTPHeaderField:@""];
//    //6.根据会话对象创建一个Task(发送请求）
//    /*
//     第一个参数：请求对象
//     第二个参数：completionHandler回调（请求完成【成功|失败】的回调）
//     data：响应体信息（期望的数据）
//     response：响应头信息，主要是对服务器端的描述
//     error：错误信息，如果请求失败，则error有值
//     */
//    
//    
//    
//    
//    NSURLSessionDataTask *dataTask = [session dataTaskWithRequest:request completionHandler:^(NSData * _Nullable data, NSURLResponse * _Nullable response, NSError * _Nullable error) {
//        
//        //8.解密解析数据
//        
////        char KEY[16]={0x3A,0xDA,0x75,0x21,0xDB,0xE2,0xDB,0xB3,0x11,0xB4,0x49,0x01,0xA5,0xC6,0xEA,0xD4};
//        char *getChar = (char *)[data bytes];
//        char *uncompressStr = (char *)malloc(1024*1000);
//        char KEY[16]={0x3B,0xCA,0x85,0x31,0xCB,0xB2,0xEB,0xB5,0xA1,0x14,0x43,0x31,0x35,0xB6,0xEA,0xD4};
//        memcpy(uncompressStr, getChar, data.length);
//        tea_decrypt_fun(uncompressStr, data.length, KEY);
//        
//       NSString  *jsonString =[[NSString alloc]initWithCString:uncompressStr encoding:NSUTF8StringEncoding];
//        
//     
//        
//        NSData *jsonData = [jsonString dataUsingEncoding:NSUTF8StringEncoding];
//        NSError *err;
//        NSDictionary *dataDic = [NSJSONSerialization JSONObjectWithData:jsonData
//                                                            options:NSJSONReadingMutableContainers
//                                                              error:&err];
//        if(err) {
//            NSLog(@"json解析失败：%@",err);
//            // 请求超时
//            [[NSNotificationCenter defaultCenter]postNotificationName:NOTICE_HTTPREQUEST_TIMEOUT object:nil];
//            return;
//        }
//        
//        
//        NSLog(@"json解析成功：%@",dataDic);
//        if ([self.delegate respondsToSelector:@selector(receiveData:)]) {
//        
//            [self.delegate receiveData:dataDic];
//        }
//        
//      
//    }];
//    
//    //7.执行任务
//    [dataTask resume];
    
}

#pragma mark --------------------
#pragma mark NSURLSessionDataDelegate
//1.接收到服务器响应的时候调用该方法
-(void)URLSession:(NSURLSession *)session dataTask:(NSURLSessionDataTask *)dataTask didReceiveResponse:(NSURLResponse *)response completionHandler:(void (^)(NSURLSessionResponseDisposition))completionHandler
{
    //在该方法中可以得到响应头信息，即response
    NSLog(@"response=\n%@\ncompletionHandler%@",response,completionHandler);
    
    //注意：需要使用completionHandler回调告诉系统应该如何处理服务器返回的数据
    //默认是取消的
    /*
     NSURLSessionResponseCancel = 0,        默认的处理方式，取消
     NSURLSessionResponseAllow = 1,         接收服务器返回的数据
     NSURLSessionResponseBecomeDownload = 2,变成一个下载请求
     NSURLSessionResponseBecomeStream        变成一个流
     */
    
    completionHandler(NSURLSessionResponseAllow);
}

//2.接收到服务器返回数据的时候会调用该方法，如果数据较大那么该方法可能会调用多次
-(void)URLSession:(NSURLSession *)session dataTask:(NSURLSessionDataTask *)dataTask didReceiveData:(NSData *)data
{
    //拼接服务器返回的数据
    [self.responseData appendData:data];
}

//3.当请求完成(成功|失败)的时候会调用该方法，如果请求失败，则error有值
-(void)URLSession:(NSURLSession *)session task:(NSURLSessionTask *)task didCompleteWithError:(NSError *)error
{
    NSLog(@"didCompleteWithError--%@",[NSThread currentThread]);
    
    if(error == nil)
    {
        //解析数据,JSON解析请参考http://www.cnblogs.com/wendingding/p/3815303.html
        NSDictionary *dict = [NSJSONSerialization JSONObjectWithData:self.responseData options:kNilOptions error:nil];
        NSLog(@"dict===%@",dict);
    }
}


// 请求数据
- (void)requestListData:(NSString *)urlStr
{
    AFHTTPSessionManager * manager = [AFHTTPSessionManager manager];
    manager.responseSerializer.acceptableContentTypes = [NSSet setWithObjects:@"html/text",@"text/plain", nil];
      manager.requestSerializer.timeoutInterval = 8;
    
    
    [manager POST:urlStr parameters:nil progress:^(NSProgress * _Nonnull uploadProgress) {
        
    } success:^(NSURLSessionDataTask * _Nonnull task, id  _Nullable responseObject) {
        
        NSLog(@"responseObject===%@",responseObject);
        NSLog(@"====%@",[responseObject objectForKey:@"EasyDarwin"]);
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
    }];
}

// 请求控制摄像头数据
- (void)requestControlCamera:(NSString *)urlStr
{
    AFHTTPSessionManager * manager = [AFHTTPSessionManager manager];
    manager.responseSerializer.acceptableContentTypes = [NSSet setWithObjects:@"html/text",@"text/plain", nil];
    
    
    [manager GET:urlStr parameters:nil progress:^(NSProgress * _Nonnull downloadProgress) {
        
    } success:^(NSURLSessionDataTask * _Nonnull task, id  _Nullable responseObject) {
          NSLog(@"responseObject===%@",responseObject);
    } failure:^(NSURLSessionDataTask * _Nullable task, NSError * _Nonnull error) {
         NSLog(@"error====%@",error);
    }];
    
    
}
@end
