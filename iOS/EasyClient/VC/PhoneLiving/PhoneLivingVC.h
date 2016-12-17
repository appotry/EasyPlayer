#import <UIKit/UIKit.h>

@interface PhoneLivingVC : UIViewController<UICollectionViewDelegate,UICollectionViewDataSource,UICollectionViewDelegateFlowLayout>
@property(nonatomic,strong)UICollectionView *collectionView;
@property(nonatomic,strong)NSArray *dataArray;
@property(nonatomic , copy)NSString *urlStr;
- (void)requestListData;
@end
