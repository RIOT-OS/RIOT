/**
 * @defgroup    net_help Net help
 * @ingroup     net
 * @brief       Helper functions for networking as byte order conversions and checksum calculations
 * @{
 *
 * @file        net_help.h
 *
 * @author      Oliver
 */

#ifndef __NET_HELP_H
#define __NET_HELP_H

#include <string.h>
#include <stdint.h>

#define BITSET(var,pos) ((var) & (1<<(pos)))
#define HTONS(a) ((((uint16_t) (a) >> 8) & 0xff) | ((((uint16_t) (a)) & 0xff) << 8))
#define HTONL(a) ((((uint32_t) (a) & 0xff000000) >> 24) | \
                  (((uint32_t) (a) & 0x00ff0000) >> 8)  | \
                  (((uint32_t) (a) & 0x0000ff00) << 8)  | \
                  (((uint32_t) (a) & 0x000000ff) << 24))
#define HTONLL(a) ((((uint64_t) (a) & 0xff00000000000000) >> 56) | \
                   (((uint64_t) (a) & 0x00ff000000000000) >> 40)  | \
                   (((uint64_t) (a) & 0x0000ff0000000000) >> 24) | \
                   (((uint64_t) (a) & 0x000000ff00000000) >> 8)  | \
                   (((uint64_t) (a) & 0x00000000ff000000) << 8)  | \
                   (((uint64_t) (a) & 0x0000000000ff0000) << 24)  | \
                   (((uint64_t) (a) & 0x000000000000ff00) << 40)  | \
                   (((uint64_t) (a) & 0x00000000000000ff) << 56))
#define NTOHS HTONS
#define NTOHL HTONL
#define NTOHLL HTONLL

#define CMP_IPV6_ADDR(a, b) (memcmp(a, b, 16))

uint16_t csum(uint16_t sum, uint8_t *buf, uint16_t len);
void printArrayRange(uint8_t *array, uint16_t len, char *str);

/** @} */
#endif /* __NET_HELP_H */
