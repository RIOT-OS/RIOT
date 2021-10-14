/*
 * Copyright (C) 2021  Michael Richardson <mcr@sandelman.ca>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     drivers_cfg_page
 * @brief       configuration data flash page
 * @{
 *
 * @file
 * @brief       configuration page access routines
 *
 * @author      Michael Ricahrdson <mcr@sandelman.ca>
 *
 * A Configuration Page consists of a fixed 16 byte header, followed by an indefinite map.
 * The header is a CBOR sequence with 3 elements:
 *    1) A CBOR-file-magic SEQUENCE with tag "RIOT"
 *       D9 D9F7         # tag(55799)
 *          DA 52494F54  # tag(1380536148)
 *             63        # text(3)
 *               424F52  # "BOR"
 *    2) A single byte sequence counter, mod 24 (so 0 > 24)
 *       01 # unsigned(1)
 *    3) A 16-bit checksum of the previous 13 bytes.
 *       19 3345 # unsigned(13125)
 *    4) An indefinite map containing key/values.
 * @}
 */

#include <errno.h>
#include <stdlib.h>

#include "cfg_page.h"

#define ENABLE_DEBUG 1
#include "debug.h"

/* for MTD_1 */
#include "board.h"

int cfg_page_init(struct cfg_page_desc_t *cpd)
{
    DEBUG("cfg_page: init\n");
#ifdef MTD_1
    cpd->dev = MTD_1;
    mtd_init(cpd->dev);
#endif

    return 0;
}

#define CBOR_SEQ_TAG      55800             /* per draft-ietf-cbor-file-magic */
#define CFG_PAGE_RIOT_TAG 1380536148        /* 'RIOT' */

int cfg_page_validate(struct cfg_page_desc_t *cpd)
{
  unsigned char header_buffer[CFG_PAGE_HEADER_SIZE];
  nanocbor_value_t decoder;
  uint32_t tagval = 0x12345678;
  int16_t  checksum = 0;
  int8_t   serialno = 0;

  /* read things in first */
  if(mtd_read(cpd->dev, header_buffer, 0, sizeof(CFG_PAGE_HEADER_SIZE)) != 0) {
    return -1;
  }

  /* validate the checksum */
  nanocbor_decoder_init(&decoder, header_buffer, CFG_PAGE_HEADER_SIZE);

  /* pull in the tags */
  if(nanocbor_get_tag(&decoder, &tagval) != NANOCBOR_OK
     || tagval != CBOR_SEQ_TAG) {
    return -1;
  }

  if(nanocbor_get_tag(&decoder, &tagval) != NANOCBOR_OK
     || tagval != CFG_PAGE_RIOT_TAG) {
    return -1;
  }

  if(nanocbor_get_bstr(&val, &bytes, &bytes_len) != NANOCBOR_OK
     || bytes_len != 3
     || bytes[0]!= 'B'
     || bytes[1]!= 'O'
     || bytes[2]!= 'R') {
    return -1;
  }

  if(nanocbor_get_int8(&val, &serialno) != NANOCBOR_OK) {
    return -1;
  }

  /* calculate a 16-bit checksum across the bytes so far */
  int16_t calculated = crc16_ccitt_calc(header_buffer, (val->cur - header_buffer));

  if(nanocbor_get_int16(&val, &checksum) != NANOCBOR_OK) {
    return -1;
  }

  if(calculated != checksum) {
    return -1;
  }

  /* Good News Everyone! */
  return 0;
}
