#import <UIKit/UIKit.h>

@interface EasyNVRVC : UIViewController<UICollectionViewDelegate,UICollectionViewDataSource,UICollectionViewDelegateFlowLayout>
@property(nonatomic,strong)UICollectionView *collectionView;
@property(nonatomic , copy)NSString *urlStr;
- (void)requestListData;
@end
