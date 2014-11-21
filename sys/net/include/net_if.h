/*
 * Copyright (C) 2013  Freie Universität Berlin.
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    net_if Network interfaces
 * @brief       Abstraction layer between transceiver module and L3 protocol
 *              implementations.
 * @ingroup     net
 *
 * @{
 *
 * @file        net_if.h
 * @brief       Types and functions for network interfaces
 * @author      Martin Lenders <mlenders@inf.fu-berlin.de>
 */
#ifndef _NET_IF_H
#define _NET_IF_H

#include <stdint.h>
#include <stdlib.h>

#include "mutex.h"
#include "transceiver.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   type to specify types of upper layer addresses
 */
typedef uint8_t net_if_l3p_t;

/**
 * @brief   Interface protocols (for net_if_t.protocols): Use raw packets with
 *          static addresses in upper layer.
 */
#define NET_IF_L3P_RAW          (0x00)

/**
 * @brief   Interface protocols (for net_if_t.protocols): Use unicast IPv6
 *          address in upper layer, addr_len must be 128.
 */
#define NET_IF_L3P_IPV6_UNICAST (0x01)

/**
 * @brief   Interface protocols (for net_if_t.protocols): Use multicast IPv6
 *          address in upper layer, addr_len must be 128.
 */
#define NET_IF_L3P_IPV6_MULTICAST (0x02)

/**
 * @brief   Interface protocols (for net_if_t.protocols): Use anycast IPv6
 *          address in upper layer, addr_len must be 128.
 */
#define NET_IF_L3P_IPV6_ANYCAST (0x04)

/**
 * @brief   Interface protocols (for net_if_t.protocols): Use IPv6 prefix in
 *          upper layer, addr_len <= 128 becomes prefix length.
 */
#define NET_IF_L3P_IPV6_PREFIX  (0x08)

/**
 * @brief   Interface protocols (for net_if_t.protocols): Convenience macro
 *          combining NET_IF_L3P_IPV6_UNICAST, NET_IF_L3P_IPV6_ANYCAST, and
 *          NET_IF_L3P_IPV6_MULTICAST for comparisons
 */
#define NET_IF_L3P_IPV6_ADDR    (NET_IF_L3P_IPV6_UNICAST | NET_IF_L3P_IPV6_ANYCAST \
                                | NET_IF_L3P_IPV6_MULTICAST)

/**
 * @brief   Interface protocols (for net_if_t.protocols): Convenience macro
 *          combining NET_IF_L3P_IPV6_UNICAST, NET_IF_L3P_IPV6_ANYCAST,
 *          NET_IF_L3P_IPV6_MULTICAST, and NET_IF_L3P_IPV6_PREFIX for
 *          comparisons
 */
#define NET_IF_L3P_IPV6         (NET_IF_L3P_IPV6_UNICAST | NET_IF_L3P_IPV6_ANYCAST \
                                | NET_IF_L3P_IPV6_MULTICAST | NET_IF_L3P_IPV6_PREFIX)

/**
 * @brief   Interface protocols: Return value of net_if_get_l3p_types() on
 *          error.
 */
#define NET_IF_L3P_FAILURE      (0x80)

#ifndef NET_IF_MAX
/**
 * @brief   Maximum number of interfaces. Redefinable via compiler flag.
 */
#define NET_IF_MAX      (1)
#endif

/**
 * @brief Data type to represent an EUI-64.
 */
typedef union __attribute__((packed)) {
    uint8_t uint8[8];   ///< split into 8 8-bit words.
    uint16_t uint16[4]; ///< split into 4 16-bit words.
    uint32_t uint32[2]; ///< split into 2 32-bit words.
    uint64_t uint64;    ///< as one 64-bit word.
} net_if_eui64_t;

/**
 * @brief   list type for upper layer address of an interface.
 *
 * @details The interpretation of the address data is left to the upper layer
 *          implementations.
 */
typedef struct __attribute__((packed)) net_if_addr_t {
    /**
     * @brief The next address on the interface. Initialise with NULL
     */
    struct net_if_addr_t *addr_next;

    /**
     * @brief The next address on the interface. Initialise with NULL
     */
    struct net_if_addr_t *addr_prev;

    /**
     * @brief Flags to define upper layer protocols this address applies to
     */
    net_if_l3p_t addr_protocol;
    void *addr_data;                    ///< The actual upper layer address
    uint8_t addr_len;                   ///< Length of the upper layer address in bit.
} net_if_addr_t;

typedef uint8_t net_if_trans_addr_m_t;

/**
 * @brief   Interface type.
 */
typedef struct {
    uint8_t initialized;                ///< Detemines if interface is initialized
    uint8_t protocols;                  ///< Interface L3 protocols
    transceiver_type_t transceivers;    ///< Transceivers to use with this interface
    net_if_trans_addr_m_t trans_src_addr_m; ///< Transceiver address mode
    mutex_t address_buffer_mutex;       ///< Mutex for address buffer operations
    net_if_addr_t *addresses;           ///< Adresses
    uint8_t l3p_data[9];                ///< generic L3 data
} net_if_t;

#define NET_IF_TRANS_ADDR_M_SHORT  2    ///< Transceiver address mode for short addresses
#define NET_IF_TRANS_ADDR_M_LONG   3    ///< Transceiver address mode for long addresses

/**
 * All registered interfaces.
 */
extern net_if_t interfaces[NET_IF_MAX];

/**
 * @brief   Initializes the module.
 */
void net_if_init(void);

/**
 * @brief   Inititializes a new interface
 *
 * @pre     *transceivers* may not be zero.
 *
 * @param[in] protocols     The upper layer protocols to use on this interface.
 * @param[in] transceivers  The transceivers this interface uses.
 *
 * @return  The new interface's ID on success, -1 on failure.
 */
int net_if_init_interface(net_if_l3p_t protocols,
                          transceiver_type_t transceivers);

/**
 * @brief   Get interface.
 *
 * @param[in] if_id The interface's ID
 *
 * @return  The interface identified by *if_id* or NULL on failure.
 */
static inline net_if_t *net_if_get_interface(int if_id)
{
    if (if_id < NET_IF_MAX && interfaces[if_id].initialized) {
        return &interfaces[if_id];
    }
    else {
        return NULL;
    }
}


/**
 * @brief   Iterates over all intitialized interfaces
 *
 * @param[in] start  Return value of last iteration step. -1 to start iteration.
 *
 * @return  ID of an initialized interface. -1 if end of interface list is
 *          reached.
 */
int net_if_iter_interfaces(int start);

/**
 * @brief   Sets the source address mode for the interface
 *
 * @param[in] if_id Interface to set source address mode for.
 * @param[in] mode  The mode to set to.
 *
 * @return  1 on success, 0 on error
 */
static inline int net_if_set_src_address_mode(int if_id,
        net_if_trans_addr_m_t mode)
{
    if (!interfaces[if_id].initialized) {
        return 0;
    }

    interfaces[if_id].trans_src_addr_m = mode;
    return 1;
}

/**
 * @brief   Gets the source address mode for the interface
 *
 * @param[in] if_id Interface to get source address mode from.
 *
 * @return  The interfaces address mode, 0 on error
 */
static inline net_if_trans_addr_m_t net_if_get_src_address_mode(int if_id)
{
    if (!interfaces[if_id].initialized) {
        return 0;
    }

    return interfaces[if_id].trans_src_addr_m;
}

/**
 * @brief   Adds new address to interface
 *
 * @pre *addr* is not NULL, *addr->addr_data* is not NULL
 *
 * @param[in] if_id     The interface's ID
 * @param[in] addr      The address to add
 *
 * @return  1 on success, 0 on failure.
 */
int net_if_add_address(int if_id, net_if_addr_t *addr);

/**
 * @brief   Removes first occurance of address from interface
 *
 * @pre *addr* is not NULL, *addr->addr_data* is not NULL
 *
 * @param[in] if_id     The interface's ID
 * @param[in] addr      The address to remove
 *
 * @return  1 on success (and if given address is not registered to this
 *          interface), 0 on failure
 */
int net_if_del_address(int if_id, net_if_addr_t *addr);

/**
 * @brief   Iterates over registered addresses of an interface.
 *
 * @param[in] if_id     The interface's ID
 * @param[in,out] addr  The previous address as in or the next address as out.
 *                      If *addr* points to NULL it will be set to the
 *                      first address assigned to *if_id*, if *addr* points to
 *                      NULL as out, the last address assigned to *if_id* was
 *                      given as *addr* previously (and the address list was
 *                      completely iterated).
 *
 * @return  The pointer *addr* refers to after call of this function or NULL on
 *          error
 */
net_if_addr_t *net_if_iter_addresses(int if_id, net_if_addr_t **addr);

/**
 * @brief   Get the upper layer protocol types assigned to the interface *if_id*
 *
 * @param[in] if_id The interface's ID
 * @return  The upper layer protocol types assigned to the interface *if_id* on
 *          success, NET_IF_L3P_FAILURE on failure.
 */
net_if_l3p_t net_if_get_l3p_types(int if_id);

/**
 * @brief   Add an upper layer protocol types to the interface *if_id*
 *
 * @param[in] if_id     The interface's ID
 * @param[in] protocols The upper layer protocol types to assign to the
 *                      interface *if_id*
 * @return  1 on success, 0 on failure.
 */
int net_if_add_l3p_types(int if_id, net_if_l3p_t protocols);

/**
 * @brief   Remove upper layer protocol types and all addresses of this scope
 *          from the interface *if_id*
 *
 * @param[in] if_id     The interface's ID
 * @param[in] protocols The upper layer protocol types to be removed from the
 *                      interface *if_id*
 * @return  1 on success, 0 on failure.
 */
int net_if_del_l3p_types(int if_id, net_if_l3p_t protocols);

/**
 * @brief   Sends a packet to a short address over the interface.
 *
 * @pre     Transceivers has to be initialized and transceiver thread has
 *          to be started.
 *
 * @param[in] if_id         The interface's ID.
 * @param[in] target        The target's short transceiver address.
 * @param[in] packet_data   The packet to send
 * @param[in] packet_len    The length of the packet's data in byte, negative
 *                          number on error.
 *
 * @return The number of bytes send on success, negative value on failure
 */
int net_if_send_packet(int if_id, uint16_t target, const void *packet_data,
                       size_t packet_len);

/**
 * @brief   Sends a packet to a long address over the interface. If transceiver
 *          only supports smaller addresses the least significant bit of the
 *          address will be taken.
 *
 * @pre     Transceivers has to be initialized and transceiver thread has
 *          to be started.
 *
 * @param[in] if_id         The interface's ID.
 * @param[in] target        The target's long transceiver address.
 * @param[in] packet_data   The packet to send
 * @param[in] packet_len    The length of the packet's data in byte, negative
 *                          number on error.
 *
 * @return The number of bytes send on success, negative value on failure
 */
int net_if_send_packet_long(int if_id, net_if_eui64_t *target,
                            const void *packet_data, size_t packet_len);

/**
 * @brief   Sends a packet over all initialized interfaces.
 *
 * @pre     Transceivers has to be initialized and transceiver thread has
 *          to be started.
 *
 * @param[in] preferred_dest_mode   The preferred transceiver address mode for
 *                                  the destination broadcast address. Choose
 *                                  NET_IF_TRANS_ADDR_M_SHORT if you are not
 *                                  sure
 * @param[in] packet_data           The packet to send
 * @param[in] packet_len            The length of the packet's data in byte,
 *                                  negative number on error.
 *
 * @return The number of bytes send on success, negative value on failure
 */
int net_if_send_packet_broadcast(net_if_trans_addr_m_t preferred_dest_mode,
                                 const void *payload, size_t payload_len);

/**
 * @brief register a thread for events an interface's transceiver
 * @details This function just wraps transceiver_register().
 *
 * @pre     Transceivers has to be initialized and transceiver thread has
 *          to be started.
 *
 * @param[in] if_id     The transceiver's interface to register for
 * @param[in] pid       The pid of the thread to register
 *
 * @return              1 on success, 0 otherwise
 */
int net_if_register(int if_id, kernel_pid_t pid);

/**
 * Returns the EUI-64 of the transeivers attached to this interface. This can
 * be get by the actual EUI-64 if the transceiver has one or a generated one
 * based of the hardware address
 *
 * @pre     Transceivers of this interface has to be initialized and
 *          transceiver thread has to be started.
 *
 * @see <a href="http://tools.ietf.org/html/rfc4944#section-6">
 *          RFC 4944, section 5
 *      </a>
 * @see <a href="http://tools.ietf.org/search/rfc6282#section-3.2.2">
 *          RFC 6282, section 3.2.2
 *      </a>
 *
 * @param[out] eui64            The EUI-64 to fill
 * @param[in]  if_id            The interface's ID
 * @param[in]  force_generation Force generation from a short address if the
 *                              hardware supports it, even if the hardware
 *                              supplies an EUI-64
 *
 * @return  1 on success, 0 on failure
 */
int net_if_get_eui64(net_if_eui64_t *eui64, int if_id, int force_generation);

/**
 * @brief   Parses a string to an EUI-64.
 * @detail  The parsing will be back to front, every non-hexadecimal character
 *          and every hexadecimal character beyond the count of 8 will be
 *          ignored
 *
 * @param[out] eui64        The generated binary EUI-64.
 * @param[in]  eui64_str    A hexadecimal number in string representation.
 */
void net_if_hex_to_eui64(net_if_eui64_t *eui64, const char *eui64_str);

/**
 * Returns the address of the transceiver associated with the given interface.
 *
 * @pre     Transceivers of this interface has to be initialized and
 *          transceiver thread has to be started.
 *
 * @param[in]  if_id    The interface's ID
 *
 * @return  The transceiver's hardware address on success, 0 on failure
 */
uint16_t net_if_get_hardware_address(int if_id);

/**
 * Returns the EUI-64 of the transeivers attached to this interface. This can
 * be get by the actual EUI-64 if the transceiver has one or a generated one
 * based of the hardware address
 *
 * @pre     Transceivers of this interface has to be initialized and
 *          transceiver thread has to be started.
 *
 * @param[in]  if_id            The interface's ID
 * @param[in] eui64             The new EUI-64
 *
 * @return  1 on success, 0 on failure
 */
int net_if_set_eui64(int if_id, net_if_eui64_t *eui64);

/**
 * Sets the address of the transceiver associated with the given interface.
 *
 * @pre     Transceivers of this interface has to be initialized and
 *          transceiver thread has to be started.
 *
 * @param[in]  if_id    The interface's ID
 * @param[in]  addr     The new hardware address
 *
 * @return  the new hardware address on success, 0 on failure.
 */
uint16_t net_if_set_hardware_address(int if_id, uint16_t addr);

/**
 * Returns the channel of the transceiver associated with the given interface.
 *
 * @pre     Transceivers of this interface has to be initialized and
 *          transceiver thread has to be started.
 *
 * @param[in]  if_id    The interface's ID
 *
 * @return  The transceiver's frequency channel on success, -1 on failure.
 */
int32_t net_if_get_channel(int if_id);

/**
 * Sets the channel of the transceiver associated with the given interface.
 *
 * @pre     Transceivers of this interface has to be initialized and
 *          transceiver thread has to be started.
 *
 * @param[in]  if_id    The interface's ID
 * @param[in]  channel  The new frequency channel
 *
 * @return  the new channel on success, -1 on failure.
 */
int32_t net_if_set_channel(int if_id, uint16_t channel);

/**
 * Returns the PAN ID of the transceiver associated with the given interface.
 *
 * @pre     Transceivers of this interface has to be initialized and
 *          transceiver thread has to be started.
 *
 * @param[in]  if_id    The interface's ID
 *
 * @return  The transceiver's PAN ID on success, -1 on failure
 */
int32_t net_if_get_pan_id(int if_id);

/**
 * Sets the PAN ID of the transceiver associated with the given interface.
 *
 * @pre     Transceivers of this interface has to be initialized and
 *          transceiver thread has to be started.
 *
 * @param[in]  if_id    The interface's ID
 * @param[in]  pan_id   The new frequency channel
 *
 * @return  the PAN ID on success, -1 on failure.
 */
int32_t net_if_set_pan_id(int if_id, uint16_t pan_id);

#ifdef __cplusplus
}
#endif

/**
 * @}
 */
#endif /* _NET_IF_H */
