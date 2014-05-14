/*
 * Copyright (C) 2014 Martin Lenders
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License. See the file LICENSE in the top level directory for more
 * details.
 */

#include "unittests.h"

#include "lpm.h"

int main(void)
{
#ifdef OUTPUT
    TextUIRunner_setOutputter(OUTPUTTER);
#endif

    TESTS_START();

    /* put test TEST_RUN() calls here: */
    /*     #ifdef TEST_xxx_ENABLED
     *         tests_xxx();
     *     #endif
     */

    TESTS_END();

    lpm_set(LPM_POWERDOWN);
    return 0;
}
