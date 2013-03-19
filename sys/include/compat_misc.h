/*
 * This file defines various functions and constants that are not properly defined in RIOT yet
 * TODO: move those functions and defines to their appropriate place in RIOT
 */

#ifndef __COMPAT_MISC_H__
#define __COMPAT_MISC_H__

typedef unsigned int sa_family_t;
typedef int socklen_t;

// since we only support v6…
#define sockaddr_in		socka6
#define sockaddr_in6		socka6
#define sockaddr		socka6
#define sockaddr_storage	socka6 

#define sa_family	sin6_family
#define sin_port	sin6_port
#define sin_addr	sin6_addr

#define htons HTONS
#define htonl HTONL
#define ntohs NTOHS
#define ntohl NTOHL

#define INET_ADDRSTRLEN		(16)
#define INET6_ADDRSTRLEN	(48)

#define random()	rand()

inline int getpagesize(void) {
	return 512;	// TODO: find appropriate pagesize
}

#define IF_NAMESIZE 4
// dummy implementation, we don't have interface names
inline char* if_indextoname(unsigned int ifindex, char *ifname) {
	ifname[0] = 'i';
	ifname[1] = 'f';
	ifname[2] = ifindex + '0';
	ifname[3] = 0;

	return ifname;
}

inline unsigned int if_nametoindex(const char *ifname) {
	return 1; // since we don't have interfaces…
}

/***************** multicast ***********/

#include "sys/net/sixlowpan/sixlowip.h"

/* IP_MTU_DISCOVER values */
#define IP_PMTUDISC_DONT                0       /* Never send DF frames */
#define IP_PMTUDISC_WANT                1       /* Use per route hints  */
#define IP_PMTUDISC_DO                  2       /* Always DF            */
#define IP_PMTUDISC_PROBE               3       /* Ignore dst pmtu      */

#define IP_MULTICAST_IF                 32
#define IP_MULTICAST_TTL                33
#define IP_MULTICAST_LOOP               34
#define IP_ADD_MEMBERSHIP               35
#define IP_DROP_MEMBERSHIP              36
#define IP_UNBLOCK_SOURCE               37
#define IP_BLOCK_SOURCE                 38
#define IP_ADD_SOURCE_MEMBERSHIP        39
#define IP_DROP_SOURCE_MEMBERSHIP       40
#define IP_MSFILTER                     41
#define MCAST_JOIN_GROUP                42
#define MCAST_BLOCK_SOURCE              43
#define MCAST_UNBLOCK_SOURCE            44
#define MCAST_LEAVE_GROUP               45
#define MCAST_JOIN_SOURCE_GROUP         46
#define MCAST_LEAVE_SOURCE_GROUP        47
#define MCAST_MSFILTER                  48
#define IP_MULTICAST_ALL                49
#define IP_UNICAST_IF                   50

/* Options for use with `getsockopt' and `setsockopt' at the IPv6 level.
   The first word in the comment at the right is the data type used;
   "bool" means a boolean value stored in an `int'.  */
#define IPV6_ADDRFORM           1
#define IPV6_2292PKTINFO        2
#define IPV6_2292HOPOPTS        3
#define IPV6_2292DSTOPTS        4
#define IPV6_2292RTHDR          5
#define IPV6_2292PKTOPTIONS     6
#define IPV6_CHECKSUM           7
#define IPV6_2292HOPLIMIT       8

#define IPV6_NEXTHOP            9
#define IPV6_AUTHHDR            10
#define IPV6_UNICAST_HOPS       16
#define IPV6_MULTICAST_IF       17
#define IPV6_MULTICAST_HOPS     18
#define IPV6_MULTICAST_LOOP     19
#define IPV6_JOIN_GROUP         20
#define IPV6_LEAVE_GROUP        21
#define IPV6_ROUTER_ALERT       22
#define IPV6_MTU_DISCOVER       23
#define IPV6_MTU                24
#define IPV6_RECVERR            25
#define IPV6_V6ONLY             26
#define IPV6_JOIN_ANYCAST       27
#define IPV6_LEAVE_ANYCAST      28
#define IPV6_IPSEC_POLICY       34
#define IPV6_XFRM_POLICY        35

#define IPV6_RECVPKTINFO        49
#define IPV6_PKTINFO            50
#define IPV6_RECVHOPLIMIT       51
#define IPV6_HOPLIMIT           52
#define IPV6_RECVHOPOPTS        53
#define IPV6_HOPOPTS            54
#define IPV6_RTHDRDSTOPTS       55
#define IPV6_RECVRTHDR          56
#define IPV6_RTHDR              57
#define IPV6_RECVDSTOPTS        58
#define IPV6_DSTOPTS            59

#define IPV6_RECVTCLASS         66
#define IPV6_TCLASS             67

struct ip_mreq {
            ipv6_addr_t imr_multiaddr;
            ipv6_addr_t imr_interface;
};

struct ipv6_mreq {
      ipv6_addr_t ipv6mr_multiaddr;
      unsigned int ipv6mr_interface;
};

#endif
