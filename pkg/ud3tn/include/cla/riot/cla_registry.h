/* SPDX-License-Identifier: BSD-3-Clause OR Apache-2.0 */
/**
 * @file
 * @brief CLA registry used by uD3TN's CLA implementation on RIOT.
 *
 * This header defines the registry and is included only by cla.c. Add a
 * constructor include and an entry below to register another CLA, and ensure
 * that its implementation is linked into the application.
 */
#ifndef CLA_RIOT_REGISTRY_H
#define CLA_RIOT_REGISTRY_H

#include "cla/riot/cla_tcpclv3_riot.h"

#include <stddef.h>

struct available_cla_list_entry {
    const char *name;
    struct cla_config *(*create_func)(
        const char *const *options,
        size_t options_count,
        const struct bundle_agent_interface *bundle_agent_interface);
};

const struct available_cla_list_entry AVAILABLE_CLAS[] = {
    { "tcpclv3-riot", &tcpclv3_riot_create },
};

#endif /* CLA_RIOT_REGISTRY_H */
