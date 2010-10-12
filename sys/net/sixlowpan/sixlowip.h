/* 6LoWPAN IP header file */

#include <stdint.h>

#define IPV6_VERSION                0x60
#define ICMP_NEXTHEADER             0x3A
#define NEIGHBOR_DISCOVERY_HOPLIMIT 255


typedef struct icmp_hdr{
    uint8_t type;
    uint8_t code;
    uint16_t checksum;
} icmp_hdr;

typedef struct ip_hdr{
    uint8_t version_trafficclass;
    uint8_t trafficclass_flowlabel;
    uint16_t flowlabel;
    uint16_t length;
    uint8_t nextheader;
    uint8_t hoplimit;
} ip_hdr;
