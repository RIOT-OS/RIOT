/* Inject some ugly workarounds needed for native on sched.h */

#if __GLIBC__
typedef void cpu_set_t;
#endif

#include_next "sched.h"
