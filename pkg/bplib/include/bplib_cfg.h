/*
 * NASA Docket No. GSC-19,559-1, and identified as "Delay/Disruption Tolerant Networking
 * (DTN) Bundle Protocol (BP) v7 Core Flight System (cFS) Application Build 7.0
 *
 * SPDX-FileCopyrightText: 2025 United States Government as represented by the Administrator of the
 * SPDX-FileCopyrightText: National Aeronautics and Space Administration.
 * SPDX-FileCopyrightText: 2026 Technische Universität Hamburg
 * SPDX-License-Identifier: Apache-2.0
 *
 * Modified version of [bplib]/inc/bplib_cfg.c
 */

#pragma once

#include "endian.h"

#ifdef __cplusplus
extern "C" {
#endif

/** Maximum length of an IP address string */
#ifndef BPLIB_MAX_IP_LENGTH
#  define BPLIB_MAX_IP_LENGTH               40
#endif

/** Maximum length for a generic string */
#ifndef BPLIB_MAX_STR_LENGTH
#  define BPLIB_MAX_STR_LENGTH              40
#endif

/** Maximum length of a bundle information string */
#ifndef BPLIB_MAX_BUNDLE_INFO_STR_LENGTH
#  define BPLIB_MAX_BUNDLE_INFO_STR_LENGTH  64
#endif


/** Maximum number of queues */
#ifndef BPLIB_MAX_NUM_BUNDLE_QUEUES
#  define BPLIB_MAX_NUM_BUNDLE_QUEUES           16
#endif

/* Maximum number of MIB sets of source configs and counters */
#ifndef BPLIB_MAX_NUM_MIB_SETS
#  define BPLIB_MAX_NUM_MIB_SETS                10
#endif

/** Maximum number of EID patterns that can map to a source MIB set index */
#ifndef BPLIB_MAX_NUM_EID_PATTERNS_PER_MIB_SET
#  define BPLIB_MAX_NUM_EID_PATTERNS_PER_MIB_SET 4
#endif

/** Maximum number of latency policy sets */
#ifndef BPLIB_MAX_NUM_LATENCY_POLICY_SETS
#  define BPLIB_MAX_NUM_LATENCY_POLICY_SETS     10
#endif

/** Maximum number of storage policy sets. */
#ifndef BPLIB_MAX_NUM_STORE_SET
#  define BPLIB_MAX_NUM_STORE_SET               10
#endif

/** Maximum number of Compressed Reporting CRS Entries */
#ifndef BPLIB_MAX_NUM_CRS
#  define BPLIB_MAX_NUM_CRS                     10
#endif

/** Maximum number of authorized source EID patterns */
#ifndef BPLIB_MAX_AUTH_SOURCE_EIDS
#  define BPLIB_MAX_AUTH_SOURCE_EIDS            10
#endif

/** Maximum number of authorized custodian EID patterns */
#ifndef BPLIB_MAX_AUTH_CUSTODIAN_EIDS
#  define BPLIB_MAX_AUTH_CUSTODIAN_EIDS         10
#endif

/** Maximum number of authorized custody source EID patterns */
#ifndef BPLIB_MAX_AUTH_CUSTODY_SOURCE_EIDS
#  define BPLIB_MAX_AUTH_CUSTODY_SOURCE_EIDS    10
#endif

/** Maximum number of report-to EID patterns */
#ifndef BPLIB_MAX_AUTH_REPORT_TO_EIDS
#  define BPLIB_MAX_AUTH_REPORT_TO_EIDS         10
#endif

/** Maximum number of EID patterns per storage policy set */
#ifndef BPLIB_MAX_NUM_STORE_EIDS
#  define BPLIB_MAX_NUM_STORE_EIDS              10
#endif

/** 
 * \brief Maximum number of contacts that can be running at once
 *          This drives the number of entries in the CLA configuration
 *          tables, as well as the number of CLA In/Out tasks in BPNode
 */
#ifndef BPLIB_MAX_NUM_CONTACTS
#  define BPLIB_MAX_NUM_CONTACTS                3
#endif

/**
 * \brief Maximum number of destination EID patterns per contact
 */
#ifndef BPLIB_MAX_CONTACT_DEST_EIDS
#  define BPLIB_MAX_CONTACT_DEST_EIDS           3
#endif

/** 
 * \brief Maximum number of channels that can be running at once
 *          This drives the number of entries in the channel and ADU proxy configuration
 *          tables, as well as the number of ADU In/Out tasks in BPNode
 */
#ifndef BPLIB_MAX_NUM_CHANNELS
#  define BPLIB_MAX_NUM_CHANNELS                2
#endif

/** 
 * \brief Maximum number of extension blocks per bundle.
 */
#ifndef BPLIB_MAX_NUM_EXTENSION_BLOCKS
#  define BPLIB_MAX_NUM_EXTENSION_BLOCKS        4
#endif

/**
 * \brief Maximum number of canonical blocks per bundle
 *        this is one more than BPLIB_MAX_NUM_EXTENSION_BLOCKS
 *        because it includes all extension blocks plus the payload block
 */
#ifndef BPLIB_MAX_NUM_CANONICAL_BLOCKS
#  define BPLIB_MAX_NUM_CANONICAL_BLOCKS        (BPLIB_MAX_NUM_EXTENSION_BLOCKS + 1)
#endif

/**
 * \brief This is the EID scheme for this instance of a DTN node
 */
#ifndef BPLIB_LOCAL_EID_SCHEME
#  define BPLIB_LOCAL_EID_SCHEME                BPLIB_EID_SCHEME_IPN
#endif

/**
 * \brief This is the EID IPN/SSP format for this instance of a DTN node
 */
#ifndef BPLIB_LOCAL_EID_IPN_SSP_FORMAT
#  define BPLIB_LOCAL_EID_IPN_SSP_FORMAT        BPLIB_EID_IPN_SSP_FORMAT_TWO_DIGIT
#endif

/**
 * \brief This is the EID allocator for this instance of a DTN node
 */
#ifndef BPLIB_LOCAL_EID_ALLOCATOR
#  define BPLIB_LOCAL_EID_ALLOCATOR             0
#endif

/**
 * \brief This is the EID node number for this instance of a DTN node
 */
#ifndef BPLIB_LOCAL_EID_NODE_NUM
#  define BPLIB_LOCAL_EID_NODE_NUM              100
#endif

/**
 * \brief This is the EID service number for this instance of a DTN node. This is not
 *        really used by BPLib, since local delivery sends bundles to other services on
 *        this node, but is included just in case a BPNode implementation does end up
 *        needing it.
 */
#ifndef BPLIB_LOCAL_EID_SERVICE_NUM
#  define BPLIB_LOCAL_EID_SERVICE_NUM           0
#endif

/**
 * \brief This reflects whether the system bplib is running on is big endian. Note that
 *        regardless of this, CBOR encoded bundles are big-endian
 */
#define BPLIB_SYS_BIG_ENDIAN                    (BYTE_ORDER == BIG_ENDIAN)

/**
 *  \brief This is the absolute maximum size a bundle is allowed to be.
 */
#ifndef BPLIB_MAX_BUNDLE_LEN
#  define BPLIB_MAX_BUNDLE_LEN                  17000
#endif

/**
 * \brief This is the absolute maximum size a bundle payload is allowed to be. The real
 *        max that channels will use is defined in the channel configurations but if
 *        those configurations have MaxBundlePayloadSize values that are larger than this
 *        value, the configurations will be rejected. This value must be smaller than
 *        \ref BPLIB_MAX_BUNDLE_LEN
 */
#ifndef BPLIB_MAX_PAYLOAD_SIZE
#  define BPLIB_MAX_PAYLOAD_SIZE                16384
#endif

/**
 * \brief This is the absolute maximum lifetime a bundle can have. Channel configurations
 *        that specify a lifetime greater than this value will be rejected and bundles
 *        received by Storage that have a lifetime greater than this value will have their
 *        functional lifetime truncated to this value.
 */
#ifndef BPLIB_MAX_LIFETIME_ALLOWED
#  define BPLIB_MAX_LIFETIME_ALLOWED            0xfffffffe
#endif


/**
 *  \brief This is the maximum retransmit time allowed in the contacts configuration (msec)
 */
#ifndef BPLIB_MAX_RETRANSMIT_ALLOWED
#  define BPLIB_MAX_RETRANSMIT_ALLOWED          600000
#endif

/**
 *  \brief This is the minimum retransmit time allowed in the contacts configuration (msec)
 */
#ifndef BPLIB_MIN_RETRANSMIT_ALLOWED
#  define BPLIB_MIN_RETRANSMIT_ALLOWED          1000
#endif

/**
 *  \brief This is the maximum CS time trigger allowed in the contacts configuration (msec)
 */
#ifndef BPLIB_MAX_CS_TIME_TRIGGER_ALLOWED
#  define BPLIB_MAX_CS_TIME_TRIGGER_ALLOWED     600000
#endif

/**
 *  \brief This is the minimum CS time trigger allowed in the contacts configuration (msec)
 */
#ifndef BPLIB_MIN_CS_TIME_TRIGGER_ALLOWED
#  define BPLIB_MIN_CS_TIME_TRIGGER_ALLOWED     1000
#endif

/**
 *  \brief This is the maximum CS size trigger allowed in the contacts configuration (bytes)
 * 
 * By default, we can only have a maximum of two bundle sequence collections per CCS and 
 * the length of their sequence range arrays is the upper limit on how big a CCS can get
 */
#ifndef BPLIB_MAX_CS_SIZE_TRIGGER_ALLOWED
#  define BPLIB_MAX_CS_SIZE_TRIGGER_ALLOWED     (BPLIB_MINIMUM_ENCODED_CCS_LEN + \
                            (BPLIB_CT_MAX_SEQ_RANGE_LEN * BPLIB_CT_MAX_SEQ_COLLECTIONS))
#endif

/**
 *  \brief This is the minimum CS size trigger allowed in the contacts configuration (bytes)
 */
#ifndef BPLIB_MIN_CS_SIZE_TRIGGER_ALLOWED
#  define BPLIB_MIN_CS_SIZE_TRIGGER_ALLOWED     BPLIB_MINIMUM_ENCODED_CCS_LEN
#endif

/**
 *  \brief Name of this entity. This should  unambiguously identify the node within 
 *         the network
 */
#ifndef BPLIB_SYSTEM_NODE_NAME
#  define BPLIB_SYSTEM_NODE_NAME                "BPLib on RIOT"
#endif

/**
 *  \brief Name of the primary manager of this node
 */
#ifndef BPLIB_SYSTEM_NODE_OWNER
#  define BPLIB_SYSTEM_NODE_OWNER               "RIOT Developer"
#endif

/**
 *  \brief Name of the underlying OS or executive controlling the resources of this node
 */
#define BPLIB_SYSTEM_SOFWARE_EXEC               "RIOT"

/**
 *  \brief Version of the software executive
 */
#define BPLIB_SYSTEM_SOFTWARE_EXEC_VERSION      RIOT_VERSION

/**
 *  \brief List of all CLAs currently supported by this node
 */
#define BPLIB_SUPPORTED_CLAS                    "(unknown)"

/**
 *  \brief Maximum number of bundle bytes allowed in storage at any given time
 */
#ifndef BPLIB_MAX_STORED_BUNDLE_BYTES
#  define BPLIB_MAX_STORED_BUNDLE_BYTES         ((size_t) 100000)  /* 100 KB */
#endif

/**
 *  \brief Whether to allow duplicate bundles in storage. This flag is recommended
 *         to be set to false unless needed otherwise for testing purposes. Note that
 *         bundle uniqueness is determined by a bundle's sequence number, source EID,
 *         and creation time.
 */
#ifndef BPLIB_ALLOW_DUPLICATE_BUNDLES
#  define BPLIB_ALLOW_DUPLICATE_BUNDLES         false
#endif

/**
 * \brief Maximum number of entries allowed in the Custody Transfer Database (CTDB)
 */
#ifndef BPLIB_CT_DB_MAX_ENTRIES
#  define BPLIB_CT_DB_MAX_ENTRIES                   (10000u)
#endif

// TODO
#ifndef BPLIB_ADMIN_RECORD_CRC_TYPE
#  define BPLIB_ADMIN_RECORD_CRC_TYPE               BPLib_CRC_Type_CRC16
#endif

// TODO
#ifndef BPLIB_ADMIN_RECORD_LIFETIME
#  define BPLIB_ADMIN_RECORD_LIFETIME               (3600000u)
#endif

// TODO
#ifndef BPLIB_ADMIN_RECORD_AGE_BLOCK_NUM
#  define BPLIB_ADMIN_RECORD_AGE_BLOCK_NUM          (2u)
#endif

// TODO
#ifndef BPLIB_ADMIN_RECORD_BLOCK_FLAGS
#  define BPLIB_ADMIN_RECORD_BLOCK_FLAGS            (0u)
#endif

/** 
 * \brief Egress queue depth
 *
 * This is one of the key configs that will affect performance -> the higher this value is,
 * the more bundles that can sit in memory for the Out task to egress while the maintenance
 * task focuses on loading bundles from storage into memory. The combination of this value,
 * and the total memory pool size provided by the user will 
 * determine the egress from storage performance.
**/
#ifndef BPLIB_QM_TX_QUEUE_DEPTH
#  define BPLIB_QM_TX_QUEUE_DEPTH               8
#endif

/* Note: BPLIB_STOR_LOADBATCHSIZE patched out and thus removed*/

#ifdef __cplusplus
} /* extern "C" */
#endif
