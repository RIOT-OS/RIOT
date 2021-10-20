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
struct cfg_page_desc_t {
  mtd_dev_t *dev;
  nanocbor_encoder_t writer;
  uint8_t            active_page;  /* 0 or 1 */
};

extern int cfg_page_init(struct cfg_page_desc_t *cpd);
extern int cfg_page_validate(struct cfg_page_desc_t *cpd, int cfg_slot_no);
extern int cfg_page_format(struct cfg_page_desc_t *cpd, int cfg_slot_no, int slotno);
extern struct cfg_page_desc_t cfgpage;

#ifdef __cplusplus
}
#endif

#endif /* CFG_PAGE_H */
/** @} */
