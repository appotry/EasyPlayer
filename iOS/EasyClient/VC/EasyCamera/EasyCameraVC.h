#import <UIKit/UIKit.h>

@interface EasyCameraVC : UIViewController<UICollectionViewDelegate,UICollectionViewDataSource,UICollectionViewDelegateFlowLayout,NetRequestDelegate>
@property(nonatomic,strong)UICollectionView *collectionView;
@property(nonatomic,strong)NSArray *dataArray;
@property(nonatomic , copy)NSString *urlStr;
- (void)requestListData;
@end
