//
//  DES.m
//  ModelDemo
//
//  Created by Tr Li on 11-11-3.
//  Copyright (c) 2011年 imohoo. All rights reserved.
//

#import "DESDecrypt.h" 

//解密开关 需要解密True 1     不需要解密False 0
#define IS_NEED_SECRET 1

//加密开关 需要加密True 1     不需要加密False 0
#define IS_NEED_ENSECRET 1
//3des秘钥
#define SECRET_KEY @"3b38e11ffd65698aedeb5ffc"
#define DES_VECTOR          @"4b5e6992"

//判断是否需要解密
BOOL isDecode = IS_NEED_SECRET;
//判断是否需要加密
BOOL isEnCode = IS_NEED_ENSECRET;

@implementation DESDecrypt

+ (NSString*)TripleDES:(NSString*)plainText encryptOrDecrypt:(CCOperation)encryptOrDecrypt key:(NSString*)key {
    
    
    const void *vplainText;
    size_t plainTextBufferSize;
    
    if (encryptOrDecrypt == kCCDecrypt)
    {
        NSData *EncryptData = [GTMBase64 decodeData:[plainText dataUsingEncoding:NSUTF8StringEncoding]];
        plainTextBufferSize = [EncryptData length];
        vplainText = [EncryptData bytes];
    }
    else
    {
        NSData* data = [plainText dataUsingEncoding:NSUTF8StringEncoding];
        plainTextBufferSize = [data length];
        vplainText = (const void *)[data bytes];
    }
    
//    CCCryptorStatus ccStatus;
    uint8_t *bufferPtr = NULL;
    size_t bufferPtrSize = 0;
    size_t movedBytes = 0;
    // uint8_t ivkCCBlockSize3DES;
    
    bufferPtrSize = (plainTextBufferSize + kCCBlockSize3DES) & ~(kCCBlockSize3DES - 1);
    bufferPtr = malloc( bufferPtrSize * sizeof(uint8_t));
    memset((void *)bufferPtr, 0x0, bufferPtrSize);
    // memset((void *) iv, 0x0, (size_t) sizeof(iv));
    
    //    NSString *key = @"123456789012345678901234";
    NSString *initVec = DES_VECTOR;
    const void *vkey = (const void *) [key UTF8String];
    const void *vinitVec = (const void *) [initVec UTF8String];
    
    
//    Byte KEY[16] = {0x26, 0x7a, 0xc2, 0xed, 0x67, 0xf2, 0x92, 0xff, 0x77, 0xc4, 0xa0, 0xb8, 0x32, 0xbb, 0xc7, 0xf5};
//    Byte IV[8] = {0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08};
//    const void *vvk = (const void *)KEY;
//    const void *vvi = (const void *)IV;
    
    CCCrypt(encryptOrDecrypt,
                       kCCAlgorithm3DES,
                       kCCOptionPKCS7Padding,
                       vkey, //"123456789012345678901234", //key
                       kCCKeySize3DES,
                       vinitVec, //"init Vec", //iv,
                       vplainText, //"Your Name", //plainText,
                       plainTextBufferSize,
                       (void *)bufferPtr,
                       bufferPtrSize,
                       &movedBytes); 
     
    
    NSString *result;
    
    if (encryptOrDecrypt == kCCDecrypt)
    {
        result = [[[NSString alloc] initWithData:[NSData dataWithBytes:(const void *)bufferPtr 
                                                                length:(NSUInteger)movedBytes] 
                                        encoding:NSUTF8StringEncoding] 
                  autorelease];
    }
    else
    {
        NSData *myData = [NSData dataWithBytes:(const void *)bufferPtr length:(NSUInteger)movedBytes];
        result = [GTMBase64 stringByEncodingData:myData];
    }
    
    free(bufferPtr);
    
    return result;
    
} 

//对3des进行加密
+ (NSString *)encryptString:(NSString *)string {
    return [DESDecrypt TripleDES:string encryptOrDecrypt:kCCEncrypt key:SECRET_KEY]; 
}

//对3des进行加密
+ (NSString *)encryptUrlString:(NSString *)string {
    if(isEnCode){
        return [DESDecrypt TripleDES:string encryptOrDecrypt:kCCEncrypt key:SECRET_KEY];
    }
    return string;
}

//对3des进行解密
+ (NSString *)decryptString:(NSString *)string { 
    if (isDecode) {
        return [DESDecrypt TripleDES:string encryptOrDecrypt:kCCDecrypt key:SECRET_KEY];
    }
    return string;
}

//对3des进行解密（一定会解密）
+ (NSString *)decryptString1:(NSString *)string { 
    return [DESDecrypt TripleDES:string encryptOrDecrypt:kCCDecrypt key:SECRET_KEY];
}
@end
