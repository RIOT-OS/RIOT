/*
 * Copyright (C) 2015  INRIA.
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @file    dev_uid.h
 * @brief   Definitions for providing unique device ID.
 *
 * @author  Kévin Roussel <Kevin.Roussel@inria.fr>
 */

#ifndef DEV_UID_H
#define DEV_UID_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

#include "dev_uid_cpu.h"


/** Type definition for a device unique ID */
typedef struct {
    uint8_t byte[DEV_UID_SIZE];   // DEV_UID_SIZE defined per-cpu
} dev_uid_t;


/** Constant pointer to the device's unique, immutable, long ID. */
extern const dev_uid_t const * dev_uid;

/**
 * Utility function to pretty-print the given device UID
 * (using stdio's standard @c printf() function).
 * 
 * @param[in]  uid  Device Unique ID to print.
 */
void print_dev_uid(const dev_uid_t const * uid);

/**
 * @return A short device ID, computed from the device unique ID @c dev_uid.
 */
uint16_t get_short_dev_id(void);


#ifdef __cplusplus
}
#endif

#endif /* DEV_UID_H */

