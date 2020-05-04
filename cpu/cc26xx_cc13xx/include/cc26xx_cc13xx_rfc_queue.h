/*
 * Copyright (C) 2020 Locha Inc
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup         cpu_cc26xx_cc13xx
 * @{
 *
 * @file
 * @brief           CC26xx/CC13xx RF Core queue management
 *
 * @author          Jean Pierre Dudey <jeandudey@hotmail.com>
 */

#ifndef CC26XX_CC13XX_RFC_QUEUE_H
#define CC26XX_CC13XX_RFC_QUEUE_H

#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Data queue
 */
typedef struct {
    uint8_t *curr_entry; /**< Pointer to the data entry to be used */
    uint8_t *last_entry; /**< Last entry of the queue, NULL is queue is
                              circular */
} __attribute__ ((aligned (4))) rfc_data_queue_t;

/**
 * @brief   Data entry status
 */
typedef enum {
    RFC_DATA_ENTRY_PENDING = 0, /**< Entry not yet used */
    RFC_DATA_ENTRY_ACTIVE = 1, /**< Entry in use by radio CPU */
    RFC_DATA_ENTRY_BUSY = 2, /**< Entry being updated */
    RFC_DATA_ENTRY_FINISHED = 3, /**< Radio CPU is finished accessing the
                                      entry */
    RFC_DATA_ENTRY_UNFINISHED = 4 /**< Radio CPU is finished accessing the
                                       entry, but packet could not be
                                       finished */
} rfc_data_entry_status_t;

/**
 * @brief   Data entry type
 */
typedef enum {
    RFC_DATA_ENTRY_TYPE_GEN  = 0, /**< General type: Tx entry or single element
                                       Rx entry */
    RFC_DATA_ENTRY_TYPE_MULTI = 1, /**< Multi-element Rx entry type */
    RFC_DATA_ENTRY_TYPE_PTR = 2, /**< Pointer entry type */
    RFC_DATA_ENTRY_TYPE_PARTIAL = 3 /**< Partial read entry type */
} rfc_data_entry_type_t;

/**
 * @brief   Data entry configuration
 */
typedef struct {
    uint8_t type:2; /**< Entry type */
    uint8_t lensz:2; /**< Length size for this entry */
    uint8_t irq_intv:4; /**< Reserved */
} rfc_data_entry_config_t;

/**
 * @brief   Data entry
 */
typedef struct {
   uint8_t* next_entry; /**< Pointer to next entry in the queue */
   uint8_t status; /**< Indicates status of entry, including whether it is free
                        for the system CPU to write to */
   rfc_data_entry_config_t config; /**< Data entry configuration */
   uint16_t length; /**< For pointer entries: Number of bytes in the data buffer
                         pointed to. For other entries: Number of bytes
                         following this length field */
} __attribute__ ((aligned (4))) rfc_data_entry_t;

/**
 * @brief   Data entry general
 */
typedef struct {
   uint8_t* next_entry; /**< Pointer to next entry in the queue */
   uint8_t status; /**< Indicates status of entry, including whether it is free
                        for the system CPU to write to */
   rfc_data_entry_config_t config; /**< Data entry configuration */
   uint16_t length; /**< For pointer entries: Number of bytes in the data buffer
                         pointed to. For other entries: Number of bytes
                         following this length field */
   uint8_t data; /**< First byte of the data array to be received or
                      transmitted */
} __attribute__ ((aligned (4))) rfc_data_entry_general_t;

#ifdef __cplusplus
} /* end of extern "C" */
#endif

#endif /* CC26XX_CC13XX_RFC_QUEUE_H */

/*@}*/
