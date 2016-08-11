//
//  PhotoVideoBrowerVC.h
//  MJPhotoBrowser
//
//  Created by UIOT-Amber on 16/7/23.
//  Copyright © 2016年 Sunnyyoung. All rights reserved.
//


#import "EasyCameraCell.h"
#import "EasyCamera.h"
#import "UIImageView+AFNetworking.h"
#import "EasyInfo.h"

@implementation EasyCameraCell


- (id)initWithFrame:(CGRect)frame
{
    self = [super initWithFrame:frame];
    if (self) {
        self.imageView = [[UIImageView alloc]initWithFrame:CGRectZero];
        [self.contentView addSubview: self.imageView];
        self.imageView.frame = CGRectMake(0, 0,(ScreenWidth-10)/2,(ScreenWidth-10)/2);
    
        self.titleLab = [[UILabel alloc]initWithFrame:CGRectMake(0,(ScreenWidth-10)/2-30, (ScreenWidth-10)/2, 30)];
        self.titleLab.font = [UIFont systemFontOfSize:10];
        self.titleLab.textAlignment = NSTextAlignmentLeft;
        self.titleLab.backgroundColor = [UIColor darkTextColor];
        self.titleLab.textColor = [UIColor whiteColor];
        self.titleLab.alpha= 0.5;
        [self bringSubviewToFront:self.titleLab];
        [self.contentView addSubview:self.titleLab];
        
        self.appType = [[UILabel alloc]initWithFrame:CGRectMake((ScreenWidth-10)/2-100,(ScreenWidth-10)/2-50, 100, 20)];
        self.appType.textColor =[UIColor whiteColor];
//         self.appType.font = [UIFont fontWithName:@"Helvetica-Bold"size:10];
          self.appType.font = [UIFont systemFontOfSize:10];
        self.appType.textAlignment = NSTextAlignmentRight;
        [self.contentView addSubview:self.appType];

    }
    return self;
}

- (void)produceCellModel:(EasyCamera *)model
{
    [ self.imageView setImageWithURL:[NSURL URLWithString:model.snapURL] placeholderImage:[UIImage imageNamed:@"slectImg"]];
    self.titleLab.text = [NSString stringWithFormat:@" %@ %@",model.name,model.serial];
    self.appType.text = model.appType;
    
}
- (void)produceCellInfoModel:(EasyInfo *)model
{
    [ self.imageView setImageWithURL:[NSURL URLWithString:model.snapURL] placeholderImage:[UIImage imageNamed:@"slectImg"]];
    self.titleLab.text = [NSString stringWithFormat:@" %@（%@）",model.name,model.status];
}
/*
// Only override drawRect: if you perform custom drawing.
// An empty implementation adversely affects performance during animation.
- (void)drawRect:(CGRect)rect
{
    // Drawing code
}
*/

@end
