// assert_stub.c
#include <stdint.h>

/* Some C files are calling the function 'assert' directly,
   so we give them something to link against: */
void assert(int cond)
{
    (void)cond;
    /* if you want to trap on a failed assert, replace the body with:
         if (!cond) { while(1); }
       or hook up your own panic handler here.
    */
}
