/* ========================================
 *
 * Copyright YOUR COMPANY, THE YEAR
 * All Rights Reserved
 * UNPUBLISHED, LICENSED SOFTWARE.
 *
 * CONFIDENTIAL AND PROPRIETARY INFORMATION
 * WHICH IS THE PROPERTY OF your company.
 *
 * ========================================
*/
#include <project.h>
#include <stdbool.h>
#include "thread.h"

/**
 * @brief Initializes RIOT.
 *
 * PSoC Creator creates some files which initialize PSoC devices and call main()
 * function. But RIOT also requires to call some initialization functions
 * before main() function and need to call main() function too.
 *
 * We cannot modify auto-generated functions by PSoC Creator so once it will
 * call main() function. This function must be called at the beginning of
 * main to start RIOT. For the first time, it will start RIOT and RIOT will call
 * main() function again. This function will not do anything for the second call
 * (call by RIOT) and will continue to execution.
 *
 * @retval true  : you should return from main() function
 * @retval false : do nothing and continue to application
 *
 */
bool initialize_RIOT(void)
{
    static bool riot_kernel_started = false;

    if (riot_kernel_started)
    {
        /* RIOT kernel was started before no need to do anything */
        return false;
    }
    else
    {
        /* For the first time, initialize board and RIOT kernel. */
        riot_kernel_started = true;

        /* Initialize Board */
        board_init();

        /* Initialize Kernel to start RIOT */
        kernel_init();

        return true;
    }
}

int main()
{
    CyGlobalIntEnable;

    /*
     * Initialize RIOT to start RIOT.
     *
     * Should be located at the beginning of main func
     */
    if (initialize_RIOT() == true)
    {
        return 0;
    }

    /* Application code */
}
