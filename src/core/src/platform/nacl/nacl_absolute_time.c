#include "nacl_absolute_time.h"
#include <ppapi/c/ppb_core.h>

#include <tyran_core/platform/nacl/nacl.h>

static PPB_Core* g_nacl_core = 0;

void nimbus_absolute_time_init(nimbus_absolute_time* a)
{
	if (!g_nacl_core) {
		g_nacl_core = (PPB_Core*) g_nacl.get_browser(PPB_CORE_INTERFACE);
	}
	a->absolute = g_nacl_core->GetTimeTicks();
}

double nimbus_absolute_time_delta(const nimbus_absolute_time* a, const nimbus_absolute_time* b)
{
	return a->absolute - b->absolute;
}
