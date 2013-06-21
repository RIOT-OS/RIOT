#include <atomic.h>
#include <cpu.h>

unsigned int atomic_set_return(unsigned int *val, unsigned int set)
{
    dINT();
    unsigned int old_val = *val;
    *val = set;
    eINT();
    return old_val;
}
