/*
 * Copyright (C) 2014  René Kijewski  <rene.kijewski@fu-berlin.de>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 */

/**
 * @ingroup     sys_shell_commands
 * @{
 *
 * @file
 * @brief       List PCI devices
 *
 * @author      René Kijewski <rene.kijewski@fu-berlin.de>
 *
 * @}
 */

#include "x86_pci.h"
#include "x86_pci_strings.h"

#include <stdio.h>

extern void _x86_lspci(int argc, char **argv)
{
    (void) argc;
    (void) argv;

    unsigned index = 0;
    while (1) {
        const char *baseclass_name, *subclass_name, *vendor_name, *device_name;

        const x86_known_pci_device_t *d = x86_enumerate_pci_devices(&index);
        if (!d) {
            break;
        }

        subclass_name = x86_pci_subclass_to_string(d->class.baseclass,
                                                   d->class.subclass,
                                                   d->class.programming_interface,
                                                   &baseclass_name);
        vendor_name = x86_pci_device_id_to_string(d->vendor.vendor_id,
                                                  d->vendor.device_id,
                                                  &device_name);
        printf("%02x:%02x.%x \"%s\": \"%s\" (%s: %s, rev: %02hhx)\n",
               d->bus, d->dev, d->fun,
               vendor_name, device_name,
               baseclass_name, subclass_name, d->class.revision_id);
    }
}
