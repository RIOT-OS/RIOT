/*
 * SPDX-FileCopyrightText: 2025 Krzysztof Cabaj <kcabaj@gmail.com>
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Static initializer for an IPv4 address.
 *
 * @param[in] a         1st octet
 * @param[in] b         2nd octet
 * @param[in] c         3rd octet
 * @param[in] d         4th octet
 *
 * @return initialized LWIP ip4_addr_t address
 */
#define LWIP_IP4_ADDR_INIT(a, b, c, d) { .addr = a + (b << 8) + (c << 16) + (d << 24) }

#ifdef __cplusplus
}
#endif