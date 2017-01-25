/*
 * Copyright (C) 2016 Leon George
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup         cpu_cc26x0_definitions
 * @{
 *
 * @file
 * @brief           CC26x0 Hard API definitions
 */

#ifndef CC26x0_HAPI_H
#define CC26x0_HAPI_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * HAPI function table
 */
typedef struct {
    void *crc32;
    void *flashGetSize;
    void *getChipId;
    void *_reserved1;
    void *_reserved2;
    void *_reserved3;
    void *resetDevice;
    void *fletcher32;
    void *minValue;
    void *maxValue;
    void *meanValue;
    void *standDeviationValue;
    void *_reserved4;
    void *_reserved5;
    void *HFSourceSafeSwitch;
    void *selectCompAInput;
    void *selectCompARef;
    void *selectADCCompBInput;
    void *selectCompBRef;
} hapi_table_t;

/** @ingroup cpu_specific_peripheral_memory_map
  * @{
  */
#define HAPI_TABLE_ADDR          0x10000048 /**< base address of HAPI table */
/*@}*/

#define HAPI                     ((hapi_table_t *)(HAPI_TABLE_ADDR)) /**< Hard API function table */


#ifdef __cplusplus
}
#endif

#endif /* CC26x0_HAPI_H */

/*@}*/
