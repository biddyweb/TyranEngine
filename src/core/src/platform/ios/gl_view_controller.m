#import "gl_view_controller.h"
#include <tyran_engine/event/touch_changed.h>

#include <tyran_core/platform/posix/log.h>

@implementation NimbusTouch

@synthesize phase;
@synthesize position;
@synthesize fingerId;

-(id) initWithPhase:(UITouchPhase)_phase andPosition:(CGPoint)_position andFingerId:(NSUInteger) _fingerId {
	self = [super init];
	self->phase = _phase;
	self->position = _position;
	self->fingerId = _fingerId;
	return self;
}

@end


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
	g_log.log = nimbus_log_posix;

	self.context = [[EAGLContext alloc] initWithAPI:kEAGLRenderingAPIOpenGLES2];

	if (!self.context) {
		NSLog(@"Failed to create ES context");
	}
    
	GLKView *view = (GLKView *)self.view;
	view.context = self.context;
	view.drawableDepthFormat = GLKViewDrawableDepthFormat24;
	self.preferredFramesPerSecond = 60;
	self->allTouches = [[NSMutableArray alloc] initWithCapacity:10];
	for (int i=0; i<10; ++i) {
		[self->allTouches addObject:[NSNull null]];
	}
	self->touchLock = [[NSLock alloc]init];
	self->touchSet = [[NSMutableSet alloc] init];

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

-(NSUInteger)getFingerId: (UITouch*) touch
{
	NSUInteger index = 0;
	for (id existingTouch in self->allTouches) {
		if (existingTouch == touch) {
			return index;
		}
		index++;
	}
	return index;
}

-(void)storeTouchEvent:(NSSet *)touches
{
	NSLog(@"Store");
	for (UITouch* touch in touches) {
		NSLog(@"touch");
		NSUInteger fingerIndex = [self getFingerId: touch];
		NimbusTouch* copy = [[NimbusTouch alloc] initWithPhase:touch.phase andPosition:[touch locationInView:self.view] andFingerId:fingerIndex];

		[self->touchLock lock];
		[self->touchSet addObject:copy];
		[self->touchLock unlock];
	}
	NSLog(@"--- Store done");
}

-(void)sendAllTouchEvents:(NSSet *)touches
{
	nimbus_touch_changed changed;
	nimbus_event_type_id type_id;
	
	for (NimbusTouch* touch in touches) {
		switch (touch.phase) {
			case UITouchPhaseBegan:
				type_id = NIMBUS_EVENT_TOUCH_BEGAN_ID;
				break;
			case UITouchPhaseMoved:
				type_id = NIMBUS_EVENT_TOUCH_MOVED_ID;
				break;
			case UITouchPhaseStationary:
				type_id = NIMBUS_EVENT_TOUCH_STATIONARY_ID;
				break;
			case UITouchPhaseEnded:
				type_id = NIMBUS_EVENT_TOUCH_ENDED_ID;
				break;
			case UITouchPhaseCancelled:
				continue;
				break;
		}
		changed.position.x = touch.position.x - self.view.frame.size.width / 2;
		changed.position.y = self.view.frame.size.height / 2 - touch.position.y;
		changed.finger_id = touch.fingerId;
		NSLog(@"test:%d", changed.finger_id);
		nimbus_boot_send_event(self->boot, type_id, &changed, sizeof(changed));
	}
}

- (void)touchesBegan:(NSSet *)touches withEvent:(UIEvent *)event
{
	self->allTouches = [event allTouches];
	[self storeTouchEvent: touches];
}

- (void)touchesMoved:(NSSet *)touches withEvent:(UIEvent *)event
{
	self->allTouches = [event allTouches];
	[self storeTouchEvent: touches];
}

- (void)touchesEnded:(NSSet *)touches withEvent:(UIEvent *)event
{
	self->allTouches = [event allTouches];
	[self storeTouchEvent: touches];
}

- (void)touchesCancelled:(NSSet *)touches withEvent:(UIEvent *)event
{
	self->allTouches = [event allTouches];
	[self storeTouchEvent: touches];
}


#pragma mark - GLKView and GLKViewController delegate methods

- (void)update
{
}

-(void) sendAndClearTouches {
	[self->touchLock lock];
	[self sendAllTouchEvents:self->touchSet];
	self->touchSet = [[NSMutableSet alloc] init];
	[self->touchLock unlock];
}

- (void)glkView:(GLKView *)view drawInRect:(CGRect)rect
{
	int err = nimbus_boot_update(self->boot);
	if (err != -1) {
		[self sendAndClearTouches];
	} else {
		glClearColor(1.0f, 0.1f, 0.1f, 0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		NSLog(@"************* LOST FRAME *************");
	}
}

@end
