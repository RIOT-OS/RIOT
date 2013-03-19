/**
 * Native Board config.h implementation
 *
 * No functionality implemented at the moment.
 *
 * Copyright (C) 2013 Ludwig Ortmann
 *
 * This file subject to the terms and conditions of the GNU General Public
 * License. See the file LICENSE in the top level directory for more details.
 *
 * @ingroup native_board
 * @{
 * @file
 * @author  Ludwig Ortmann <ludwig.ortmann@fu-berlin.de>
 * @}
 */

#include <stdio.h>
#include <stdint.h>

#include <config.h>

/**
 * XXX: loading not implemented
 */
void config_load(void)
{
    printf("XXX: config_load(): not implemented\n");
    return;
}

/**
 * XXX: storing not implemented
 */
uint8_t config_save(void)
{
    printf("XXX: config_save(): not implemented - your config will vanish on process termination\n");
    return 1;
}
