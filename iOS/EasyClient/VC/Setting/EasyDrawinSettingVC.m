#import "EasyDrawinSettingVC.h"

@interface EasyDrawinSettingVC ()
{
    UITextField *_ipTextField,*_portTextField;
}
@end

@implementation EasyDrawinSettingVC

- (void)viewDidLoad {
    [super viewDidLoad];
     self.view.backgroundColor = [UIColor whiteColor];
    [self nsvigationSetting];
    [self initSomeView];
}

//导航栏设置
- (void)nsvigationSetting
{
   self.navigationItem.title = @"设置";
    [self.navigationController.navigationBar setTitleTextAttributes:
     @{NSFontAttributeName:[UIFont systemFontOfSize:17],
       NSForegroundColorAttributeName:[UIColor whiteColor]}];
    
    UIButton *leftBtn = [UIButton buttonWithType:UIButtonTypeCustom];
    [leftBtn setFrame:CGRectMake(0, 0,30, 30)];
    [leftBtn setBackgroundImage:[UIImage imageNamed:@"custom_back"] forState:UIControlStateNormal];
    leftBtn.tag = 101;
    [leftBtn addTarget:self action:@selector(clickNavgationBarItem:) forControlEvents:UIControlEventTouchUpInside];
    UIBarButtonItem *leftItem = [[UIBarButtonItem alloc]initWithCustomView:leftBtn];
    self.navigationItem.leftBarButtonItem = leftItem;
    
    
    UIButton *rightBtn = [UIButton buttonWithType:UIButtonTypeCustom];
    [rightBtn setFrame:CGRectMake(0, 0,40, 30)];
    rightBtn.titleLabel.font = [UIFont systemFontOfSize:17];
    [rightBtn setTitle:@"保存" forState:UIControlStateNormal];
    [rightBtn addTarget:self action:@selector(clickNavgationBarItem:) forControlEvents:UIControlEventTouchUpInside];
    rightBtn.tag = 102;
    UIBarButtonItem *rightItem = [[UIBarButtonItem alloc]initWithCustomView:rightBtn];
    self.navigationItem.rightBarButtonItem = rightItem;
}

- (void)initSomeView
{
    UILabel* label1 = [[UILabel alloc] initWithFrame:CGRectMake(15, 75, 60, 30)];
    label1.backgroundColor = [UIColor clearColor];
    label1.text = @"IP地址";
    label1.textColor = MAIN_COLOR;
    
    [self.view addSubview:label1];
    _ipTextField = [[UITextField alloc] initWithFrame:CGRectMake(75.0f,70, ScreenWidth - 100, 40.0f)];
    [_ipTextField setBorderStyle:UITextBorderStyleRoundedRect]; //外框类型
    _ipTextField.placeholder = @"请输入IP地址"; //默认显示的字
    _ipTextField.autocorrectionType = UITextAutocorrectionTypeNo;//设置是否启动自动提醒更正功能
    _ipTextField.returnKeyType = UIReturnKeyDone;  //键盘返回类型
    _ipTextField.clearButtonMode = UITextFieldViewModeWhileEditing; //编辑时会出现个修改X
    _ipTextField.keyboardType = UIKeyboardTypeDefault;//键盘显示类型
    [self.view addSubview:_ipTextField];
    
    UILabel* label2 = [[UILabel alloc] initWithFrame:CGRectMake(15, 120, 60, 30)];
    label2.backgroundColor = [UIColor clearColor];
    label2.text = @"端口";
    label2.textColor = MAIN_COLOR;
    
    [self.view addSubview:label2];
    _portTextField = [[UITextField alloc] initWithFrame:CGRectMake(75.0f,115, ScreenWidth - 100, 40.0f)];
    [_portTextField setBorderStyle:UITextBorderStyleRoundedRect]; //外框类型
    _portTextField.placeholder = @"请输入端口"; //默认显示的字
    _portTextField.autocorrectionType = UITextAutocorrectionTypeNo;//设置是否启动自动提醒更正功能
    _portTextField.returnKeyType = UIReturnKeyDone;  //键盘返回类型
    _portTextField.clearButtonMode = UITextFieldViewModeWhileEditing; //编辑时会出现个修改X
    _portTextField.keyboardType = UIKeyboardTypeNumberPad;//键盘显示类型
    [self.view addSubview:_portTextField];
    
    NSUserDefaults *user = [NSUserDefaults standardUserDefaults];
    NSString* ip = [user valueForKey:@"cms_ip"];
    NSString* port = [user valueForKey:@"cms_port"];
    [_ipTextField setText:ip];
    [_portTextField setText:port];
}
//设置
-(void)clickNavgationBarItem:(UIButton *)oneButton
{
    if (oneButton.tag == 102) {
        NSUserDefaults *user = [NSUserDefaults standardUserDefaults];
        [user setObject:_ipTextField.text forKey:@"cms_ip"];
        [user setObject: _portTextField.text forKey:@"cms_port"];
        [[NSUserDefaults standardUserDefaults] synchronize];
    }
    [self.navigationController popToRootViewControllerAnimated:YES];
}

- (void)didReceiveMemoryWarning {
    [super didReceiveMemoryWarning];
}

@end
