/*
 * Copyright (C) 2021 Michael Richardson <mcr@sandelman.ca>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#include <stdio.h>

#include "mtd.h"
#include "cfg_page.h"
#include "od.h"
#include "board.h"

static char cfg_page_temp[MTD_PAGE_SIZE];

int cfgpage_print_cmd(int argc, char **argv)
{
  (void)argc;
  (void)argv;
  if(mtd_read(cfgpage.dev, cfg_page_temp, 0, MTD_PAGE_SIZE) != 0) {
    return -1;
  }

  od_hex_dump_ext(cfg_page_temp, MTD_PAGE_SIZE, 16, 0);

    return 0;
}
