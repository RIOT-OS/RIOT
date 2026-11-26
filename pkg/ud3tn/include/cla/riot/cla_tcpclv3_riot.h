/* SPDX-License-Identifier: BSD-3-Clause OR Apache-2.0 */
/**
 * @file
 * @brief Public interface for the RIOT TCPCLv3 convergence-layer adapter.
 */
#ifndef CLA_TCPCLV3_RIOT_H
#define CLA_TCPCLV3_RIOT_H

#include "cla/cla.h"

#include "ud3tn/bundle_processor.h"

#include <stddef.h>

/**
 * @brief Create and initialize a RIOT TCPCLv3 convergence-layer instance.
 *
 * @param options Adapter options: the IP address followed by the port.
 * @param options_count Number of entries in @p options; must be 2.
 * @param bundle_agent_interface Bundle-agent interface used by the adapter.
 * @return The initialized CLA configuration, or `NULL` if creation fails.
 */
extern struct cla_config *tcpclv3_riot_create(
    const char *const *options, size_t options_count,
    const struct bundle_agent_interface *bundle_agent_interface);

#endif /* CLA_TCPCLV3_RIOT_H */
