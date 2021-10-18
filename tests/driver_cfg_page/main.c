/*
 * Copyright (C) 2021 Michael Richardson <mcr@sandelman.ca>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup tests
 * @{
 *
 * @file
 * @brief       Test application for the CFG-PAGE driver
 *
 * @author      Michael Richardson <mcr@sandelman.ca>
 *
 * @}
 */

#include <stdio.h>
#include <string.h>
#include <ctype.h>

#include "board.h"
#include "cfg_page.h"

//static char cfg_page_temp[MTD_PAGE_SIZE];

int main(void)
{
    puts("CFG-PAGE test application starting...");

    cfg_page_init(&cfgpage, 0);

    if(cfg_page_validate(&cfgpage) == 0) {
      puts("succeed\n");
    } else {
      puts("failed\n");
    }
}
