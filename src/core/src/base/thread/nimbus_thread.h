#include <tyranscript/tyran_types.h>

#if defined TORNADO_OS_WINDOWS
#include "windows/thread.h"
#else
#include "../../platform/posix/nimbus_thread.h"
#endif
