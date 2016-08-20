#import <UIKit/UIKit.h>
@class EasyCamera;
@class EasyInfo;

@interface EasyCameraCell : UICollectionViewCell
@property(nonatomic,strong)UIImageView *imageView;
@property(nonatomic,strong)UILabel *titleLab,*appType;

- (void)produceCellModel:(EasyCamera *)model;

- (void)produceCellInfoModel:(EasyInfo *)model;
@end
