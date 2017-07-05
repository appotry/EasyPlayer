//
//  AppDelegate.m
//  EasyDarwinPlayer
//
//  Created by Amber on 16/8/1.
//  Copyright © 2016年 Amber. All rights reserved.
//

#import "AppDelegate.h"
#import "RootVC.h"
@interface AppDelegate ()

@end

@implementation AppDelegate


- (BOOL)application:(UIApplication *)application didFinishLaunchingWithOptions:(NSDictionary *)launchOptions {
    // Override point for customization after application launch.
    self.window = [[UIWindow alloc]initWithFrame:[[UIScreen mainScreen] bounds]];
    self.window.backgroundColor = [UIColor whiteColor];
    
    UINavigationController *rootNavVC = [[UINavigationController alloc]initWithRootViewController:[[RootVC alloc]init]];
    [UINavigationBar appearance].barTintColor = MAIN_COLOR;
    self.window.rootViewController = rootNavVC;
    [self.window makeKeyAndVisible];
    
    NSString *host = [[NSUserDefaults standardUserDefaults] objectForKey:@"cms_ip"];
    
    if (!host || [host isEqualToString:@"121.40.50.44"]) {
        //        NSDictionary *defaultValues = [NSDictionary dictionaryWithObjectsAndKeys: @"cloud.easydarwin.org", @"cms_ip", @"10000", @"cms_port", nil];
        
        [[NSUserDefaults standardUserDefaults] setValue:@"cloud.easydarwin.org" forKey:@"cms_ip"];
        [[NSUserDefaults standardUserDefaults] setValue:@"10000" forKey:@"cms_port"];
        [[NSUserDefaults standardUserDefaults] synchronize];
    }

    return YES;
}

- (void)applicationWillResignActive:(UIApplication *)application {
    // Sent when the application is about to move from active to inactive state. This can occur for certain types of temporary interruptions (such as an incoming phone call or SMS message) or when the user quits the application and it begins the transition to the background state.
    // Use this method to pause ongoing tasks, disable timers, and throttle down OpenGL ES frame rates. Games should use this method to pause the game.
}

- (void)applicationDidEnterBackground:(UIApplication *)application {
    // Use this method to release shared resources, save user data, invalidate timers, and store enough application state information to restore your application to its current state in case it is terminated later.
    // If your application supports background execution, this method is called instead of applicationWillTerminate: when the user quits.
}

- (void)applicationWillEnterForeground:(UIApplication *)application {
    // Called as part of the transition from the background to the inactive state; here you can undo many of the changes made on entering the background.
}

- (void)applicationDidBecomeActive:(UIApplication *)application {
    // Restart any tasks that were paused (or not yet started) while the application was inactive. If the application was previously in the background, optionally refresh the user interface.
}

- (void)applicationWillTerminate:(UIApplication *)application {
    // Called when the application is about to terminate. Save data if appropriate. See also applicationDidEnterBackground:.
}

@end
