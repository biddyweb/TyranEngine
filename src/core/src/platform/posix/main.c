#include "../../base/boot/nimbus_boot.h"
#include "../../base/thread/nimbus_thread_sleep.h"
#include <stdio.h>
#include <tyranscript/tyran_log.h>
#include "log.h"

#include <signal.h>

int g_terminate = 0;

static void signal_callback_handler(int signum)
{
	if (signum == SIGTERM || signum == SIGINT) {
		g_terminate = 1;
	}
}

int main()
{
	signal(SIGINT, signal_callback_handler);
	g_log.log = nimbus_log_posix;

	nimbus_boot* boot = nimbus_boot_new();

	while (!g_terminate) {
		printf("main::update\n");
		int err = nimbus_boot_update(boot);
		if (err < -1) {
			break;
		}
		nimbus_thread_sleep(0.8f);
	}

	TYRAN_LOG("main::destroy");
	nimbus_boot_destroy(boot);

	TYRAN_LOG("main::exit");

	return 0;
}
