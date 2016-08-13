//
//  EasyCamera.h
//  TempTest
//
//  Created by UIOT-Amber on 16/8/1.
//  Copyright © 2016年 YinXX. All rights reserved.
//

#import <Foundation/Foundation.h>

@interface EasyCamera : NSObject

@property(nonatomic,strong)NSString *appType;
@property(nonatomic,strong)NSString *snapURL;
@property(nonatomic,strong)NSString *terminalType;
@property(nonatomic,strong)NSString *name;
@property(nonatomic,strong)NSString *tag;
@property(nonatomic,strong)NSNumber *serial;
@property(nonatomic,assign)BOOL isOpen;
@property(nonatomic,strong)NSMutableArray *deviceArr;

@end
