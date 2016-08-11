//
//  ViewController.h
//  scan_qrcode_demo
//
//  Created by strivingboy on 14/11/3.
//
//

#import <UIKit/UIKit.h>

@protocol MyURLDelegate <NSObject>

-(void)getUrlStr:(NSString *)urlStr;

@end

@interface ScanViewContriller : UIViewController

@property(assign)id<MyURLDelegate>delegate;
@end

