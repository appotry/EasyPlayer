#import <Foundation/Foundation.h>

@protocol NetRequestDelegate <NSObject>

- (void)receiveData:(NSMutableArray *)sender;

@end

@interface NetRequestTool : NSObject<NSURLSessionDataDelegate>
@property (nonatomic, strong) NSMutableData *responseData;
@property(assign)id<NetRequestDelegate>delegate;

- (void)requestListData:(NSString *)urlStr;
// 请求控制摄像头数据
- (void)requestControlCamera:(NSString *)urlStr;
@end
