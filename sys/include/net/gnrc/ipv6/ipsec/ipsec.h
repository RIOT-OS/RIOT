#ifndef NET_GNRC_IPV6_IPSEC_H
#define NET_GNRC_IPV6_IPSEC_H


#include "net/gnrc/pkt.h"
#include "net/gnrc/ipv6/ipsec/ipsec_enums.h"
#include "net/gnrc/ipv6/ipsec/ipsec_ts.h"
#ifdef __cplusplus
extern "C" {
#endif


kernel_pid_t gnrc_ipv6_ipsec_init(void);

ipsec_sp_rule_t ipsec_get_policy_rule(ipsec_ts_t *ts, ipsec_traffic_dir_t dir);

#ifdef __cplusplus
}
#endif

#endif /* NET_GNRC_IPV6_IPSEC_H */
