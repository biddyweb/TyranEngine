#import <UIKit/UIKit.h>
#import <GLKit/GLKit.h>

#include "../../base/boot/nimbus_boot.h"

@interface NimbusTouch :
NSObject {
	CGPoint position;
	UITouchPhase phase;


}

-(id) initWithPhase:
(UITouchPhase)phase andPosition:
(CGPoint)position;

@property(nonatomic) CGPoint position;
@property(nonatomic) UITouchPhase phase;

@end


@interface ViewController :
GLKViewController {
	nimbus_boot* boot;
	NSLock* touchLock;
	NSMutableSet* touchSet;
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
