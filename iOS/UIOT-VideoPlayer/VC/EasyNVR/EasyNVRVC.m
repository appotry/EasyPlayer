//
//  EasyCameraVC00.m
//  EasyDarwinPlayer
//
//  Created by Amber on 16/8/1.
//  Copyright © 2016年 Amber. All rights reserved.
//

#import "EasyNVRVC.h"
#import "EasyCameraCell.h"
#import "EasyCamera.h"
#import "EasyNVRCell.h"
#import "EasyInfo.h"
#import "EasyUrl.h"

@interface EasyNVRVC ()<NetRequestDelegate>
{
    
    NSMutableArray *_dataArr;
    UiotHUD *_HUD;
    NSString *_urlStr;
    EasyCamera *_currentCamera;
}

@property(nonatomic,strong) NetRequestTool *requestTool;
@end

@implementation EasyNVRVC

static NSString *cellIdentifier1 = @"Cell1";
static NSString *cellIdentifier2 = @"Cell2";

- (void)viewDidLoad {
    
    [super viewDidLoad];
    self.view.backgroundColor = [UIColor whiteColor];
    
    //    [self nsvigationSetting];
    [self initSomeView];
    _dataArr = [NSMutableArray array];
    self.requestTool = [[NetRequestTool alloc]init];
    self.requestTool .delegate = self;
    _urlStr= @"http://121.40.50.44:10000/api/getdevicelist?AppType=EasyNVR&TerminalType=ARM_Linux";
    
    
}

- (void)initSomeView
{
    self.view.backgroundColor = [UIColor whiteColor];
    UICollectionViewFlowLayout *layout = [[UICollectionViewFlowLayout alloc]init];
    //定义每个UICollectionView 横向的间距
    layout.minimumLineSpacing = 2;
    //定义每个UICollectionView 纵向的间距
    layout.minimumInteritemSpacing = 0;
    //定义每个UICollectionView 的边距距
    layout.sectionInset = UIEdgeInsetsMake(0, 2, 5, 2);//上左下右
    self.collectionView = [[UICollectionView alloc]initWithFrame:CGRectMake(0, 0, ScreenWidth, ScreenHeight-64-40) collectionViewLayout:layout];
    self.collectionView.delegate = self;
    self.collectionView.dataSource = self;
    self.collectionView.showsHorizontalScrollIndicator = NO;
    self.collectionView.showsVerticalScrollIndicator = NO;
    self.collectionView.backgroundColor = [UIColor whiteColor];
    
    
    [self.collectionView registerClass:[EasyCameraCell class] forCellWithReuseIdentifier:cellIdentifier1];
    [self.collectionView registerClass:[EasyNVRCell class] forCellWithReuseIdentifier:cellIdentifier2];
    [self.view addSubview:self.collectionView];
    
    __weak typeof(self) weakSelf = self;
    //1. 添加头部控件的方法
    self.collectionView.mj_header = [MJRefreshNormalHeader headerWithRefreshingBlock:^{
        [_HUD show:YES];
        [weakSelf requestListData];
        
        
    }];
    
    _HUD = [[UiotHUD alloc]initWithUiotView:self.view];
    [self.view addSubview:_HUD];
    [self.view bringSubviewToFront:_HUD];
    [_HUD show:YES];
    [_HUD hide:YES afterDelay:10.0];
    
}

#pragma mark -  receiveData
- (void)receiveData:(NSMutableArray *)sender
{
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



- (NSInteger)numberOfSectionsInCollectionView:(UICollectionView *)collectionView
{
    return _dataArr.count;
}
// 返回Items的个数 （图片的个数）
-(NSInteger)collectionView:(UICollectionView *)collectionView numberOfItemsInSection:(NSInteger)section
{
    EasyCamera *model = _dataArr[section];
    
    if (model.isOpen) {
        
        return model.deviceArr.count>0 ? model.deviceArr.count+1:1;
        
    }else{
        
        return 1;
        
    }
}

// item大小
-(CGSize)collectionView:(UICollectionView *)collectionView layout:(UICollectionViewLayout *)collectionViewLayout sizeForItemAtIndexPath:(NSIndexPath *)indexPath
{
    if (indexPath.row == 0) {
        return  CGSizeMake(ScreenWidth-5, 30);//CGSize size={ScreenWidth-10,20};
    }else
    {
        return CGSizeMake((ScreenWidth-10)/2, (ScreenWidth-10)/2);
        
    }
}

-(UICollectionViewCell *)collectionView:(UICollectionView *)collectionView cellForItemAtIndexPath:(NSIndexPath *)indexPath
{
    
    if (indexPath.row == 0) {
        EasyNVRCell * cell = [collectionView dequeueReusableCellWithReuseIdentifier:cellIdentifier2 forIndexPath:indexPath];
        
        EasyCamera *model = _dataArr[indexPath.row];
        
        [cell produceCellModel:model];
        
        cell.backgroundColor = COLOR(152, 152, 152, 152);
        return cell;
    }else{
        
        EasyCameraCell * cell = [collectionView dequeueReusableCellWithReuseIdentifier:cellIdentifier1 forIndexPath:indexPath];
        
        EasyCamera *model = _dataArr[indexPath.section];
        
        EasyInfo *model1 = model.deviceArr[indexPath.row-1];
        
        [cell produceCellInfoModel:model1];
        
        return cell;
    }
    
    
}

-(void)collectionView:(UICollectionView *)collectionView didSelectItemAtIndexPath:(NSIndexPath *)indexPath
{
    NSLog(@"---%d---%d",indexPath.section,indexPath.row);
    if (indexPath.row == 0) {
        EasyCamera *model = _dataArr[indexPath.section];
        _currentCamera = model;
        model.isOpen = !model.isOpen;
        [self requestListData:model.serial];
    }else
    {
      

       
         EasyCamera *model = _dataArr[indexPath.section];
        EasyInfo *model1 = model.deviceArr[indexPath.row-1];
        
        [self requestUrlData:model.serial channal:model1.channel];
    }
    
    
    
}


//http://121.40.50.44:10000/api/getdevicestream?device=001001000010&channel=0&protocol=RTSP&reserve=1
// 请求数据
- (void)requestUrlData:(NSNumber *)Serial channal:(NSString *)channal
{
    AFHTTPSessionManager * manager = [AFHTTPSessionManager manager];
    manager.responseSerializer.acceptableContentTypes = [NSSet setWithObjects:@"html/text",@"text/plain", nil];
    //    NSString *urlStr = @"http://121.40.50.44:10000/api/getdevicelist";
    //    NSDictionary *parametersDic = @{@"AppType":@"EasyNVR",@"TerminalType":@"ARM_Linux"};
    NSString *urlStr =[NSString stringWithFormat:@"http://121.40.50.44:10000/api/getdevicestream?device=%@&channel=%@&protocol=RTSP&reserve=1",Serial,channal];// @"http://121.40.50.44:10000/api/getdevicelist?AppType=c";
    NSLog(@"----%@",urlStr);
    
    [manager POST:urlStr parameters:nil progress:^(NSProgress * _Nonnull uploadProgress) {
        
    } success:^(NSURLSessionDataTask * _Nonnull task, id  _Nullable responseObject) {
        
        NSLog(@"EasyNVRVC===%@",responseObject);
        NSDictionary *easyDic = [responseObject objectForKey:@"EasyDarwin"];
        NSDictionary *headerDic =  [easyDic objectForKey:@"Header"];
        NSDictionary *bodyDic =  [easyDic objectForKey:@"Body"];
        if ([[headerDic objectForKey:@"ErrorNum"] isEqualToString:@"200"]) {
            
           EasyUrl  *urlModel = [[EasyUrl alloc]init];
            urlModel.channel = [bodyDic objectForKey:@"Channel"];
            urlModel.protocol = [bodyDic objectForKey:@"Protocol"];
            urlModel.reserve = [bodyDic objectForKey:@"Reserve"];
            urlModel.serial = [bodyDic objectForKey:@"Serial"];
            urlModel.url = [bodyDic objectForKey:@"URL"];
            PlayViewController *playVC = [[PlayViewController alloc]init];
            playVC.urlModel = urlModel;
            [self presentViewController:playVC animated:YES completion:nil];
            
            
            
        }
        
        
    } failure:^(NSURLSessionDataTask * _Nullable task, NSError * _Nonnull error) {
        NSLog(@"error====%@",error);
    }];
}




// 请求数据
- (void)requestListData:(NSNumber *)deviceStr
{
    AFHTTPSessionManager * manager = [AFHTTPSessionManager manager];
    manager.responseSerializer.acceptableContentTypes = [NSSet setWithObjects:@"html/text",@"text/plain", nil];
    //    NSString *urlStr = @"http://121.40.50.44:10000/api/getdevicelist";
    //    NSDictionary *parametersDic = @{@"AppType":@"EasyNVR",@"TerminalType":@"ARM_Linux"};
    NSString *urlStr =[NSString stringWithFormat:@"http://121.40.50.44:10000/api/getdeviceinfo?device=%@",deviceStr];// @"http://121.40.50.44:10000/api/getdevicelist?AppType=c";
    NSLog(@"----%@",urlStr);
    
    [manager POST:urlStr parameters:nil progress:^(NSProgress * _Nonnull uploadProgress) {
        
    } success:^(NSURLSessionDataTask * _Nonnull task, id  _Nullable responseObject) {
        
        NSLog(@"EasyNVRVC===%@",responseObject);
        NSDictionary *easyDic = [responseObject objectForKey:@"EasyDarwin"];
        NSDictionary *headerDic =  [easyDic objectForKey:@"Header"];
        NSDictionary *bodyDic =  [easyDic objectForKey:@"Body"];
        if ([[headerDic objectForKey:@"ErrorNum"] isEqualToString:@"200"]) {
            NSArray *devices = [bodyDic objectForKey:@"Channels"] ;
            NSMutableArray *dataArr = [[NSMutableArray alloc]init];
            for (NSDictionary *deviceDic in devices) {
                
                EasyInfo *model = [[EasyInfo alloc]init];
                model.channel = [deviceDic objectForKey:@"Channel"];
                model.name = [deviceDic objectForKey:@"Name"];
                model.status = [deviceDic objectForKey:@"Status"];
                model.snapURL = [deviceDic objectForKey:@"SnapURL"];
                [dataArr addObject:model];
            }
            _currentCamera.deviceArr =dataArr;
            //            _dataArr = dataArr;
            [self.collectionView reloadData];
        }
        
        
    } failure:^(NSURLSessionDataTask * _Nullable task, NSError * _Nonnull error) {
        NSLog(@"error====%@",error);
    }];
}

- (void)didReceiveMemoryWarning {
    [super didReceiveMemoryWarning];
    // Dispose of any resources that can be recreated.
}

/*
 #pragma mark - Navigation
 
 // In a storyboard-based application, you will often want to do a little preparation before navigation
 - (void)prepareForSegue:(UIStoryboardSegue *)segue sender:(id)sender {
 // Get the new view controller using [segue destinationViewController].
 // Pass the selected object to the new view controller.
 }
 */

@end
