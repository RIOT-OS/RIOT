/*
 * SPDX-FileCopyrightText: 2026 Technische Universität Hamburg
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @defgroup pkg_bplib_util 
 * @ingroup pkg_bplib
 * @brief TODO
 *
 * ## TODO
 *
 * @{
 *
 * @file
 * @brief       TODO
 *
 * @author      Simon Grund <mail@simongrund.de>
 */

// TODO search for all TODOs in this file

#include "bplib.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Max string buffer size of an IPN format EID
 *
 * Example of a max. length 2 digit IPN string: "ipn:18446744073709551615.18446744073709551615"
 * Both the node number and the service number can be 64 bit numbers.
 *
 * Example of a max. length 3 digit IPN string: "ipn:4294967295.4294967295.18446744073709551615"
 * The node number is split in the less significant 32 bit and the more significant
 * 32 bits. The least significant 32 bits represent the node number, the others
 * represent the allocator.
 */
#define DTN_EID_IPN_MAX_SIZE    47

/**
 * @brief Convert bplib EID to string
 *
 * @param[in] eid EID to convert
 * @param[out] str String buffer. Should be >= DTN_EID_IPN_MAX_SIZE bytes long.
 * @retval true on success, if the EID was valid
 * @retval false otherwise
 */
bool bplib_util_eid2str(BPLib_EID_t* eid, char* str);

/**
 * @brief Convert bplib EID pattern to two EIDs 
 *
 * An EID pattern holds a range of EIDs used for routing.
 *
 * @param[in] pat The EID pattern to convert
 * @param[out] eid1 The lower bound EID. May be NULL if not of interest.
 * @param[out] eid2 The upper bound EID. May be NULL if not of interest.
 */
void bplib_util_eids_from_pattern(BPLib_EID_Pattern_t* pat, BPLib_EID_t* eid1, BPLib_EID_t* eid2);

#ifdef __cplusplus
}
#endif

/** @} */
