/*
 * SPDX-FileCopyrightText: 2022 Gunar Schorcht
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#define PTHREAD_CANCEL_DISABLE 1
/*
 * This is a dummy function to avoid undefined references when linking
 * against newlib and module pthread is not used.
 */
int pthread_setcancelstate(int state, int *oldstate)
{
    if (oldstate) {
        *oldstate = PTHREAD_CANCEL_DISABLE;
    }
    return 0;
}
