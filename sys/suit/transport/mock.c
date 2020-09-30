/*
 * Copyright (C) 2020 Kaspar Schleiser <kaspar@schleiser.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#include <assert.h>
#include <stdio.h>
#include <stdbool.h>

#include "kernel_defines.h"
#include "log.h"

#include "suit.h"
#include "suit/storage.h"
#include "suit/transport/mock.h"

/* Must be defined by the test */
extern const suit_transport_mock_payload_t payloads[];
extern const size_t num_payloads;

static const suit_component_t *_get_component(const suit_manifest_t *manifest)
{
    assert(manifest->component_current < CONFIG_SUIT_COMPONENT_MAX);
    return &manifest->components[manifest->component_current];
}

int suit_transport_mock_fetch(const suit_manifest_t *manifest)
{
    size_t file = manifest->component_current;
    const suit_component_t *comp = _get_component(manifest);

    assert(file < num_payloads);

    LOG_INFO("Mock writing payload %d\n", (unsigned)file);

    suit_storage_write(comp->storage_backend, manifest, payloads[file].buf, 0,
                       payloads[file].len);
    return 0;
}
