#if defined TORNADO_OS_WINDOWS
#include <tyran_core/platform/windows/mutex.h>
#elif defined TORNADO_OS_IOS
#include "../../platform/ios/mutex.h"
#else
#include "../../platform/posix/nimbus_mutex.h"
#endif
