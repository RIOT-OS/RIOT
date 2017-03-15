#ifndef SX1276_NETDEV_H_
#define SX1276_NETDEV_H_

#include "net/netdev2.h"

#ifdef __cplusplus
extern "C" {
#endif
extern const netdev2_driver_t sx1276_driver;
typedef struct netdev2_radio_rx_info netdev2_sx1276_rx_info_t;
#ifdef __cplusplus
}  /* extern "C" */
#endif
#endif
