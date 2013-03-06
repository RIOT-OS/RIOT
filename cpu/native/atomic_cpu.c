#include <atomic.h>
#include <cpu.h>
#include "debug.h"

unsigned int atomic_set_return(unsigned int* val, unsigned int set)
{
    unsigned int old_val;
    unsigned int old_state;

    DEBUG("atomic_set_return\n");

    old_state = disableIRQ();

    old_val = *val;
    *val = set;

    restoreIRQ(old_state);

    return old_val;
}
