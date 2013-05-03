#import "tyran_cocoa_opengl_view.h"
#import <GLUT/glut.h>

// #include <lightning/mac_os_x/render_window_opengl.h>

@implementation cocoa_opengl_view

-(id)initWithFrame:(NSRect)frameRect {
	NSTimeInterval timeInterval = 0.005;
	self = [super initWithFrame:frameRect];
	self->boot = nimbus_boot_new();
	
	NSTimer *renderTimer = [[NSTimer scheduledTimerWithTimeInterval:timeInterval
		target:self
		selector:@selector(renderTimerCallback:)
		userInfo:nil
		repeats:YES] retain];
	//[[NSRunLoop currentRunLoop] addTimer:renderTimer forMode:NSEventTrackingRunLoopMode];
	[[NSRunLoop currentRunLoop] addTimer:renderTimer forMode:NSDefaultRunLoopMode];
	//[[NSRunLoop currentRunLoop] addTimer:renderTimer forMode:NSModalPanelRunLoopMode];
	//[renderTimer release];

	return self;
}

-(BOOL)isOpaque {
	return YES;
}

-(void)renderTimerCallback:(NSTimer*)theTimer {
	[self setNeedsDisplay:YES];
}

-(BOOL)acceptsFirstResponder {
	return YES;
}

-(void) keyUp:(NSEvent *)theEvent {
	if (theEvent.isARepeat) {
		return;
	}

//	uint keyCode = theEvent.keyCode;
//	nimbus::lightning::render_window_opengl::g_window->key_up(keyCode);
}

-(void) keyDown:(NSEvent *)theEvent {
	if (theEvent.isARepeat) {
		return;
	}

//	uint keyCode = theEvent.keyCode;
//	nimbus::lightning::render_window_opengl::g_window->key_down(keyCode);
}

- (void)mouseDown:(NSEvent *)theEvent {
	intensity = 1.0f;
//	NSPoint position = [self eventPosition:theEvent];
//	nimbus::vector2 pos;
//	pos.x = position.x;
//	pos.y = position.y;
//	nimbus::lightning::render_window_opengl::g_window->mouse_down(pos);
}

-(void)mouseUp:(NSEvent *)theEvent {
	intensity = 0.5f;
//	NSPoint position = [self eventPosition:theEvent];
//	nimbus::vector2 pos;
//	pos.x = position.x;
//	pos.y = position.y;
//	nimbus::lightning::render_window_opengl::g_window->mouse_up(pos);
}

- (void)mouseDragged:(NSEvent *)theEvent {
	intensity = 0.3f;
//	NSPoint position = [self eventPosition:theEvent];
//	nimbus::vector2 pos;
//	pos.x = position.x;
//	pos.y = position.y;
//	nimbus::lightning::render_window_opengl::g_window->mouse_drag(pos);
}

- (NSPoint) eventPosition:(NSEvent *)theEvent {
	NSPoint mouseLoc = [self convertPoint:[theEvent locationInWindow] fromView:nil];
	mouseLoc.x = (mouseLoc.x - 160.0f) / 160.0f;
	mouseLoc.y = (mouseLoc.y - 240.0f) / 160.0f;
	
	return mouseLoc;
}

- (void)drawRect:(NSRect)theRect {
	if (!nimbus_boot_ready_for_next_frame(self->boot)) {
		return;
	}
	
	tyran_boolean should_draw = nimbus_boot_should_render(self->boot);
	if (should_draw) {
		[[self openGLContext] makeCurrentContext];
		glFlush();
		[[self openGLContext] flushBuffer];
	}
	
	nimbus_boot_update(self->boot);
	
}
@end
