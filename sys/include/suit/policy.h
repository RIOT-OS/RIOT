/*
 * Copyright (C) 2019 Koen Zandberg
 *               2019 Kaspar Schleiser <kaspar@schleiser.de>
 *               2019 Inria
 *               2019 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#pragma once

/**
 * @ingroup     sys_suit
 * @brief       SUIT policy definitions
 *
 * @{
 *
 * @brief       SUIT policy definitions
 * @author      Koen Zandberg <koen@bergzand.net>
 * @author      Kaspar Schleiser <kaspar@schleiser.de>
 *
 */

#include <stddef.h>
#include <stdint.h>

#include "uuid.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name bitfield of required policies
 * @{
 */
#define SUIT_VALIDATED_AUTH         0x1     /**< currently unused           */
#define SUIT_VALIDATED_VERSION      0x2     /**< SUIT format version        */
#define SUIT_VALIDATED_SEQ_NR       0x4     /**< new seq nr > old seq nr    */
#define SUIT_VALIDATED_VENDOR       0x8     /**< vendor UUID matches        */
#define SUIT_VALIDATED_CLASS        0x10    /**< class UUID matches         */
#define SUIT_VALIDATED_DEVICE       0x20    /**< device UUID matches        */
/** @} */

/**
 * @brief   SUIT default policy
 */
#define SUIT_DEFAULT_POLICY \
    (SUIT_VALIDATED_VERSION | SUIT_VALIDATED_SEQ_NR | SUIT_VALIDATED_VENDOR | \
     SUIT_VALIDATED_CLASS)

#ifdef __cplusplus
}
#endif

/** @} */
