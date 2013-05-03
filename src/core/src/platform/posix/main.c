#include "../../base/boot/nimbus_boot.h"

int main()
{
	nimbus_boot* boot = nimbus_boot_new();
	nimbus_boot_update(boot);
	nimbus_boot_destroy(boot);
}
