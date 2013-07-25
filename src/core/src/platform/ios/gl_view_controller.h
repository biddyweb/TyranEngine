#import <UIKit/UIKit.h>
#import <GLKit/GLKit.h>

#include "../../base/boot/nimbus_boot.h"

@interface ViewController :
GLKViewController {
	nimbus_boot* boot;
}

-(void)touchesBegan:
(NSSet*)touches withEvent:
(UIEvent*)event;
-(void)touchesMoved:
(NSSet*)touches withEvent:
(UIEvent*)event;
-(void)touchesEnded:
(NSSet*)touches withEvent:
(UIEvent*)event;
-(void)touchesCancelled:
(NSSet*)touches withEvent:
(UIEvent*)event;

@end
