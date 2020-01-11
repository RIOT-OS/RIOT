#ifndef NET_GNRC_IPV4_ARP_H
#define NET_GNRC_IPV4_ARP_H

#include "kernel_types.h"
#include "thread.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @defgroup    net_gnrc_ipv6_conf  GNRC IPv6 compile configurations
 * @ingroup     net_gnrc_ipv6
 * @ingroup     config
 * @{
 */
/**
 * @brief   Default stack size to use for the ARP thread,
            Max see with ps command: 1064
 */
#ifndef GNRC_IPV4_ARP_STACK_SIZE
#define GNRC_IPV4_ARP_STACK_SIZE        (2048)
#endif

/**
 * @brief   Default priority for the IPv6 thread
 */
#ifndef GNRC_IPV4_ARP_PRIO
#define GNRC_IPV4_ARP_PRIO              (THREAD_PRIORITY_MAIN - 3)
#endif

/**
 * @brief   Default message queue size to use for the IPv6 thread.
 */
#ifndef GNRC_IPV4_ARP_MSG_QUEUE_SIZE
#define GNRC_IPV4_ARP_MSG_QUEUE_SIZE    (8U)
#endif

/** @} */

/**
 * @brief   The PID to the IPv6 thread.
 *
 * @note    Use @ref gnrc_ipv6_init() to initialize. **Do not set by hand**.
 *
 * @details This variable is preferred for IPv6 internal communication *only*.
 *          Please use @ref net_gnrc_netreg for external communication.
 */
extern kernel_pid_t gnrc_ipv4_arp_pid;

/**
 * @brief   Initialization of the IPv6 thread.
 *
 * @return  The PID to the IPv6 thread, on success.
 * @return  a negative errno on error.
 * @return  -EOVERFLOW, if there are too many threads running already
 * @return  -EEXIST, if IPv6 was already initialized.
 */
kernel_pid_t gnrc_ipv4_arp_init(void);

void gnrc_ipv4_arp_print_table(void);

#ifdef __cplusplus
}
#endif

#endif /* NET_GNRC_IPV4_ARP_H */
/**
 * @}
 */
