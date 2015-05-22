/*
 * Copyright (C) 2014 Freie Universität Berlin.
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @file
 * @author  Martine Lenders <mlenders@inf.fu-berlin.de>
 * @brief   Public declarations for the 6LoWPAN Border Router driver.
 */

#ifndef SIXLOWDRIVER_H
#define SIXLOWDRIVER_H

#include <stdint.h>

#include <sys/types.h>

#include <netinet/ip6.h>

/**
 * @brief   The maximum string length of an IPv6 address in text representation.
 */
#define IPV6_ADDR_LEN   40
#define IF_NAME_LEN     10              ///< Maximum length of an interface name
#define DEV_LEN         20              ///< Maximum length of a device filename

/**
 * @brief   Defines the format of the context information.
 */
typedef struct __attribute__((packed)) border_context_t {
    uint16_t version;       ///< Version of this context, send via the ABRO (s. draft-ietf-6lowpan-nd-17).
    uint8_t cid;            ///< The CID of the Context (s. draft-ietf-6lowpan-nd-17).
    struct in6_addr prefix; ///< The prefix this context defines.
    uint8_t len;            ///< The length of the prefix in bits.
    uint8_t comp;           ///< Flag to determine if Context is allowed for compression (s. draft-ietf-6lowpan-nd-17).
    uint16_t lifetime;      ///< Lifetime of this context (s. draft-ietf-6lowpan-nd-17).
} border_context_t;

/**
 * @brief   Initializes a TUN- or TAP-Interface.
 * @param[in,out]   dev     The name of the new interface. If the name
 *                          is not given, it is chosen by the kernel
 *                          and returned to the caller.
 * @param[in]       flags   Flags that signify the interface type
 *                          (<tt>IFF_TUN</tt> = TUN interface,
 *                           <tt>IFF_TAP</tt> = TAP interface,
 * @return  The file descriptor to the new interface.
 *                       <tt>IFF_NO_PI</tt> = provide no packet information).
 * @see     http://backreference.org/2010/03/26/tuntap-interface-tutorial/
 * @see     http://www.kernel.org/pub/linux/kernel/people/marcelo/linux-2.4/Documentation/networking/tuntap.txt
 *
 * Initializes a TUN- or TAP-Interface by the name <em>dev</em>. Which kind
 * of interface is defined by <em>flags</em>. The function returns the file
 * descriptor to the interface, which can be accessed via the
 * POSIX.1-2001 functions <tt>read()</tt> and <tt>write()</tt>
 */
int open_tun(char *dev, int flags);

/**
 * @brief Returns the file descriptor to the TUN interface.
 * @return  The file descriptor to the TUN interface initialized by
 *          open_tun()
 *
 * open_tun() needs to be called before. Otherwise the result of this
 * function may not make sense.
 */
int get_tun_fd(void);

/**
 * @brief   Sets the owner of a TUN- or TAP-Interface.
 * @param[in]   fd      The file descriptor to the interface to be changed.
 * @param[in]   uid     User ID of the new user to be assigned.
 * @param[in]   gid     Group ID of the new group to be assigned.
 * @return  0, if successful, -1 if not.
 */
int tun_set_owner(int fd, const uid_t *uid, const gid_t *gid);

/**
 * @brief   Adds a new IPv6 address to the TUN interface.
 * @param[in]   ip_addr    The new address.
 * @return  0, if <em>ip_addr</em> is a valid IPv6 address, -1, if not.
 */
int tun_add_addr(const char *ip_addr);

/**
 * @brief   Initializes the border router.
 * @param[in,out]   if_name The name of the new TUN interface. If the
 *                          name is not given, it is chosen by the
 *                          kernel and returned to the caller.
 * @param[in]       ip_addr The IPv6 Address, that is initially attached
 *                          to the LoWPAN Border Router.
 * @param[in]       tty_dev Device file of the serial interface the
 *                          MSB-A2 is attached to.
 * @return  0 if successfull,
 *          != 0 if an error occurs.
 */
int border_initialize(char *if_name, const char *ip_addr, const char *tty_dev);

/**
 * @brief   Sends an IPv6 datagram via the TUN interface.
 * @param[in]   fd      The file descriptor of the TUN interface
 * @param[in]   packet  The IPv6 datagram that is to be send via the
 *                      TUN interface and starts with an IPv6 header.
 *
 * The function uses the payload length field of the IPv6 Header to
 * determine the length of the overall packet. The payload bytes
 * <em>must</em> follow the header in memory.
 */
void border_send_ipv6_over_tun(int fd, const struct ip6_hdr *packet);

/**
 * @brief   Updates 6LoWPAN contex information.
 * @param[in]   cid         The context identifier, only values < 16
 *                          are allowed.
 * @param[in]   prefix      The prefix that shall be associated with the
 *                          <em>cid</em>.
 * @param[in]   len         The length of the <em>prefix</em> in bits.
 * @param[in]   comp        Flag to determine if Context is allowed for
 *                          compression (s. draft-ietf-6lowpan-nd-17).
 * @param[in]   lifetime    Lifetime of this context
 *                          (s. draft-ietf-6lowpan-nd-17).
 * @return If cid < 16 than it is 0, otherwise -1.
 */
int border_update_context(uint8_t cid, const struct in6_addr *prefix,
                          uint8_t len, uint8_t comp,
                          uint16_t lifetime);

/**
 * @brief   Updates 6LoWPAN contex information, as it is stored in the
 *          border router (e.g. to maintain a context which lifetime
 *          is about to run out).
 * @param[in]   cid The context identifier.
 * @return  0, if the context identifiet by cid is stored in the border
 *          router, -1 if not.
 *
 * The information taken for this update ist the last, that was used
 * with \ref border_update_context().
 */
int border_renew_existing_context(uint8_t cid);

/**
 * @brief   Removes 6LoWPAN context information.
 * @param[in]   cid The context identifier.
 *
 * The information taken for this update ist the last, that was used
 * with \ref border_update_context().
 */
void border_remove_context(uint8_t cid);

/**
 * @brief   Adds a new IPv6 address to the whole interface (TUN and
 *          sensor node).
 * @param[in]   ip_addr    The new address.
 * @return  0, if <em>ip_addr</em> is a valid IPv6 address, -1, if not.
 */
int border_add_addr(const char *ip_addr);


/**
 * @brief   Getter for the current version for ABROs send by this
 *          router.
 * @return  This border router's current version.
 */
uint16_t get_abro_version(void);

/**
 * @brief   Increments and updates the version for ABROs send by this
 *          router.
 * @return  This border router's new ABRO version.
 */
uint16_t get_next_abro_version(void);

#endif /* SIXLOWDRIVER_H*/
