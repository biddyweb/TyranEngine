#import <Cocoa/Cocoa.h>

extern "C" {
#include "../../base/boot/nimbus_boot.h"
}

@interface cocoa_opengl_view : NSOpenGLView {
	float intensity;
	nimbus_boot* boot;
}

- (id)initWithFrame:(NSRect)frameRect;
- (void)renderTimerCallback:(NSTimer*)theTimer;

- (BOOL)isOpaque;
- (void)drawRect:(NSRect)theRect;
- (void)mouseDown:(NSEvent*)theEvent;
- (void)mouseUp:(NSEvent*)theEvent;
- (void)mouseDragged:(NSEvent*)theEvent;
- (void)keyDown:(NSEvent*)theEvent;
- (void)keyUp:(NSEvent*)theEvent;
- (NSPoint)eventPosition:(NSEvent*)theEvent;
- (BOOL)acceptsFirstResponder;

@end
