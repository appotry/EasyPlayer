#import "EasyCameraVC.h"
#import "EasyCameraCell.h"
#import "EasyCamera.h"
#import "EasyUrl.h"


@interface EasyCameraVC()
{

    NSMutableArray *_dataArr;
     UiotHUD *_HUD;
    NSString *_urlStr;
    EasyUrl *_urlModel;
}
@property(nonatomic,strong) NetRequestTool *requestTool;
@end

@implementation EasyCameraVC

static NSString *cellIdentifier1 = @"Cell1";

- (void)viewDidLoad {
    [super viewDidLoad];
    
    self.view.backgroundColor = [UIColor whiteColor];
    [self initSomeView];
    _dataArr = [NSMutableArray array];
    self.requestTool = [[NetRequestTool alloc]init];
    self.requestTool.delegate = self;
//    NSString *cmsIp = [[NSUserDefaults standardUserDefaults] stringForKey:@"cms_ip"];
//    NSString *cmsPort = [[NSUserDefaults standardUserDefaults] stringForKey:@"cms_port"];
//    _urlStr = [NSString stringWithFormat:@"http://%@:%@/api/getdevicelist?AppType=EasyCamera&TerminalType=ARM_Linux",cmsIp, cmsPort];
}


- (void)initSomeView
{
    self.view.backgroundColor = [UIColor whiteColor];
    UICollectionViewFlowLayout *layout = [[UICollectionViewFlowLayout alloc]init];
    //定义每个UICollectionView 横向的间距
    layout.minimumLineSpacing = 5;
    //定义每个UICollectionView 纵向的间距
    layout.minimumInteritemSpacing = 5;
    //定义每个UICollectionView 的边距距
    layout.sectionInset = UIEdgeInsetsMake(5, 5, 5, 5);//上左下右
    self.collectionView = [[UICollectionView alloc]initWithFrame:CGRectMake(0, 0, ScreenWidth, ScreenHeight) collectionViewLayout:layout];
    self.collectionView.delegate = self;
    self.collectionView.dataSource = self;
    self.collectionView.showsHorizontalScrollIndicator = NO;
    self.collectionView.showsVerticalScrollIndicator = NO;
    self.collectionView.backgroundColor = [UIColor whiteColor];
    
    
    [self.collectionView registerClass:[EasyCameraCell class] forCellWithReuseIdentifier:cellIdentifier1];
    
    [self.view addSubview:self.collectionView];
    
     __weak typeof(self) weakSelf = self;
    //1. 添加头部控件的方法
    self.collectionView.mj_header = [MJRefreshNormalHeader headerWithRefreshingBlock:^{
        [_HUD show:YES];
        [weakSelf requestListData];
    }];
 
}

- (void)receiveData:(NSMutableArray *)sender
{
    [_dataArr removeAllObjects];
    _dataArr = sender;
      [self.collectionView.mj_header endRefreshing];
     [_HUD hide:YES afterDelay:0.5];
     [self.collectionView reloadData];
}
// 请求数据
- (void)requestListData
{
    [self.requestTool  requestListData:_urlStr];
}


// 返回Items的个数 （图片的个数）
-(NSInteger)collectionView:(UICollectionView *)collectionView numberOfItemsInSection:(NSInteger)section
{
    return _dataArr.count;//model.fileArray.count;
}

// item大小
-(CGSize)collectionView:(UICollectionView *)collectionView layout:(UICollectionViewLayout *)collectionViewLayout sizeForItemAtIndexPath:(NSIndexPath *)indexPath
{
    int w = (ScreenWidth - 16) /2;
    int h = w * 9 /16;
    return CGSizeMake(w, h + 30);//30 is the bottom title height
}

-(UIEdgeInsets)collectionView:(UICollectionView *)collectionView layout:(UICollectionViewLayout *)collectionViewLayout insetForSectionAtIndex:(NSInteger)section
{
    return UIEdgeInsetsMake(5, 5, 5, 5);
}

-(UICollectionViewCell *)collectionView:(UICollectionView *)collectionView cellForItemAtIndexPath:(NSIndexPath *)indexPath
{
    EasyCameraCell * cell = [collectionView dequeueReusableCellWithReuseIdentifier:cellIdentifier1 forIndexPath:indexPath];
    
    EasyCamera *model = _dataArr[indexPath.row];
    
    [cell produceCellModel:model];
    
    cell.backgroundColor = [UIColor orangeColor];
    
    return cell;
}

-(void)collectionView:(UICollectionView *)collectionView didSelectItemAtIndexPath:(NSIndexPath *)indexPath
{
     EasyCamera *model = _dataArr[indexPath.row];
    [self requestUrlData:model.serial];
}

// 请求数据
- (void)requestUrlData:(NSString *)deviceStr
{
    NSString *cmsIp = [[NSUserDefaults standardUserDefaults] stringForKey:@"cms_ip"];
    NSString *cmsPort = [[NSUserDefaults standardUserDefaults] stringForKey:@"cms_port"];
    
    AFHTTPSessionManager * manager = [AFHTTPSessionManager manager];
    manager.responseSerializer.acceptableContentTypes = [NSSet setWithObjects:@"html/text",@"text/plain", nil];
    NSString *urlStr =[NSString stringWithFormat:@"http://%@:%@/api/getdevicestream?device=%@&channel=0&protocol=RTSP&reserve=1",cmsIp,cmsPort, deviceStr];
    
    [manager POST:urlStr parameters:nil success:^(NSURLSessionDataTask * _Nonnull task, id  _Nullable responseObject) {
        NSDictionary *easyDic = [responseObject objectForKey:@"EasyDarwin"];
        NSDictionary *headerDic =  [easyDic objectForKey:@"Header"];
        NSDictionary *bodyDic =  [easyDic objectForKey:@"Body"];
        if ([[headerDic objectForKey:@"ErrorNum"] isEqualToString:@"200"]) {
                _urlModel = [[EasyUrl alloc]init];
                _urlModel.channel = [bodyDic objectForKey:@"Channel"];
                _urlModel.protocol = [bodyDic objectForKey:@"Protocol"];
                _urlModel.reserve = [bodyDic objectForKey:@"Reserve"];
                _urlModel.serial = [bodyDic objectForKey:@"Serial"];
                _urlModel.url = [bodyDic objectForKey:@"URL"];
                PlayViewController *playVC = [[PlayViewController alloc]init];
                playVC.urlModel = _urlModel;
                [self presentViewController:playVC animated:YES completion:nil];
        }
    } failure:^(NSURLSessionDataTask * _Nullable task, NSError * _Nonnull error) {
        NSLog(@"error====%@",error);
    }];
}

- (void)didReceiveMemoryWarning {
    [super didReceiveMemoryWarning];
}

@end
