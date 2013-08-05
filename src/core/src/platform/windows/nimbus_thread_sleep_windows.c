#include "../../base/thread/nimbus_thread_sleep.h"

#include <tyran_core/platform/windows/tyran_windows.h>

void nimbus_thread_sleep(tyran_number seconds)
{
	DWORD milliseconds = (DWORD)(seconds * 1000.0f);

	Sleep(milliseconds);
}
