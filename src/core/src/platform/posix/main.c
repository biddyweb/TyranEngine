#include "../../base/boot/nimbus_boot.h"
#include "../../base/thread/nimbus_thread_sleep.h"
#include <stdio.h>

int main()
{
	nimbus_boot* boot = nimbus_boot_new();

	while (1) {
		printf("main::update\n");
		if (nimbus_boot_ready_for_next_frame(boot)) {
			int err = nimbus_boot_update(boot);
			if (err) {
				break;
			}
		}
		nimbus_thread_sleep(0.5f);
	}
	nimbus_boot_destroy(boot);
	return 0;
}
