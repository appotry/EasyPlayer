#import <UIKit/UIKit.h>
@class EasyUrl;

#define kNumberAudioQueueBuffers 3
#define kDefaultSampleRate 8000
#define kDefaultInputBufferSize 1600
#define kDefaultInputBufferSizeL 320
#define kDefaultOutputBufferSize 7040

@interface PlayViewController : UIViewController
@property (nonatomic,strong)UIButton   *doneBtn,*picBtn,*tempBtn,*midBtn;
@property(nonatomic,strong)EasyUrl *urlModel;
@property(nonatomic,strong)NSNumber *pts;
//提示框
-(void)hudNotice:(NSString *)message;
@end

