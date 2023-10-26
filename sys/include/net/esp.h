#ifndef NET_ESP_H
#define NET_ESP_H

#include <stdint.h>

#include "byteorder.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct __attribute__((packed)) {
    network_uint32_t spi;   /**< Security Paramters Index */
    network_uint32_t seq;   /**< Sequence number */
} esp_header_t;


#ifdef __cplusplus
}
#endif

#endif /* NET_ESP_H */
/** @} */
