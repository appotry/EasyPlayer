#import <UIKit/UIKit.h>

@interface EasyNVRVC : UIViewController<UICollectionViewDelegate,UICollectionViewDataSource,UICollectionViewDelegateFlowLayout>
@property(nonatomic,strong)UICollectionView *collectionView;
- (void)requestListData;
@end
