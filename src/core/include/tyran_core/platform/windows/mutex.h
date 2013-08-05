#ifndef nimbus_tornado_mutex_h
#define nimbus_tornado_mutex_h

#include "tyran_windows.h"

#define nimbus_mutex CRITICAL_SECTION

#define nimbus_mutex_init(handle) InitializeCriticalSection(handle);
#define nimbus_mutex_destroy(handle) DeleteCriticalSection(handle);
#define nimbus_mutex_lock(handle) EnterCriticalSection(handle);
#define nimbus_mutex_unlock(handle) LeaveCriticalSection(handle);

#endif
