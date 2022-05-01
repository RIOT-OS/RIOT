/*
 * Copyright (C) 2022 Juliusz Neuman
 */

#ifndef IKE_KERNEL_IFACE_H
#define IKE_KERNEL_IFACE_H

#include "net/ipv6/addr.h"
#include "ike/chunk.h"

#ifdef __cplusplus
extern "C"
{
#endif

// TODO: add more parameters
int install_esp(chunk_t our_ska, chunk_t other_ska, chunk_t our_ske, chunk_t other_ske,
                ipv6_addr_t our_addr, ipv6_addr_t other_addr, uint32_t our_spi,
                uint32_t other_spi, int *policy_id_in, int *policy_id_out);

int clear_esp(uint32_t our_spi, uint32_t other_spi, int policy_in, int policy_out);


#ifdef __cplusplus
}
#endif

#endif /* IKE_KERNEL_IFACE_H */
