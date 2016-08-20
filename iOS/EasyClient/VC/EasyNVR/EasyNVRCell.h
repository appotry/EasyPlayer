#import <UIKit/UIKit.h>
@class EasyCamera;

@interface EasyNVRCell : UICollectionViewCell
@property(strong,nonatomic)UIImageView *indexImgView;
@property(strong,nonatomic)UILabel *titleLab;
- (void)produceCellModel:(EasyCamera *)model;
@end
