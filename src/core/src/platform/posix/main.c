#include "../../base/boot/nimbus_boot.h"
#include "../../base/thread/nimbus_thread_sleep.h"
#include <stdio.h>

int main()
{
	nimbus_boot* boot = nimbus_boot_new();
	
	while (1)
	{
		int err = nimbus_boot_update(boot);
		if (err) {
			break;
		}
		nimbus_thread_sleep(0.5f);
	}
	nimbus_boot_destroy(boot);
	return 0;
}
