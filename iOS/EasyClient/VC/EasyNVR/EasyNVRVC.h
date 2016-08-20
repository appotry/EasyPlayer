#import <UIKit/UIKit.h>

@interface EasyNVRVC : UIViewController<UICollectionViewDelegate,UICollectionViewDataSource,UICollectionViewDelegateFlowLayout>
@property(nonatomic,strong)UICollectionView *collectionView;
@property(nonatomic,strong)NSString* cmsIp;
@property(nonatomic,strong)NSString* cmsPort;
- (void)requestListData;
@end
