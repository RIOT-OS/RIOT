#include <stdio.h>

#include "lpm.h"

/* TODO: implement */
enum lpm_mode lpm_set(enum lpm_mode target)
{
    enum lpm_mode last_mode;

    /* dummy value as lpm is not currently implemented */
    last_mode = LPM_ON;

    return last_mode;
}


/* TODO: implement */
enum lpm_mode lpm_get()
{
    enum lpm_mode current_mode;

    /* dummy value as lpm is not currently implemented */
    current_mode = LPM_ON;

    return current_mode;
}
