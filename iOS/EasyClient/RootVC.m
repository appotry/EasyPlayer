//
//  ViewController.m
//  EasyDarwinPlayer
//
//  Created by Amber on 16/8/1.
//  Copyright © 2016年 Amber. All rights reserved.
//

#import "RootVC.h"
#import "AmberContainerVC.h"
#import "EasyDrawinSettingVC.h"
#import "EasyCameraVC.h"
#import "PhoneLivingVC.h"
#import "EasyNVRVC.h"



@interface RootVC ()<AmberContainerViewControllerDelegate>
{
    EasyCameraVC *_VC1;
    
    PhoneLivingVC *_VC2;
    
    EasyNVRVC *_VC3;
    int currentPage;
}
@end

@implementation RootVC

- (void)viewDidLoad {
    [super viewDidLoad];
    
    self.view.backgroundColor = [UIColor whiteColor];
    [self navigationSetting];
    [self initSomeView];
    //改变设置后本界面需要刷新数据
    [[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(changeConfig) name:@"changeConfig" object:nil];
}

//改变设置后强刷数据
- (void)changeConfig{
    NSString *cmsIp = [[NSUserDefaults standardUserDefaults] stringForKey:@"cms_ip"];
    NSString *cmsPort = [[NSUserDefaults standardUserDefaults] stringForKey:@"cms_port"];
    if (currentPage == 0) {
        _VC1.urlStr = [NSString stringWithFormat:@"http://%@:%@/api/getdevicelist?AppType=EasyCamera&TerminalType=ARM_Linux",cmsIp, cmsPort];
        [_VC1 requestListData];
    }else if(currentPage == 1){
        NSString *urlString= @"http://%@:%@/api/getdevicelist?AppType=EasyCamera&TerminalType=Android";
        _VC2.urlStr = [NSString stringWithFormat:urlString, cmsIp,cmsPort];
        [_VC2 requestListData];
    }else{
        _VC3.urlStr = [NSString stringWithFormat:@"http://%@:%@/api/getdevicelist?AppType=EasyNVR&TerminalType=ARM_Linux",cmsIp,cmsPort];
        [_VC3 requestListData];
    }
}

- (void)initSomeView
{
    _VC1 = [[EasyCameraVC alloc]init];
    
    _VC2 = [[PhoneLivingVC alloc]init];
    
    _VC3 = [[EasyNVRVC alloc]init];
    NSString *cmsIp = [[NSUserDefaults standardUserDefaults] stringForKey:@"cms_ip"];
    NSString *cmsPort = [[NSUserDefaults standardUserDefaults] stringForKey:@"cms_port"];
    float statusHeight = [[UIApplication sharedApplication] statusBarFrame].size.height;
    float navigationHeight = self.navigationController.navigationBar.frame.size.height;
    
    _VC1.title = @"EasyCamera";
    _VC2.title = @"手机直播";
    _VC3.title = @"EasyNVR";
    
    AmberContainerVC *containerVC = [[AmberContainerVC alloc]initWithControllers:@[_VC1,_VC2,_VC3]
                                                                    topBarHeight:statusHeight + navigationHeight
                                                            parentViewController:self];
    containerVC.delegate = self;
    
    [self.view addSubview:containerVC.view];
    _VC1.urlStr = [NSString stringWithFormat:@"http://%@:%@/api/getdevicelist?AppType=EasyCamera&TerminalType=ARM_Linux",cmsIp, cmsPort];
    [_VC1 requestListData];
}
#pragma mark -- AmberContainerViewControllerDelegate
- (void)containerViewItemIndex:(NSInteger)index currentController:(UIViewController *)controller
{
    NSLog(@"current controller : %@",controller);
    NSString *cmsIp = [[NSUserDefaults standardUserDefaults] stringForKey:@"cms_ip"];
    NSString *cmsPort = [[NSUserDefaults standardUserDefaults] stringForKey:@"cms_port"];
    currentPage = (short)index;
    if (index ==1) {
        NSString *urlString= @"http://%@:%@/api/getdevicelist?AppType=EasyCamera&TerminalType=Android";
        _VC2.urlStr = [NSString stringWithFormat:urlString, cmsIp,cmsPort];
        [_VC2 requestListData];
    }else if (index == 2)
    {
        _VC3.urlStr = [NSString stringWithFormat:@"http://%@:%@/api/getdevicelist?AppType=EasyNVR&TerminalType=ARM_Linux",cmsIp,cmsPort];
        [_VC3 requestListData];
    }
}

- (BOOL)prefersStatusBarHidden{
    return NO;
}

//导航栏设置
- (void)navigationSetting
{
    [self.navigationController.navigationBar setTitleTextAttributes:
     @{NSFontAttributeName:[UIFont systemFontOfSize:17],
       NSForegroundColorAttributeName:[UIColor whiteColor]}];
    
    UIView* logo = [[UIView alloc]init];
    logo.frame = CGRectMake(0, 0, 160, 30);
    UIImageView* img = [[UIImageView alloc]init];
    img.layer.masksToBounds = YES;
    img.layer.cornerRadius = 5;
    img.image = [UIImage imageNamed:@"EasyClient60.png"];
    img.frame = CGRectMake(0, 0, 30, 30);
    [logo addSubview:img];
    
    UILabel* label = [[UILabel alloc] initWithFrame:CGRectMake(40, 0, 130, 30)];
    [label setText:@"EasyClient"];
    [label setTextColor:[UIColor whiteColor]];
    [logo addSubview:label];
    self.navigationItem.leftBarButtonItem = [[UIBarButtonItem alloc] initWithCustomView:logo];
    
    UIButton *rightBtn = [UIButton buttonWithType:UIButtonTypeCustom];
    [rightBtn setFrame:CGRectMake(0, 0, 30, 30)];
    [rightBtn setBackgroundImage:[UIImage imageNamed:@"costom_setting"] forState:UIControlStateNormal];
    [rightBtn addTarget:self action:@selector(clickNavgationBarItem:) forControlEvents:UIControlEventTouchUpInside];
    UIBarButtonItem *rightItem = [[UIBarButtonItem alloc]initWithCustomView:rightBtn];
    self.navigationItem.rightBarButtonItem = rightItem;
}

//设置
-(void)clickNavgationBarItem:(UIButton *)oneButton
{
    EasyDrawinSettingVC *easyDrawinVC = [[EasyDrawinSettingVC alloc]init];
    [self.navigationController pushViewController:easyDrawinVC animated:YES];
}

- (void)dealloc{
    [[NSNotificationCenter defaultCenter] removeObserver:self];
}

- (void)didReceiveMemoryWarning {
    [super didReceiveMemoryWarning];
}

@end
