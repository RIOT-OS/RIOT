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
 * @brief    CFG PAGE configuration object
 */
typedef struct cfg_page_desc {
  mtd_dev_t *dev;
  nanocbor_encoder_t writer;
  nanocbor_value_t   reader;
  uint8_t            active_page;  /* 0 or 1 */
  uint8_t            active_serialno;
} cfg_page_desc_t;

#define CFG_PAGE_HEADER_SIZE 16

/**
 * @brief Initialize the configuration page descriptor on @cpd object.
 *
 * This will initialize the cfg_page subsystem to start.
 * Typically, there is a global cfg_page_desc_t called cfgpage.
 *
 * The initialization system will validate the CRC of each of the two possible slots
 * and if both are correct, it will take the slot with the higher serialno.
 * Serial numbers go from 0 to 23, and then will wrap around to 0 after 23.
 *
 * @param[in]   cpd     the global context
 * @return              negative value for error, 0 on success
 */
extern int cfg_page_init(cfg_page_desc_t *cpd);

/**
 * @brief Validate a page of configuration variables. Normally does not need to be
 *        called, use cfg_page_init.
 *
 * This function will validate the CRC on the initial 16 byte header, and then
 * validate that all the magic numbers are in place.  It will return the decoded
 * serial number if successful, negative value if not.
 *
 * @param[in]   cpd     the global context
 * @param[in]   cfg_slot_no   which of two pages to validate
 * @return              The positive serial number (0->23) if valid. Negative if not.
 */
extern int cfg_page_validate(cfg_page_desc_t *cpd, int cfg_slot_no);

/**
 * @brief This function will intialize a page number with a header and serial number.
 *        It inserts a 12 byte header to identify the page as a configuration variable
 *        page, a one byte serial number and a CRC.  All encoded in CBOR.
 *        The total is 16 bytes of overhead.
 *
 * The page will then have an indefinite map opened, and then immediately ended with a stop
 * code.  The entire sector (typically 4k) will then be erased and this entire structure
 * written to flash.
 *
 * @param[in]   cpd           the global context
 * @param[in]   cfg_slot_no   which of two pages to write to.
 * @param[in]   serialno      the serial number to write to the page
 * @return                    0 on success, negative on error
 */
extern int cfg_page_format(cfg_page_desc_t *cpd, int cfg_slot_no, int serialno);

/**
 * @brief Dumb the contents of the active page to the console.
 *        This auxiliary function can be used in debugging.
 *
 * @param[in]   cpd           the global context
 * @return                    0 on success, negative on error
 */
extern int cfg_page_print(cfg_page_desc_t *cpd);

/**
 * @brief This function will intialize a nanocbor reader so that the keys and values
 *        can be read out.  This is a low-level interface.
 *
 * This function figures out which is the correct page to read, and then reads the
 * entire page in.  The decoder is also initialized.
 * While an arbirary page can be used, the module includes a static buffer that
 * is used internally.
 *
 * This function is not yet (prematurely) optimized to retain the loaded data across calls.
 *
 * @param[in]   cpd           the global context
 * @param[in]   cfg_page_buffer  some ram to keep the mapping
 * @param[in]   cfg_page_size    the size of the buffer
 * @param[out]  cfg_page_reader  a NANOCBOR reader to be initialized
 * @return                    0 on success, negative on error
 */
extern int cfg_page_init_reader(cfg_page_desc_t *cpd,
                                unsigned char *cfg_page_buffer, size_t cfg_page_size,
                                nanocbor_value_t *cfg_page_reader);

/**
 * @brief This function will find the value associated with a given integer key.
 *
 * It will intialize the provided NANOCBOR reader to process the value.
 * It will call cfg_page_init_reader to read all the values in.
 *
 * The last/most-recent value of the key will be returned
 *
 * @param[in]   cpd           the global context
 * @param[in]   wantedkey     the unsigned int key to retrieve
 * @param[out]  valuereader   a NANOCBOR reader to be initialized
 * @return                    0 on success, negative on error
 */
extern int cfg_page_get_value(cfg_page_desc_t *cpd,
                              uint32_t wantedkey,
                              nanocbor_value_t *valuereader);

/**
 * @brief This function will write a bstr value associated with a given integer key.
 *
 * This function will append a new value to the page.
 *
 * @param[in]   cpd           the global context
 * @param[in]   wantedkey     the unsigned int key to retrieve
 * @param[in]   strvalue      a buffer of bytes to write
 * @param[in]   valuereader   length of above buffer
 * @return                    0 on success, negative on error
 */
extern int cfg_page_set_str_value(cfg_page_desc_t *cpd,
                                  uint32_t newkey,
                                  const uint8_t *strvalue, size_t strlen);

/**
 * @brief This global is provided to describe the default set of cfg variables.
 *
 */
extern cfg_page_desc_t cfgpage;

#ifdef __cplusplus
}
#endif

#endif /* CFG_PAGE_H */
/** @} */
