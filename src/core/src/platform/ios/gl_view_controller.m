#import "gl_view_controller.h"
#include <tyran_engine/event/touch_changed.h>

@interface ViewController () {
}
@property (strong, nonatomic) EAGLContext *context;
@property (strong, nonatomic) GLKBaseEffect *effect;

- (void)setupGL;
- (void)tearDownGL;

@end

@implementation ViewController

- (void)viewDidLoad
{
    [super viewDidLoad];
    
    self.context = [[EAGLContext alloc] initWithAPI:kEAGLRenderingAPIOpenGLES2];

    if (!self.context) {
        NSLog(@"Failed to create ES context");
    }
    
    GLKView *view = (GLKView *)self.view;
    view.context = self.context;
    view.drawableDepthFormat = GLKViewDrawableDepthFormat24;

    self->boot = nimbus_boot_new();

    [self setupGL];
}

- (void)dealloc
{    
    [self tearDownGL];
    
    if ([EAGLContext currentContext] == self.context) {
        [EAGLContext setCurrentContext:nil];
    }
    [super dealloc];
}

- (void)didReceiveMemoryWarning
{
    [super didReceiveMemoryWarning];

    if ([self isViewLoaded] && ([[self view] window] == nil)) {
        self.view = nil;
        
        [self tearDownGL];
        
        if ([EAGLContext currentContext] == self.context) {
            [EAGLContext setCurrentContext:nil];
        }
        self.context = nil;
    }

    // Dispose of any resources that can be recreated.
}

- (void)setupGL
{
    [EAGLContext setCurrentContext:self.context];
}

- (void)tearDownGL
{
    [EAGLContext setCurrentContext:self.context];
}

-(void)sendTouchEvent:(NSSet *)touches withEvent:(UIEvent *)event
{
	nimbus_touch_changed changed;
	
	for (UITouch* touch in touches) {
		switch (touch.phase) {
			case UITouchPhaseBegan:
				changed.phase = nimbus_touch_phase_began;
				break;
			case UITouchPhaseMoved:
				changed.phase = nimbus_touch_phase_moved;
				break;
			case UITouchPhaseStationary:
				continue;
				break;
			case UITouchPhaseEnded:
				changed.phase = nimbus_touch_phase_ended;
				break;
			case UITouchPhaseCancelled:
				changed.phase = nimbus_touch_phase_cancelled;
				break;
		}
		CGPoint touch_position = [touch locationInView:self.view];
		changed.position.x = touch_position.x;
		changed.position.y = touch_position.y;
		nimbus_boot_send_event(self->boot, NIMBUS_EVENT_TOUCH_CHANGED, &changed, sizeof(changed));
	}
}

- (void)touchesBegan:(NSSet *)touches withEvent:(UIEvent *)event
{
	NSLog(@"touches began");
	[self sendTouchEvent: touches withEvent: event];
}

- (void)touchesMoved:(NSSet *)touches withEvent:(UIEvent *)event
{
	NSLog(@"touches moved");
	[self sendTouchEvent: touches withEvent: event];
}

- (void)touchesEnded:(NSSet *)touches withEvent:(UIEvent *)event
{
	NSLog(@"touches ended");
	[self sendTouchEvent: touches withEvent: event];
}

- (void)touchesCancelled:(NSSet *)touches withEvent:(UIEvent *)event
{
	NSLog(@"touches cancelled");
	[self sendTouchEvent: touches withEvent: event];
}


#pragma mark - GLKView and GLKViewController delegate methods

- (void)update
{
}

- (void)glkView:(GLKView *)view drawInRect:(CGRect)rect
{
	if (nimbus_boot_ready_for_next_frame(self->boot)) {
		nimbus_boot_update(self->boot);
	} else {
		glClearColor(1.0f, 0.1f, 0.1f, 0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		NSLog(@"************* LOST FRAME *************");
	}
}

@end
