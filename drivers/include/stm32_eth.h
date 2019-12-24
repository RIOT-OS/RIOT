/*
 *
 * @file
 * @brief       Interface definition for the stm32 ethernet driver
 *
 * @author      Robin Lösch <robin@chilio.net>
 *
 * @{
 */

#ifndef STM32_ETH_H
#define STM32_ETH_H

#include "net/netdev.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Setup netdev
 *
 */
void stm32_eth_netdev_setup(netdev_t *netdev);

#ifdef __cplusplus
}
#endif

#endif /* STM32_ETH_H */
/* @} */
