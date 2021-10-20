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
 *             43        # bytes(3)
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

#include "nanocbor/nanocbor.h"
#include "checksum/crc16_ccitt.h"
#include "cfg_page.h"
#include "od.h"

#define ENABLE_DEBUG 1
#include "debug.h"

/* for MTD_1 */
#include "board.h"

#define CBOR_SEQ_TAG      55800             /* per draft-ietf-cbor-file-magic */
#define CFG_PAGE_RIOT_TAG 1380536148        /* 'RIOT' = 0x52 0x49 0x4f 0x54 */

#ifndef CFG_PAGE_HEADER_SIZE
#define CFG_PAGE_HEADER_SIZE 16
#endif

int _calculate_slot_offset(unsigned int cfg_slot_no)
{
    int byte_offset = 0;

    if(cfg_slot_no == 0) {
        byte_offset = 0;
    } else if(cfg_slot_no == 1) {
        byte_offset = MTD_SECTOR_SIZE;
    } else {
        return -1;
    }
    return byte_offset;
}


int cfg_page_validate(cfg_page_desc_t *cpd, int cfg_slot_no)
{
    unsigned char header_buffer[CFG_PAGE_HEADER_SIZE];
    nanocbor_value_t decoder;
    uint32_t tagval = 0x12345678;
    uint16_t  checksum = 0;
    int8_t   serialno = 0;
    const uint8_t  *bytes   = NULL;
    size_t   bytes_len;
    unsigned int byte_offset = 0;

    if(cfg_slot_no == 0) {
        byte_offset = 0;
    } else if(cfg_slot_no == 1) {
        byte_offset = MTD_SECTOR_SIZE;
    } else {
        return -1;
    }

    /* read things in a specific block in first */
    if(mtd_read(cpd->dev, header_buffer, byte_offset, CFG_PAGE_HEADER_SIZE) != 0) {
        DEBUG("read failed\n");
        return -1;
    }

    //od_hex_dump_ext(header_buffer, CFG_PAGE_HEADER_SIZE, 16, 0);

    /* validate the checksum */
    nanocbor_decoder_init(&decoder, header_buffer, CFG_PAGE_HEADER_SIZE);

    /* pull in the tags */
    if(nanocbor_get_tag(&decoder, &tagval) != NANOCBOR_OK) {
        return -1;
    }

    if(tagval != CBOR_SEQ_TAG) {
        return -2;
    }

    if(nanocbor_get_tag(&decoder, &tagval) != NANOCBOR_OK) {
        return -3;
    }

    if(tagval != CFG_PAGE_RIOT_TAG) {
        return -4;
    }

    if(nanocbor_get_bstr(&decoder, &bytes, &bytes_len) != NANOCBOR_OK
       || bytes_len != 3
       || bytes[0]!= 'B'
       || bytes[1]!= 'O'
       || bytes[2]!= 'R') {
        return -5;
    }

    if(nanocbor_get_int8(&decoder, &serialno) < NANOCBOR_OK) {
        return -6;
    }

    /* calculate a 16-bit checksum across the bytes so far */
    uint16_t calculated = crc16_ccitt_calc(header_buffer, (decoder.cur - header_buffer));

    if(nanocbor_get_uint16(&decoder, &checksum) < NANOCBOR_OK) {
        return -7;
    }

    if(calculated != checksum) {
        DEBUG("slot:%d expected %04x got %04x\n", cfg_slot_no, calculated, checksum);
        return -8;
    }

    /* Good News Everyone! */
    return serialno;
}

int cfg_page_format(cfg_page_desc_t *cpd, int cfg_slot_no, int serialno)
{
    unsigned char header_buffer[CFG_PAGE_HEADER_SIZE+2];
    nanocbor_encoder_t encoder;
    unsigned int write_size=CFG_PAGE_HEADER_SIZE+2;

    nanocbor_encoder_init(&encoder, header_buffer, write_size);

    if(nanocbor_fmt_tag(&encoder, CBOR_SEQ_TAG) < 0) {
        return -1;
    }

    if(nanocbor_fmt_tag(&encoder, CFG_PAGE_RIOT_TAG) < 0) {
        return -1;
    }

    if(nanocbor_put_bstr(&encoder, (unsigned const char *)"BOR", 3) < 0) {
        return -1;
    }

    if(nanocbor_fmt_uint(&encoder, serialno) < NANOCBOR_OK) {
        return -6;
    }

    /* now calculate the CRC */
    /* calculate a 16-bit checksum across the bytes so far */
    uint16_t calculated = crc16_ccitt_calc(header_buffer, (encoder.cur - header_buffer));

    if(nanocbor_fmt_uint(&encoder, calculated) < NANOCBOR_OK) {
        return -7;
    }

    /* now initialize an indefinite map, and stop code */
    if(nanocbor_fmt_map_indefinite(&encoder) < 0) {
        return -8;
    }
    if(nanocbor_fmt_end_indefinite(&encoder) < 0) {
        return -9;
    }

    unsigned int byte_offset = _calculate_slot_offset(cfg_slot_no);

    /* read things in a specific block in first */
    DEBUG("writing %d bytes to slot_no: %d, at offset: %u\n", write_size,
          cfg_slot_no, byte_offset);

    //od_hex_dump_ext(header_buffer, write_size, 16, 0);

    int error = 0;
    /* erase the entire page */
    if((error = mtd_erase(cpd->dev, byte_offset, MTD_SECTOR_SIZE)) != 0) {
        DEBUG("erase failed: %d\n\n", error);
        return -10;
    }
    if((error = mtd_write(cpd->dev, header_buffer, byte_offset, write_size)) != 0) {
        DEBUG("write failed: %d\n", error);
        return -11;
    }

    DEBUG("formatted slot %u\n", cfg_slot_no);
    return 0;
}

int cfg_page_init_reader(cfg_page_desc_t *cpd,
                         unsigned char *cfg_page_buffer, size_t cfg_page_size,
                         nanocbor_value_t *cfg_page_reader)
{
    unsigned int byte_offset = _calculate_slot_offset(cpd->active_page);

    /* read in the whole page */
    if(mtd_read(cpd->dev, cfg_page_buffer, byte_offset, cfg_page_size) != 0) {
        DEBUG("read failed\n");
        return -1;
    }

    nanocbor_decoder_init(cfg_page_reader, cfg_page_buffer+CFG_PAGE_HEADER_SIZE,
                          cfg_page_size - CFG_PAGE_HEADER_SIZE);

    return 0;
}

static unsigned char cfg_page_active_buffer[MTD_PAGE_SIZE];
int cfg_page_get_value(cfg_page_desc_t *cpd, uint32_t wantedkey, nanocbor_value_t *valuereader)
{
    nanocbor_value_t reader;
    nanocbor_value_t values;
    int ret = 0;

    if(cfg_page_init_reader(cpd, cfg_page_active_buffer, sizeof(cfg_page_active_buffer), &reader) < 0) {
        return -1;
    }

    if(nanocbor_get_type(&reader) != NANOCBOR_TYPE_MAP ||
       nanocbor_enter_map(&reader, &values) != NANOCBOR_OK) {
        return -2;
    }

    while(!nanocbor_at_end(&values)) {
        uint32_t key = 0;
        if(nanocbor_get_uint32(&values, &key) < 0) {
            DEBUG("non interger key value found: %d", nanocbor_get_type(&values));
            return -3;
        }

        if(key == wantedkey) {
            /* found a key, but might not be last one */
            if(valuereader) {
                /* make copy of reader so that caller can read out value */
                *valuereader = values;
                ret = 1;
            }
        }

        /* skip the map value */
        nanocbor_skip(&values);
    }
    return ret;
}


int cfg_page_init(cfg_page_desc_t *cpd)
{
    DEBUG("cfg_page: init\n");
#ifdef MTD_1
    cpd->dev = MTD_1;
    mtd_init(cpd->dev);
#endif

    int slot0_serial = cfg_page_validate(&cfgpage, 0);
    int slot1_serial = cfg_page_validate(&cfgpage, 1);
    cpd->active_page = 0;

    if(slot0_serial < 0 && slot1_serial < 0) {
        DEBUG("Formatting cfg page %u\n", cpd->active_page);
        cfg_page_format(&cfgpage, cpd->active_page, 0);
    } else if(slot0_serial <  0  && slot1_serial >= 0) {
        cpd->active_page = 1;
    } else if(slot0_serial >= 0  && slot1_serial < 0) {
        cpd->active_page = 0;
    } else if(slot0_serial >= slot1_serial) {
        cpd->active_page = 0;
    } else {
        cpd->active_page = 1;
    }

    DEBUG("Using cfg page %u\n", cpd->active_page);

    /*
     * now setup with a nanocbor writer, pointing at the end of the
     * active page
     */

    return 0;
}

