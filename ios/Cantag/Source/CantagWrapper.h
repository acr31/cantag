#import <Foundation/Foundation.h>

@interface CantagWrapper : NSObject

+ (void)process:(int)width :(int)height :(int)widthStep :(unsigned char *)data :(unsigned char *)debugData;

@end
