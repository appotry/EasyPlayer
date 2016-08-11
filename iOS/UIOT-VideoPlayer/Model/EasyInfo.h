//
//  EasyInfo.h
//  UIOT-VideoPlayer
//
//  Created by Amber on 16/8/3.
//  Copyright © 2016年 YinXX. All rights reserved.
//

#import <Foundation/Foundation.h>

@interface EasyInfo : NSObject
@property(nonatomic,strong)NSString *channel;
@property(nonatomic,strong)NSString *name;
@property(nonatomic,strong)NSString *snapURL;
@property(nonatomic,strong)NSString *status;
@property(nonatomic,strong)NSNumber *serial;
@property(nonatomic,assign)BOOL isOpen;
@end
