#import <Foundation/Foundation.h>

@interface EasyInfo : NSObject
@property(nonatomic,strong)NSString *channel;
@property(nonatomic,strong)NSString *name;
@property(nonatomic,strong)NSString *snapURL;
@property(nonatomic,strong)NSString *status;
@property(nonatomic,strong)NSString *serial;
@property(nonatomic,strong)NSString *appType;
@property(nonatomic,strong)NSString *terminalType;
@property(nonatomic,assign)BOOL isOpen;
@end
