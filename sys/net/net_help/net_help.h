/*
 * common.h
 *
 *  Created on: 05.10.2011
 *      Author: Oliver
 */

#ifndef COMMON_H_
#define COMMON_H_

#define HTONS(a) ((((uint16_t) (a) >> 8) & 0xff) | ((((uint16_t) (a)) & 0xff) << 8))
#define HTONL(a) ((((uint32_t) (a) & 0xff000000) >> 24) | \
                  (((uint32_t) (a) & 0x00ff0000) >> 8)  | \
                  (((uint32_t) (a) & 0x0000ff00) << 8)  | \
                  (((uint32_t) (a) & 0x000000ff) << 24))

uint16_t csum(uint16_t sum, uint8_t *buf, uint16_t len);

#endif /* COMMON_H_ */
