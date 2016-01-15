#if defined TORNADO_OS_WINDOWS
#include <tyran_core/platform/windows/absolute_time.h>
#elif defined TORNADO_OS_IOS
#include "../../platform/ios/ios_absolute_time.h"
#elif defined TORNADO_OS_NACL
#include "../../platform/nacl/nacl_absolute_time.h"
#else
#include "../../platform/posix/posix_absolute_time.h"
#endif
