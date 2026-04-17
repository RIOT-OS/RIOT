/*
 * SPDX-FileCopyrightText: 2026 Hamburg University of Technology (TUHH)
 * SPDX-License-Identifier: LGPL-2.1-only
 */
#pragma once
/**
 * @defgroup pkg_bplib_nc NC (Node Config) wrapper
 * @ingroup pkg_bplib
 * @brief A wrapper for the NC operations.
 *
 * # About
 * The NC contains the configuration of the node. In NASA's bpnode
 * implementations this is done through special tables, here we just use
 * static arrays for every configuration table.
 *
 * The arrays are hidden and exposed only via the setter functions in this
 * module, because, again, some members of configuration tables are not
 * implemented yet on the bplib side.
 *
 * The functions can only be called when a channel or contact is stopped.
 * This means the usual lifecycle of a channel / contact is as follows:
 *
 * 1. Configure the channel / contact with the functions in this module.
 * 2. Call BPLib_PI_AddApplication() or BPLib_CLA_ContactSetup() for a channel /
 *    contact. It can only be configured again (using functions in this module)
 *    once it has been completely torn down (step 5). Tear down from this state
 *    is possible as well.
 * 3. Call BPLib_PI_StartApplication() or BPLib_CLA_ContactStart() respectively.
 *    This makes the channel / contact active and bundles can flow through them.
 * 4. With BPLib_PI_StopApplication() or BPLib_CLA_ContactStop() respectively,
 *    the channel / contact can be stopped. This retains its configuration and
 *    it can be started again by step 3 if necessary. Bundles will not flow
 *    anymore. This is useful for
 *    contacts, when the configuration should stay the same (e.g. same network
 *    address to connect to), but the contact is currently disconnected.
 * 5. Call BPLib_PI_RemoveApplication() or BPLib_CLA_ContactTeardown(). This
 *    will tear down the channel / contact.
 *    Now goto 1 if the channel / contact  should be changed, for example when a
 *    new neighbor has been discovered.
 *
 * ## Optimizations
 *
 * bplib forces many tables in the NC which are not currently implemented.
 * These have been removed and allowed to be NULL inside of bplib. When the
 * features for these tables get implemented in bplib, and the port is reworked
 * they have to be added back.
 *
 * In NC, like in @ref pkg_bplib_fwp, there are more counter tables, which take
 * up several KB of space. These are disabled by default and can be enable
 * again with the `bplib_include_nc_telemetry` pseudomodule.
 *
 * @{
 *
 * @file
 * @brief       A wrapper for the NC operations.
 *
 * @author      Simon Grund <mail@simongrund.de>
 */

#include "bplib.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Max sequence number, after which it wraps back to 0.
 */
#ifndef CONFIG_BPLIB_MAX_SEQ_NUM
#  define CONFIG_BPLIB_MAX_SEQ_NUM 1000000
#endif

/**
 * @brief block type indicator for bplib_channel_set_block_*() functions
 */
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
 * This function sets the config pointers, calls BPLib_NC_Init and returns.
 *
 * @param[out] ConfigPtrs pointers in this will be set to the config
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
 * @retval BPLIB_SUCCESS on success
 * @retval BPLIB_INVALID_CHAN_ID_ERR if channel >= BPLIB_MAX_NUM_CHANNELS
 * @retval BPLIB_APP_STATE_ERR if the channel / app is not in removed state
 */
BPLib_Status_t bplib_channel_set_hop_limit(
            uint32_t channel, uint8_t limit);

/**
 * @brief Set the CRC type of the primary block.
 *
 * @param channel Index of the channel
 * @param type CRC Type, see BPLib_CRC_Type_t. BPLib_CRC_Type_None is not supported here.
 * @retval BPLIB_SUCCESS on success
 * @retval BPLIB_INVALID_CHAN_ID_ERR if channel >= BPLIB_MAX_NUM_CHANNELS
 * @retval BPLIB_APP_STATE_ERR if the channel / app is not in removed state
 * @retval BPLIB_INVALID_CRC_ERROR if the CRC type is unknown
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
 * @retval BPLIB_SUCCESS on success
 * @retval BPLIB_INVALID_CHAN_ID_ERR if channel >= BPLIB_MAX_NUM_CHANNELS
 * @retval BPLIB_APP_STATE_ERR if the channel / app is not in removed state
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
 * @retval BPLIB_SUCCESS on success
 * @retval BPLIB_INVALID_CHAN_ID_ERR if channel >= BPLIB_MAX_NUM_CHANNELS
 * @retval BPLIB_APP_STATE_ERR if the channel / app is not in removed state
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
 * @retval BPLIB_SUCCESS on success
 * @retval BPLIB_INVALID_CHAN_ID_ERR if channel >= BPLIB_MAX_NUM_CHANNELS
 * @retval BPLIB_APP_STATE_ERR if the channel / app is not in removed state
 */
BPLib_Status_t bplib_channel_set_lifetime(
            uint32_t channel, uint64_t lifetime);

/**
 * @brief Set the destination of bundles in the channel.
 *
 * @param channel Index of the channel
 * @param dest_eid Destination EID.
 * @retval BPLIB_SUCCESS on success
 * @retval BPLIB_INVALID_CHAN_ID_ERR if channel >= BPLIB_MAX_NUM_CHANNELS
 * @retval BPLIB_APP_STATE_ERR if the channel / app is not in removed state
 */
BPLib_Status_t bplib_channel_set_dest_eid(
            uint32_t channel, BPLib_EID_t dest_eid);

/**
 * @brief Set the report-to node endpoint of bundles in the channel.
 *
 * @note This may be set to BPLIB_EID_DTN_NONE for the null endpoint.
 *
 * @param channel Index of the channel
 * @param report_eid Report-to EID.
 * @retval BPLIB_SUCCESS on success
 * @retval BPLIB_INVALID_CHAN_ID_ERR if channel >= BPLIB_MAX_NUM_CHANNELS
 * @retval BPLIB_APP_STATE_ERR if the channel / app is not in removed state
 */
BPLib_Status_t bplib_channel_set_report_to_eid(
            uint32_t channel, BPLib_EID_t report_eid);

/**
 * @brief Set if a block should be part of the bundle.
 *
 * @note Whether the BPLIB_BUNDLE_AGE_BLOCK is present depends on weather
 *       an accurate DTN time can be provided, this function has no effect.
 *
 * @note The BPLIB_PAYLOAD_BLOCK can also not be turned off.
 *
 * @param channel Index of the channel
 * @param block Selector of a canonical block
 * @param include true to include the block, false to not include it.
 *                See the description for exceptions.
 * @retval BPLIB_SUCCESS on success
 * @retval BPLIB_INVALID_CHAN_ID_ERR if channel >= BPLIB_MAX_NUM_CHANNELS
 * @retval BPLIB_APP_STATE_ERR if the channel / app is not in removed state
 * @retval BPLIB_INVALID_CONFIG_ERR if the block parameter is invalid
 */
BPLib_Status_t bplib_channel_set_block_include(
            uint32_t channel, bplib_nc_canonical_block_t block, bool include);

/**
 * @brief Set the CRC type of a canonical block.
 *
 * @param channel Index of the channel
 * @param block Selector of a canonical block
 * @param type CRC type to use
 * @retval BPLIB_SUCCESS on success
 * @retval BPLIB_INVALID_CHAN_ID_ERR if channel >= BPLIB_MAX_NUM_CHANNELS
 * @retval BPLIB_APP_STATE_ERR if the channel / app is not in removed state
 * @retval BPLIB_INVALID_CONFIG_ERR if the block parameter is invalid
 * @retval BPLIB_INVALID_CRC_ERROR if the CRC type is unknown
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
 * @param num Block number to set
 * @retval BPLIB_SUCCESS on success
 * @retval BPLIB_INVALID_CHAN_ID_ERR if channel >= BPLIB_MAX_NUM_CHANNELS
 * @retval BPLIB_APP_STATE_ERR if the channel / app is not in removed state
 * @retval BPLIB_INVALID_CONFIG_ERR if the block parameter is invalid
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
 * @param flags Block flags to set
 * @retval BPLIB_SUCCESS on success
 * @retval BPLIB_INVALID_CHAN_ID_ERR if channel >= BPLIB_MAX_NUM_CHANNELS
 * @retval BPLIB_APP_STATE_ERR if the channel / app is not in removed state
 * @retval BPLIB_INVALID_CONFIG_ERR if the block parameter is invalid
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
 * @retval BPLIB_SUCCESS on success
 * @retval BPLIB_INVALID_CONT_ID_ERR if contact >= BPLIB_MAX_NUM_CONTACTS
 * @retval BPLIB_CLA_INCORRECT_STATE if the contact is not torn down
 * @retval BPLIB_ERROR if index >= BPLIB_MAX_CONTACT_DEST_EIDS
 */
BPLib_Status_t bplib_contact_set_destinations(uint32_t contact,
            unsigned index, BPLib_EID_Pattern_t eid_pat);

/**
 * @brief Set the out address of a contact.
 *
 * The type of address depends on the CLA. For the UDP CLA this is an IPv6 address.
 * This is the address where bundles are sent to.
 *
 * @note The table only holds BPLIB_MAX_IP_LENGTH (including '\0'), which might need
 *       to be increased for other / external CLAs.
 *
 * @param contact Index of the contact
 * @param addr CLA destination address.
 * @param port CLA destination port
 * @retval BPLIB_SUCCESS on success
 * @retval BPLIB_INVALID_CONT_ID_ERR if contact >= BPLIB_MAX_NUM_CONTACTS
 * @retval BPLIB_CLA_INCORRECT_STATE if the contact is not torn down
 */
BPLib_Status_t bplib_contact_set_out_addr(uint32_t contact,
            const char* addr, uint16_t port);

/**
 * @brief Set the in address of a contact.
 *
 * The type of address depends on the CLA. For the UDP CLA this is an IPv6 address.
 * This address may be meaningless depending on the CLA implementation, for UDP CLA
 * it is the address to listen on, to bind to.
 *
 * @note The table only holds BPLIB_MAX_IP_LENGTH (including '\0'), which might need
 *       to be increased for other / external CLAs.
 *
 * @param contact Index of the contact
 * @param addr CLA local address.
 * @param port CLA local port
 * @retval BPLIB_SUCCESS on success
 * @retval BPLIB_INVALID_CONT_ID_ERR if contact >= BPLIB_MAX_NUM_CONTACTS
 * @retval BPLIB_CLA_INCORRECT_STATE if the contact is not torn down
 */
BPLib_Status_t bplib_contact_set_in_addr(uint32_t contact,
            const char* addr, uint16_t port);

#ifdef __cplusplus
}
#endif

/** @} */
