#import <Foundation/Foundation.h>

@interface EasyCamera : NSObject

@property(nonatomic,strong)NSString *appType;
@property(nonatomic,strong)NSString *snapURL;
@property(nonatomic,strong)NSString *terminalType;
@property(nonatomic,strong)NSString *name;
@property(nonatomic,strong)NSString *tag;
@property(nonatomic,strong)NSString *serial;
@property(nonatomic,assign)BOOL isOpen;
@property(nonatomic,strong)NSMutableArray *deviceArr;

@end
