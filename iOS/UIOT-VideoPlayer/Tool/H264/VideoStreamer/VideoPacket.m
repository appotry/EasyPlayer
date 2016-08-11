
#include "VideoPacket.h"

//const uint8_t KStartCode[4] = {0, 0, 0, 1};

@implementation VideoPacket
- (instancetype)initWithSize:(NSInteger)size
{
    self = [super init];
    self.buffer = malloc(size);
    self.size = size;
    
    return self;
}

-(void)dealloc
{
//    free(self.buffer);
}
@end
