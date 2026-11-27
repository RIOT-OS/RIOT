/*
 * NASA Docket No. GSC-19,559-1, and identified as "Delay/Disruption Tolerant Networking
 * (DTN) Bundle Protocol (BP) v7 Core Flight System (cFS) Application Build 7.0
 *
 * SPDX-FileCopyrightText: 2025 United States Government as represented by the Administrator of the
 * SPDX-FileCopyrightText: National Aeronautics and Space Administration.
 * SPDX-FileCopyrightText: 2026 Technische Universität Hamburg
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

/**
 * @defgroup pkg_bplib_configopts Configuration Options 'bplib_cfg.h'
 * @ingroup pkg_bplib
 * @brief Configuration options
 *
 * ## About
 * Configuration options of the `[bplib]/inc/bplib_cfg.h` file
 *
 * All defines in this module can be overwritten by CFLAGS definitions.
 *
 *
 * @{
 *
 * @file
 * @brief       Configuration options
 *
 * @author      Simon Grund <mail@simongrund.de>
 */

#include "endian.h"

#ifdef __cplusplus
extern "C" {
#endif

/** @brief Maximum length of an IP address string */
#ifndef BPLIB_MAX_IP_LENGTH
#  define BPLIB_MAX_IP_LENGTH               40
#endif

/** @brief Maximum length for a generic string */
#ifndef BPLIB_MAX_STR_LENGTH
#  define BPLIB_MAX_STR_LENGTH              40
#endif

/**
 * @brief Maximum length of a bundle information string
 *
 * This is used for buffers to the BPLib_BI_GetBundleInfo() function
 */
#ifndef BPLIB_MAX_BUNDLE_INFO_STR_LENGTH
#  define BPLIB_MAX_BUNDLE_INFO_STR_LENGTH  64
#endif

/* BPLIB_MAX_NUM_BUNDLE_QUEUES was removed due to being unused */

/**
 * @brief Maximum number of MIB sets of source configs and counters
 *
 * Used only for Admin Statistics. Has no effect if AS is not enabled.
 */
#ifndef BPLIB_MAX_NUM_MIB_SETS
#  define BPLIB_MAX_NUM_MIB_SETS                10
#endif

/**
 * @brief Maximum number of EID patterns that can map to a source MIB set index
 *
 * Used only for Admin Statistics. Has no effect if AS is not enabled.
 */
#ifndef BPLIB_MAX_NUM_EID_PATTERNS_PER_MIB_SET
#  define BPLIB_MAX_NUM_EID_PATTERNS_PER_MIB_SET 4
#endif

/** Unused but bplib expects the define */
#define BPLIB_MAX_NUM_LATENCY_POLICY_SETS     10

/** Unused but bplib expects the define */
#define BPLIB_MAX_NUM_STORE_SET               10

/** Unused but bplib expects the define */
#define BPLIB_MAX_NUM_CRS                     10

/** Unused but bplib expects the define */
#define BPLIB_MAX_AUTH_SOURCE_EIDS            10

/** Unused but bplib expects the define */
#define BPLIB_MAX_AUTH_CUSTODIAN_EIDS         10

/** Unused but bplib expects the define */
#define BPLIB_MAX_AUTH_CUSTODY_SOURCE_EIDS    10

/** Unused but bplib expects the define */
#define BPLIB_MAX_AUTH_REPORT_TO_EIDS         10

/** Unused but bplib expects the define */
#define BPLIB_MAX_NUM_STORE_EIDS              10

/**
 * @brief Maximum number of contacts that can be running at once
 *
 * This drives the number of entries in the CLA configuration tables
 */
#ifndef BPLIB_MAX_NUM_CONTACTS
#  define BPLIB_MAX_NUM_CONTACTS                2
#endif

/**
 * @brief Maximum number of destination EID patterns per contact
 */
#ifndef BPLIB_MAX_CONTACT_DEST_EIDS
#  define BPLIB_MAX_CONTACT_DEST_EIDS           3
#endif

/**
 * @brief Maximum number of channels that can be running at once
 *
 * This drives the number of entries in the channel configuration tables
 */
#ifndef BPLIB_MAX_NUM_CHANNELS
#  define BPLIB_MAX_NUM_CHANNELS                2
#endif

/**
 * @brief Maximum number of extension blocks per bundle
 *
 * bplib currently supports 4 types of extension blocks, to use them all this
 * value should be 4. If you are certain that less are needed, this can be
 * reduced to reduce the in-memory and in-storage size of a bundle.
 */
#ifndef BPLIB_MAX_NUM_EXTENSION_BLOCKS
#  define BPLIB_MAX_NUM_EXTENSION_BLOCKS        4
#endif

/**
 * @brief Maximum number of canonical blocks per bundle
 *
 * This is one more than BPLIB_MAX_NUM_EXTENSION_BLOCKS
 * because it includes all extension blocks plus the payload block.
 */
#define BPLIB_MAX_NUM_CANONICAL_BLOCKS          (BPLIB_MAX_NUM_EXTENSION_BLOCKS + 1)

/**
 * @brief This is the EID scheme for this instance of a DTN node
 *
 * Currently only BPLIB_EID_SCHEME_IPN is supported.
 */
#ifndef BPLIB_LOCAL_EID_SCHEME
#  define BPLIB_LOCAL_EID_SCHEME                BPLIB_EID_SCHEME_IPN
#endif

/**
 * @brief This is the EID IPN/SSP format for this instance of a DTN node.
 *
 * Currently only BPLIB_EID_IPN_SSP_FORMAT_TWO_DIGIT is supported.
 */
#ifndef BPLIB_LOCAL_EID_IPN_SSP_FORMAT
#  define BPLIB_LOCAL_EID_IPN_SSP_FORMAT        BPLIB_EID_IPN_SSP_FORMAT_TWO_DIGIT
#endif

/**
 * @brief This is the EID allocator for this instance of a DTN node
 *
 * Since only BPLIB_EID_IPN_SSP_FORMAT_TWO_DIGIT is supported, this has to be 0.
 */
#ifndef BPLIB_LOCAL_EID_ALLOCATOR
#  define BPLIB_LOCAL_EID_ALLOCATOR             0
#endif

/** @brief This is the EID node number for this instance of a DTN node */
#ifndef BPLIB_LOCAL_EID_NODE_NUM
#  define BPLIB_LOCAL_EID_NODE_NUM              100
#endif

/**
 * @brief This is the EID service number for this instance of a DTN node.
 *
 * This is unused as it is defined per channel.
 */
#ifndef BPLIB_LOCAL_EID_SERVICE_NUM
#  define BPLIB_LOCAL_EID_SERVICE_NUM           0
#endif

/**
 * @brief This reflects whether the system bplib is running on is big endian.
 *
 * @note Regardless of this, CBOR encoded bundles are big-endian
 */
#define BPLIB_SYS_BIG_ENDIAN                    (BYTE_ORDER == BIG_ENDIAN)

/**
 * @brief This is the absolute maximum size a bundle is allowed to be.
 *
 * This is used for validation only; bundles larger that this will be rejected early.
 */
#ifndef BPLIB_MAX_BUNDLE_LEN
#  define BPLIB_MAX_BUNDLE_LEN                  4500
#endif

/**
 * @brief This is the absolute maximum size a bundle payload is allowed to be.
 *
 * Must be smaller than @ref BPLIB_MAX_BUNDLE_LEN.
 *
 * This option controls the size of the CBOR encoding buffer. In order to reduce
 * the memory footprint, this can be reduced.
 */
#ifndef BPLIB_MAX_PAYLOAD_SIZE
#  define BPLIB_MAX_PAYLOAD_SIZE                4096
#endif

/**
 * @brief This is the absolute maximum lifetime [ms] a bundle can have.
 *
 * Channel configurations that specify a lifetime greater than this value will be
 * rejected and bundles received by Storage that have a lifetime greater than this
 * value will have their functional lifetime truncated to this value.
 */
#ifndef BPLIB_MAX_LIFETIME_ALLOWED
#  define BPLIB_MAX_LIFETIME_ALLOWED            0xfffffffe
#endif

/** @brief This is the maximum retransmit time allowed in the contacts configuration [ms] */
#ifndef BPLIB_MAX_RETRANSMIT_ALLOWED
#  define BPLIB_MAX_RETRANSMIT_ALLOWED          600000
#endif

/** @brief This is the minimum retransmit time allowed in the contacts configuration [ms] */
#ifndef BPLIB_MIN_RETRANSMIT_ALLOWED
#  define BPLIB_MIN_RETRANSMIT_ALLOWED          1000
#endif

/** @brief This is the maximum CS time trigger allowed in the contacts configuration [ms] */
#ifndef BPLIB_MAX_CS_TIME_TRIGGER_ALLOWED
#  define BPLIB_MAX_CS_TIME_TRIGGER_ALLOWED     600000
#endif

/** @brief This is the minimum CS time trigger allowed in the contacts configuration [ms] */
#ifndef BPLIB_MIN_CS_TIME_TRIGGER_ALLOWED
#  define BPLIB_MIN_CS_TIME_TRIGGER_ALLOWED     1000
#endif

/**
 * @brief This is the maximum CS size trigger allowed in the contacts configuration [bytes]
 *
 * By default, we can only have a maximum of two bundle sequence collections per CCS and
 * the length of their sequence range arrays is the upper limit on how big a CCS can get.
 */
#ifndef BPLIB_MAX_CS_SIZE_TRIGGER_ALLOWED
#  define BPLIB_MAX_CS_SIZE_TRIGGER_ALLOWED     (BPLIB_MINIMUM_ENCODED_CCS_LEN + \
                            (BPLIB_CT_MAX_SEQ_RANGE_LEN * BPLIB_CT_MAX_SEQ_COLLECTIONS))
#endif

/** @brief This is the minimum CS size trigger allowed in the contacts configuration [bytes] */
#ifndef BPLIB_MIN_CS_SIZE_TRIGGER_ALLOWED
#  define BPLIB_MIN_CS_SIZE_TRIGGER_ALLOWED     BPLIB_MINIMUM_ENCODED_CCS_LEN
#endif

/**
 * @brief Name of this entity. This should unambiguously identify the node within the network
 *
 * Only used when Admin Statistics are used.
 */
#ifndef BPLIB_SYSTEM_NODE_NAME
#  define BPLIB_SYSTEM_NODE_NAME                "BPLib on RIOT"
#endif

/**
 * @brief Name of the primary manager of this node
 *
 * Only used when Admin Statistics are used.
 */
#ifndef BPLIB_SYSTEM_NODE_OWNER
#  define BPLIB_SYSTEM_NODE_OWNER               "RIOT Developer"
#endif

/**
 * @brief Name of the underlying OS or executive controlling the resources of this node
 *
 * Only used when Admin Statistics are used.
 */
#define BPLIB_SYSTEM_SOFWARE_EXEC               "RIOT"

/**
 * @brief Version of the software executive
 *
 * Only used when Admin Statistics are used.
 */
#define BPLIB_SYSTEM_SOFTWARE_EXEC_VERSION      RIOT_VERSION

/**
 * @brief List of all CLAs currently supported by this node
 *
 * Only used when Admin Statistics are used.
 */
#define BPLIB_SUPPORTED_CLAS                    "(unknown)"

/**
 * @brief Maximum number of bundle bytes allowed in storage at any given time
 */
#ifndef BPLIB_MAX_STORED_BUNDLE_BYTES
#  define BPLIB_MAX_STORED_BUNDLE_BYTES         ((size_t) 100000UL)  /* 100 KB */
#endif

/**
 * @brief Whether to allow duplicate bundles in storage.
 *
 * This flag is recommended to be set to false unless needed otherwise for testing purposes.
 */
#ifndef BPLIB_ALLOW_DUPLICATE_BUNDLES
#  define BPLIB_ALLOW_DUPLICATE_BUNDLES         false
#endif

/** @brief Maximum number of entries allowed in the Custody Transfer Database (CTDB) */
#ifndef BPLIB_CT_DB_MAX_ENTRIES
#  define BPLIB_CT_DB_MAX_ENTRIES               (10u)
#endif

/** @brief CRC type to use for generated admin records */
#ifndef BPLIB_ADMIN_RECORD_CRC_TYPE
#  define BPLIB_ADMIN_RECORD_CRC_TYPE           BPLib_CRC_Type_CRC16
#endif

/** @brief Lifetime [ms] to use for generated admin records */
#ifndef BPLIB_ADMIN_RECORD_LIFETIME
#  define BPLIB_ADMIN_RECORD_LIFETIME           (3600000u)
#endif

/** @brief Block number of the 'Age Block' used for generated admin records */
#ifndef BPLIB_ADMIN_RECORD_AGE_BLOCK_NUM
#  define BPLIB_ADMIN_RECORD_AGE_BLOCK_NUM      (2u)
#endif

/** @brief Additional bundle flags to used for generated admin records */
#ifndef BPLIB_ADMIN_RECORD_BLOCK_FLAGS
#  define BPLIB_ADMIN_RECORD_BLOCK_FLAGS        (0u)
#endif

/**
 * @brief Egress queue depth
 *
 * Number of elements in the egress queues per channel and per contact. In RIOT,
 * this is likely bottlenecked by the size of the memory pool anyways.
 */
#ifndef BPLIB_QM_TX_QUEUE_DEPTH
#  define BPLIB_QM_TX_QUEUE_DEPTH               8
#endif

/* Note: BPLIB_STOR_LOADBATCHSIZE patched out and thus removed */

#ifdef __cplusplus
} /* extern "C" */
#endif

/** @} */
