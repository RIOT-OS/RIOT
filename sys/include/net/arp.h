#ifndef NET_ARP_H
#define NET_ARP_H

#include "kernel_types.h"
#include "thread.h"

#include "net/ipv4/addr.h"

#ifdef __cplusplus
extern "C" {
#endif

#define ARP_MAC_SIZE (6)

#define ARP_FLAG_KNOWN      (1)
#define ARP_FLAG_COMPLETE   (2)

#ifndef ARP_RETRY_COUNT
#define ARP_RETRY_COUNT     (3)
#endif

typedef struct __attribute__((packed)) {
    ipv4_addr_t ipv4;
    ipv4_addr_t mask;
    uint8_t mac[ARP_MAC_SIZE];
    uint8_t flags;
    uint8_t retryCount;
    kernel_pid_t iface;
} arp_t;

typedef struct __attribute__((packed)) {
    network_uint16_t hw_type;
    network_uint16_t protocol_type;
    uint8_t hw_size;
    uint8_t protocol_size;
    network_uint16_t opcode;
    uint8_t sender_hw_addr[6];
    ipv4_addr_t sender_protocol_addr;
    uint8_t target_hw_addr[6];
    ipv4_addr_t target_protocol_addr;
} arp_payload_t;

typedef struct __attribute__((packed)) {
    ipv4_addr_t ipv4;
    uint8_t mac[ARP_MAC_SIZE];
    kernel_pid_t iface;
} arp_netapi_get_t;

#ifdef __cplusplus
}
#endif

#endif /* NET_ARP_H */
/**
 * @}
 */
