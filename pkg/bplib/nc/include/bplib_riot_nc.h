/*
 * SPDX-FileCopyrightText: 2026 Hamburg University of Technology (TUHH)
 * SPDX-License-Identifier: LGPL-2.1-only
 */
#pragma once

#include "bplib.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Max sequence number, after which it wraps back to 0.
 */
#ifndef BPLIB_MAX_SEQ_NUM
#  define BPLIB_MAX_SEQ_NUM 1000000
#endif

typedef enum {
    BPLIB_PREVIOUS_NODE_BLOCK = 0,
    BPLIB_BUNDLE_AGE_BLOCK = 1,
    BPLIB_HOP_COUNT_BLOCK = 2,
    BPLIB_PAYLOAD_BLOCK = 3
} bplib_nc_canonical_block_t;

/**
 * @brief Initializes bplib's NC.
 *
 * NC means "Node Config" and contains pointers to the configuration tables.
 * This function sets the config point
 * Calls BPLib_NC_Init and returns
 *
 * @param ConfigPtrs [out] pointers in this will be set to the config
 * @return BPLIB_SUCCESS on success, forwarded from BPLib_NC_Init call.
 */
BPLib_Status_t bplib_riot_nc_init(BPLib_NC_ConfigPtrs_t* ConfigPtrs);

/**
 * @brief Set the hop limit of the channel.
 *
 * This only matters if the Hop Limit block is enabled,
 * using bplib_channel_set_block_include()
 *
 * @param channel Index of the channel
 * @param limit Hop limit
 * @return BPLIB_SUCCESS on success
 * @return BPLIB_INVALID_CHAN_ID_ERR if channel >= BPLIB_MAX_NUM_CHANNELS
 * @return BPLIB_APP_STATE_ERR if the channel / app is not in removed state
 */
BPLib_Status_t bplib_channel_set_hop_limit(
            uint32_t channel, uint8_t limit);

/**
 * @brief Set the CRC type of the primary block.
 *
 * @param channel Index of the channel
 * @param type CRC Type, see BPLib_CRC_Type_t. BPLib_CRC_Type_None is not supported here.
 * @return BPLIB_SUCCESS on success
 * @return BPLIB_INVALID_CHAN_ID_ERR if channel >= BPLIB_MAX_NUM_CHANNELS
 * @return BPLIB_APP_STATE_ERR if the channel / app is not in removed state
 * @return BPLIB_INVALID_CRC_ERROR if the CRC type is unknown
 */
BPLib_Status_t bplib_channel_set_crc_type(
            uint32_t channel, BPLib_CRC_Type_t type);

/**
 * @brief Set the local service number of the channel.
 *
 * Bundles to this service number can then be received with BPLib_PI_Egress()
 *
 * @param channel Index of the channel
 * @param service_no Local service number
 * @return BPLIB_SUCCESS on success
 * @return BPLIB_INVALID_CHAN_ID_ERR if channel >= BPLIB_MAX_NUM_CHANNELS
 * @return BPLIB_APP_STATE_ERR if the channel / app is not in removed state
 */
BPLib_Status_t bplib_channel_set_service_no(
            uint32_t channel, uint64_t service_no);

/**
 * @brief Set the bundle control flags of the primary block.
 *
 * Refer to RFC 9171, an example would be to prevent fragmentation or request reports.
 *
 * @param channel Index of the channel
 * @param flags Bundle control flags
 * @return BPLIB_SUCCESS on success
 * @return BPLIB_INVALID_CHAN_ID_ERR if channel >= BPLIB_MAX_NUM_CHANNELS
 * @return BPLIB_APP_STATE_ERR if the channel / app is not in removed state
 */
BPLib_Status_t bplib_channel_set_bundle_flags(
            uint32_t channel, uint64_t flags);

/**
 * @brief Set the lifetime of a bundle, in [ms].
 *
 * Bundles in transit for longer may / will be dropped by BP nodes.
 *
 * @param channel Index of the channel
 * @param lifetime Lifetime in ms
 * @return BPLIB_SUCCESS on success
 * @return BPLIB_INVALID_CHAN_ID_ERR if channel >= BPLIB_MAX_NUM_CHANNELS
 * @return BPLIB_APP_STATE_ERR if the channel / app is not in removed state
 */
BPLib_Status_t bplib_channel_set_lifetime(
            uint32_t channel, uint64_t lifetime);

/**
 * @brief Set the destination of bundles in the channel.
 *
 * @param channel Index of the channel
 * @param dest_eid Destination EID.
 * @return BPLIB_SUCCESS on success
 * @return BPLIB_INVALID_CHAN_ID_ERR if channel >= BPLIB_MAX_NUM_CHANNELS
 * @return BPLIB_APP_STATE_ERR if the channel / app is not in removed state
 */
BPLib_Status_t bplib_channel_set_dest_eid(
            uint32_t channel, BPLib_EID_t dest_eid);

/**
 * @brief Set the report-to node endpoint of bundles in the channel.
 *
 * Note: This may be set to BPLIB_EID_DTN_NONE for the null endpoint.
 *
 * @param channel Index of the channel
 * @param dest_eid Report-to EID.
 * @return BPLIB_SUCCESS on success
 * @return BPLIB_INVALID_CHAN_ID_ERR if channel >= BPLIB_MAX_NUM_CHANNELS
 * @return BPLIB_APP_STATE_ERR if the channel / app is not in removed state
 */
BPLib_Status_t bplib_channel_set_report_to_eid(
            uint32_t channel, BPLib_EID_t report_eid);

/**
 * @brief Set if a block should be part of the bundle.
 *
 * @note Wheather the BPLIB_BUNDLE_AGE_BLOCK is present depends on wheather
 *       an accurate DTN time can be provided, this function has no effect.
 *
 * @note The BPLIB_PAYLOAD_BLOCK can also not be turned off.
 *
 * @param channel Index of the channel
 * @param block Selector of a canonical block
 * @param include true to include the block, false to not include it.
 *                See the description for exceptions.
 * @return BPLIB_SUCCESS on success
 * @return BPLIB_INVALID_CHAN_ID_ERR if channel >= BPLIB_MAX_NUM_CHANNELS
 * @return BPLIB_APP_STATE_ERR if the channel / app is not in removed state
 * @return BPLIB_INVALID_CONFIG_ERR if the block parameter is invalid
 */
BPLib_Status_t bplib_channel_set_block_include(
            uint32_t channel, bplib_nc_canonical_block_t block, bool include);

/**
 * @brief Set the CRC type of a canonical block.
 *
 * @param channel Index of the channel
 * @param block Selector of a canonical block
 * @param type CRC type to use
 * @return BPLIB_SUCCESS on success
 * @return BPLIB_INVALID_CHAN_ID_ERR if channel >= BPLIB_MAX_NUM_CHANNELS
 * @return BPLIB_APP_STATE_ERR if the channel / app is not in removed state
 * @return BPLIB_INVALID_CONFIG_ERR if the block parameter is invalid
 * @return BPLIB_INVALID_CRC_ERROR if the CRC type is unknown
 */
BPLib_Status_t bplib_channel_set_block_crc_type(
            uint32_t channel, bplib_nc_canonical_block_t block, BPLib_CRC_Type_t type);

/**
 * @brief Set the block number of a canonical block.
 *
 * @note All canonical blocks have to have unique block numbers. Without a call of this
 *       function they default to 0.
 *
 * @note The payload block has to have block number 1, which is also not changeable
 *       with this function.
 *
 * @param channel Index of the channel
 * @param block Selector of a canonical block
 * @param type CRC type to use
 * @return BPLIB_SUCCESS on success
 * @return BPLIB_INVALID_CHAN_ID_ERR if channel >= BPLIB_MAX_NUM_CHANNELS
 * @return BPLIB_APP_STATE_ERR if the channel / app is not in removed state
 * @return BPLIB_INVALID_CONFIG_ERR if the block parameter is invalid
 */
BPLib_Status_t bplib_channel_set_block_num(
            uint32_t channel, bplib_nc_canonical_block_t block, uint32_t num);

/**
 * @brief Set the canonical block flags of a canonical block.
 *
 * Refer to RFC 9171 for details. For example one can specify that the block
 * has to be replicated in every fragment, or deleted when it cannot be processed.
 *
 * @param channel Index of the channel
 * @param block Selector of a canonical block
 * @param type CRC type to use
 * @return BPLIB_SUCCESS on success
 * @return BPLIB_INVALID_CHAN_ID_ERR if channel >= BPLIB_MAX_NUM_CHANNELS
 * @return BPLIB_APP_STATE_ERR if the channel / app is not in removed state
 * @return BPLIB_INVALID_CONFIG_ERR if the block parameter is invalid
 */
BPLib_Status_t bplib_channel_set_block_flags(
            uint32_t channel, bplib_nc_canonical_block_t block, uint64_t flags);

/**
 * @brief Set one reachable EID pattern for a contact.
 *
 * Each contact has BPLIB_MAX_CONTACT_DEST_EIDS slots for EID patterns it can reach.
 *
 * @param contact Index of the contact
 * @param index Index of the EID pattern slot.
 * @param eid_pat EID pattern, defining all EIDs that can be reached over this contact.
 * @return BPLIB_SUCCESS on success
 * @return BPLIB_INVALID_CONT_ID_ERR if contact >= BPLIB_MAX_NUM_CONTACTS
 * @return BPLIB_CLA_INCORRECT_STATE if the contact is not torn down
 * @return BPLIB_ERROR if index >= BPLIB_MAX_CONTACT_DEST_EIDS
 */
BPLib_Status_t bplib_contact_set_destinations(uint32_t contact,
            unsigned index, BPLib_EID_Pattern_t eid_pat);

/**
 * @brief Set the out address of a contact.
 *
 * What this address is depends on the CLA. For the UDP CLA this is an IPv6 address.
 * This is the address where bundles are sent to.
 *
 * @note The table only holds BPLIB_MAX_IP_LENGTH (including '\0'), which might need
 *       to be increased for other / external CLAs.
 *
 * @param contact Index of the contact
 * @param addr CLA destination address.
 * @param port CLA destination port
 * @return BPLIB_SUCCESS on success
 * @return BPLIB_INVALID_CONT_ID_ERR if contact >= BPLIB_MAX_NUM_CONTACTS
 * @return BPLIB_CLA_INCORRECT_STATE if the contact is not torn down
 */
BPLib_Status_t bplib_contact_set_out_addr(uint32_t contact,
            const char* addr, uint16_t port);

/**
 * @brief Set the in address of a contact.
 *
 * What this address is depends on the CLA. For the UDP CLA this is an IPv6 address.
 * This address may be meaningless depending on the CLA implementation, for UDP CLA
 * it is the address to listen on, to bind to.
 *
 * @note The table only holds BPLIB_MAX_IP_LENGTH (including '\0'), which might need
 *       to be increased for other / external CLAs.
 *
 * @param contact Index of the contact
 * @param addr CLA local address.
 * @param port CLA local port
 * @return BPLIB_SUCCESS on success
 * @return BPLIB_INVALID_CONT_ID_ERR if contact >= BPLIB_MAX_NUM_CONTACTS
 * @return BPLIB_CLA_INCORRECT_STATE if the contact is not torn down
 */
BPLib_Status_t bplib_contact_set_in_addr(uint32_t contact,
            const char* addr, uint16_t port);

#ifdef __cplusplus
}
#endif
