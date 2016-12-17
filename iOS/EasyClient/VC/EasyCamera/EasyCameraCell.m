#import "EasyCameraCell.h"
#import "EasyCamera.h"
#import "UIImageView+WebCache.h"
#import "EasyInfo.h"

@implementation EasyCameraCell


- (id)initWithFrame:(CGRect)frame
{
    self = [super initWithFrame:frame];
    if (self) {
        CGRect imgRect =CGRectMake(0, 0, self.contentView.frame.size.width, self.contentView.frame.size.height - 30);
        self.imageView = [[UIImageView alloc]initWithFrame:imgRect];
        self.imageView.contentMode = UIViewContentModeScaleToFill;
        [self.contentView addSubview: self.imageView];
        self.titleLab = [[UILabel alloc]initWithFrame:CGRectMake(0,self.contentView.frame.size.height-30, self.contentView.frame.size.width, 30)];
        self.titleLab.font = [UIFont systemFontOfSize:10];
        self.titleLab.textAlignment = NSTextAlignmentLeft;
        self.titleLab.backgroundColor = COLOR(222,222,222,1);
        self.titleLab.textColor = [UIColor darkGrayColor];
        self.titleLab.alpha= 1;
        [self bringSubviewToFront:self.titleLab];
        [self.contentView addSubview:self.titleLab];
        
        self.appType = [[UILabel alloc]initWithFrame:CGRectMake(0,self.contentView.frame.size.height-52, self.contentView.frame.size.width, 20)];
        self.appType.textColor =[UIColor whiteColor];
        self.appType.font = [UIFont systemFontOfSize:10];
        [self.appType setTextAlignment:NSTextAlignmentCenter];
        self.appType.backgroundColor = COLOR(222,222,222,0.5);
        self.appType.alpha = 0.8;
        [self.contentView addSubview:self.appType];
        [self bringSubviewToFront:self.appType];
        
        
    }
    return self;
}

- (void)produceCellModel:(EasyCamera *)model
{
    [ self.imageView sd_setImageWithURL:[NSURL URLWithString:model.snapURL] placeholderImage:[UIImage imageNamed:@"snap"]];
    self.titleLab.text = [NSString stringWithFormat:@" %@ (%@)",model.name,model.serial];
    self.appType.text = model.terminalType;
    [self.appType sizeToFit];
    [self.appType setFrame:CGRectMake(self.contentView.frame.size.width - self.appType.frame.size.width - 7, self.appType.frame.origin.y, self.appType.frame.size.width + 5, 20)];
    
}


- (void)produceCellInfoModel:(EasyInfo *)model
{
    [self.imageView sd_setImageWithURL:[NSURL URLWithString:model.snapURL] placeholderImage:[UIImage imageNamed:@"snap"]];
    self.titleLab.text = [NSString stringWithFormat:@" %@ (%@)",model.name,model.serial];
}

@end
