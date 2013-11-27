/*
 * common.h
 *
 *  Created on: 05.10.2011
 *      Author: Oliver
 */

/**
 * @defgroup	net_help Net help
 * @ingroup		net
 * @brief		Helper functions for networking as byte order conversions and checksum calculations
 */

#ifndef COMMON_H_
#define COMMON_H_
#include <string.h>
#include <stdint.h>

#define BITSET(var,pos) ((var) & (1<<(pos)))
#define HTONS(a) ((((uint16_t) (a) >> 8) & 0xff) | ((((uint16_t) (a)) & 0xff) << 8))
#define HTONL(a) ((((uint32_t) (a) & 0xff000000) >> 24) | \
                  (((uint32_t) (a) & 0x00ff0000) >> 8)  | \
                  (((uint32_t) (a) & 0x0000ff00) << 8)  | \
                  (((uint32_t) (a) & 0x000000ff) << 24))
#define NTOHS HTONS
#define NTOHL HTONL

#define CMP_IPV6_ADDR(a, b) (memcmp(a, b, 16))

uint16_t csum(uint16_t sum, uint8_t *buf, uint16_t len);
void printArrayRange(uint8_t *array, uint16_t len, char *str);

#endif /* COMMON_H_ */
