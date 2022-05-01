
#ifndef NET_GNRC_IPV6_IPSEC_CONFIG_H
#define NET_GNRC_IPV6_IPSEC_CONFIG_H

#include "kernel_defines.h"
#include "timex.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Default stack size to use for the IPSEC thread.
 */
#ifndef GNRC_IPV6_IPSEC_STACK_SIZE
#define GNRC_IPV6_IPSEC_STACK_SIZE           2 * (THREAD_STACKSIZE_DEFAULT)
#endif

/**
 * @brief   Default priority for the IPSEC thread.
 */
#ifndef GNRC_IPV6_IPSEC_PRIO
#define GNRC_IPV6_IPSEC_PRIO                 (THREAD_PRIORITY_MAIN - 2)
#endif

/**
 * @brief   Default message queue size to use for the IPSEC thread (as
 *          exponent of 2^n).
 *
 * As the queue size ALWAYS needs to be power of two, this option represents the
 * exponent of 2^n, which will be used as the size of the queue.
 */
#ifndef CONFIG_GNRC_IPV6_IPSEC_MSG_QUEUE_SIZE_EXP
#define CONFIG_GNRC_IPV6_IPSEC_MSG_QUEUE_SIZE_EXP   (3U)
#endif

/**
 * @brief   Message queue size to use for the IPSEC thread.
 */
#ifndef GNRC_IPV6_IPSEC_MSG_QUEUE_SIZE
#define GNRC_IPV6_IPSEC_MSG_QUEUE_SIZE    (1 << CONFIG_GNRC_IPV6_IPSEC_MSG_QUEUE_SIZE_EXP)
#endif

#ifndef IPSEC_MAX_KEY_SIZE
#define IPSEC_MAX_KEY_SIZE 32
#endif

#ifndef IPSEC_MAX_HASH_SIZE
#define IPSEC_MAX_HASH_SIZE 32
#endif

#ifndef IPSEC_MAX_SA_NUM
#define IPSEC_MAX_SA_NUM 5
#endif

#ifndef IPSEC_MAX_SP_NUM
#define IPSEC_MAX_SP_NUM 5
#endif

#ifdef __cplusplus
}
#endif

#endif /* NET_GNRC_IPV6_IPSEC_CONFIG_H */
