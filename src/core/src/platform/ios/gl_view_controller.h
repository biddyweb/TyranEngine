//
//  ViewController.h
//  GAMENAME
//
//  Created by Peter Björklund on 2013-05-09.
//  Copyright (c) 2013 Outbreak Studios. All rights reserved.
//

#import <UIKit/UIKit.h>
#import <GLKit/GLKit.h>

#include "../../base/boot/nimbus_boot.h"

@interface ViewController :
GLKViewController {
	nimbus_boot* boot;
}


@end
