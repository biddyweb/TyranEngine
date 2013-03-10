#if defined TORNADO_OS_WINDOWS
#include "../platform/windows/mutex.h"
#else
#include "../../platform/posix/nimbus_mutex.h"
#endif
