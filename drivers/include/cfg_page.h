/*
 * Copyright (C) 2021 Michael Richardson <mcr@sandelman.ca>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    cfg_page
 * @ingroup     notsure
 * @brief       Driver Configuration Pages
 *
 * @{
 *
 * @file
 * @brief       Public interface for CFG Pages driver
 * @author      Michael Richardson <mcr@sandelman.ca>
 */

#ifndef CFG_PAGE_H
#define CFG_PAGE_H

#include "mtd.h"
#include "nanocbor/nanocbor.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name    CFG page configuration
 * @{
 */
typedef struct cfg_page_desc {
  mtd_dev_t *dev;
  nanocbor_encoder_t writer;
  nanocbor_value_t   reader;
  uint8_t            active_page;  /* 0 or 1 */
  uint8_t            active_serialno;
} cfg_page_desc_t;

#define CFG_PAGE_HEADER_SIZE 16

extern int cfg_page_init(cfg_page_desc_t *cpd);
extern int cfg_page_validate(cfg_page_desc_t *cpd, int cfg_slot_no);
extern int cfg_page_format(cfg_page_desc_t *cpd, int cfg_slot_no, int slotno);
extern int cfg_page_print(cfg_page_desc_t *cpd);
extern int cfg_page_init_reader(cfg_page_desc_t *cpd,
                                unsigned char *cfg_page_buffer, size_t cfg_page_size,
                                nanocbor_value_t *cfg_page_reader);
extern int cfg_page_get_value(cfg_page_desc_t *cpd,
                              uint32_t wantedkey,
                              nanocbor_value_t *valuereader);
extern int cfg_page_set_str_value(cfg_page_desc_t *cpd,
                                  uint32_t newkey,
                                  const uint8_t *strvalue, size_t strlen);

extern cfg_page_desc_t cfgpage;

#ifdef __cplusplus
}
#endif

#endif /* CFG_PAGE_H */
/** @} */
