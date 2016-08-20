#import "EasyNVRCell.h"
#import "EasyCamera.h"
@implementation EasyNVRCell
- (id)initWithFrame:(CGRect)frame
{
    self = [super initWithFrame:frame];
    if (self) {
        
        self.indexImgView = [[UIImageView alloc]initWithFrame:CGRectZero];
        [self.contentView addSubview:self.indexImgView];
        self.indexImgView.frame = CGRectMake(0, 0, 30, 30);
        
        self.titleLab = [[UILabel alloc]initWithFrame:CGRectMake(30, 0, ScreenWidth-10, 30)];
          self.titleLab.font = [UIFont systemFontOfSize:12];
        [self.contentView addSubview:self.titleLab];
    }
    return self;
}

- (void)produceCellModel:(EasyCamera *)model
{
    if (model.isOpen) {
        self.indexImgView.image = [UIImage imageNamed:@"slectImg"];
    }else{
         self.indexImgView.image = [UIImage imageNamed:@"noSelectImg"];
    }
    self.titleLab.text = [NSString stringWithFormat:@"%@:%@:%@",model.appType,model.name,model.serial];

}
@end
