//
//  EasyNVRCell.h
//  EasyDarwinPlayer
//
//  Created by Amber on 16/8/1.
//  Copyright © 2016年 Amber. All rights reserved.
//

#import <UIKit/UIKit.h>
@class EasyCamera;

@interface EasyNVRCell : UICollectionViewCell
@property(strong,nonatomic)UIImageView *indexImgView;
@property(strong,nonatomic)UILabel *titleLab;
- (void)produceCellModel:(EasyCamera *)model;
@end
