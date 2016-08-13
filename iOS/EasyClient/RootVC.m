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
    

}
@end

@implementation RootVC

- (void)viewDidLoad {
    [super viewDidLoad];
    
    self.view.backgroundColor = [UIColor whiteColor];
    [self nsvigationSetting];
    [self initSomeView];
    
}

- (void)initSomeView
{
     _VC1 = [[EasyCameraVC alloc]init];
    
     _VC2 = [[PhoneLivingVC alloc]init];
    
     _VC3 = [[EasyNVRVC alloc]init];

    float statusHeight = [[UIApplication sharedApplication] statusBarFrame].size.height;
    float navigationHeight = self.navigationController.navigationBar.frame.size.height;
    
     _VC1.title = @"EasyCamera";
     _VC2.title = @"Phone Living";
     _VC3.title = @"EasyNVR";
    
    AmberContainerVC *containerVC = [[AmberContainerVC alloc]initWithControllers:@[_VC1,_VC2,_VC3]
                                                                                        topBarHeight:statusHeight + navigationHeight
                                                                                parentViewController:self];
    containerVC.delegate = self;
    containerVC.menuItemFont = [UIFont fontWithName:@"Futura-Medium" size:16];
    
    [self.view addSubview:containerVC.view];
    [_VC1 requestListData];
}
#pragma mark -- AmberContainerViewControllerDelegate
- (void)containerViewItemIndex:(NSInteger)index currentController:(UIViewController *)controller
{
    //    NSLog(@"current Index : %ld",(long)index);
    NSLog(@"current controller : %@",controller);
    //    [controller viewWillAppear:YES];
    if (index ==1) {
            [_VC2 requestListData];
    }else if (index == 2)
    {
            [_VC3 requestListData];
    }
}



//导航栏设置
- (void)nsvigationSetting
{
    self.navigationItem.title = @"EasyDarwin";
    [self.navigationController.navigationBar setTitleTextAttributes:
     @{NSFontAttributeName:[UIFont systemFontOfSize:17],
       NSForegroundColorAttributeName:[UIColor whiteColor]}];
    
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

- (void)didReceiveMemoryWarning {
    [super didReceiveMemoryWarning];
    // Dispose of any resources that can be recreated.
}

@end
