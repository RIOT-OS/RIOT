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
 * @ingroup     x86-irq
 * @{
 *
 * @file
 * @brief       PCI configuration and accessing.
 *
 * @author      René Kijewski <rene.kijewski@fu-berlin.de>
 *
 * @}
 */

#include "x86_memory.h"
#include "x86_pci.h"
#include "x86_pci_init.h"
#include "x86_pci_strings.h"
#include "x86_ports.h"

#include <malloc.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static struct x86_known_pci_device **known_pci_devices = NULL;
static unsigned num_known_pci_devices;

static void set_addr(unsigned bus, unsigned dev, unsigned fun, unsigned reg)
{
    unsigned addr = PCI_IO_ENABLE
                  | (bus << PCI_IO_SHIFT_BUS)
                  | (dev << PCI_IO_SHIFT_DEV)
                  | (fun << PCI_IO_SHIFT_FUN)
                  | (reg << PCI_IO_SHIFT_REG);
    outl(PCI_CONFIG_ADDRESS, addr);
}

uint32_t x86_pci_read(unsigned bus, unsigned dev, unsigned fun, unsigned reg)
{
    set_addr(bus, dev, fun, reg);
    return U32_PCItoH(inl(PCI_CONFIG_DATA));
}

uint8_t x86_pci_read8(unsigned bus, unsigned dev, unsigned fun, unsigned reg)
{
    set_addr(bus, dev, fun, reg);
    return U8_PCItoH(inb(PCI_CONFIG_DATA));
}

uint16_t x86_pci_read16(unsigned bus, unsigned dev, unsigned fun, unsigned reg)
{
    set_addr(bus, dev, fun, reg);
    return U16_PCItoH(inw(PCI_CONFIG_DATA));
}

static bool pci_vendor_id_valid(uint16_t vendor_id)
{
    return vendor_id != PCI_HEADER_VENDOR_ID_INVALID &&
           vendor_id != PCI_HEADER_VENDOR_ID_UNRESPONSIVE &&
           vendor_id != PCI_HEADER_VENDOR_ID_ABSENT;
}

void x86_pci_write(unsigned bus, unsigned dev, unsigned fun, unsigned reg, uint32_t datum)
{
    set_addr(bus, dev, fun, reg);
    outl(PCI_CONFIG_DATA, U32_HtoPCI(datum));
}

void x86_pci_write8(unsigned bus, unsigned dev, unsigned fun, unsigned reg, uint8_t datum)
{
    set_addr(bus, dev, fun, reg);
    outb(PCI_CONFIG_DATA, U8_HtoPCI(datum));
}

void x86_pci_write16(unsigned bus, unsigned dev, unsigned fun, unsigned reg, uint16_t datum)
{
    set_addr(bus, dev, fun, reg);
    outw(PCI_CONFIG_DATA, U16_HtoPCI(datum));
}

static unsigned pci_init_secondary_bus(unsigned bus, unsigned dev, unsigned fun)
{
    known_pci_devices[num_known_pci_devices - 1]->managed = true;

    /* TODO */
    printf("    TODO: pci_init_secondary_bus(0x%x, 0x%x, 0x%x)\n", bus, dev, fun);
    (void) bus;
    (void) dev;
    (void) fun;
    return 0;
}

static void pci_setup_ios(struct x86_known_pci_device *dev)
{
    /* § 6.2.5. (pp. 224) */

    unsigned bar_count = 0;
    unsigned bar_base;

    unsigned header_type = x86_pci_read_reg(0x0c, dev->bus, dev->dev, dev->fun).header_type & PCI_HEADER_TYPE_MASK;
    switch (header_type) {
        case PCI_HEADER_TYPE_GENERAL_DEVICE:
            bar_count = 6;
            bar_base = 0x10;
            break;
        case PCI_HEADER_TYPE_BRIDGE:
            bar_count = 2;
            bar_base = 0x10;
            break;
        default:
            printf("    Cannot configure header_type == 0x%02x, yet.\n", header_type);
            return;
    }

    for (unsigned bar_num = 0; bar_num < bar_count; ++bar_num) {
        uint32_t old_bar = x86_pci_read(dev->bus, dev->dev, dev->fun, bar_base + 4 * bar_num);
        if (old_bar == 0) {
            continue;
        }

        x86_pci_write(dev->bus, dev->dev, dev->fun, bar_base + 4 * bar_num, -1ul);
        uint32_t tmp_bar = x86_pci_read(dev->bus, dev->dev, dev->fun, bar_base + 4 * bar_num);
        x86_pci_write(dev->bus, dev->dev, dev->fun, bar_base + 4 * bar_num, old_bar);
        if ((old_bar & PCI_BAR_IO_SPACE) != (tmp_bar & PCI_BAR_IO_SPACE)) {
            /* cannot happen (?) */
            continue;
        }

        dev->io = realloc(dev->io, sizeof (*dev->io) * (dev->io_count + 1));
        struct x86_pci_io *io = calloc(1, sizeof *io);
        dev->io[dev->io_count] = io;
        io->bar_num = bar_num;
        ++dev->io_count;

        unsigned addr_offs = tmp_bar & PCI_BAR_IO_SPACE ? PCI_BAR_ADDR_OFFS_IO : PCI_BAR_ADDR_OFFS_MEM;
        uint32_t length_tmp = tmp_bar >> addr_offs;
        uint32_t length = 1 << addr_offs;
        while ((length_tmp & 1) == 0) {
            length <<= 1;
            length_tmp >>= 1;
        }
        io->length = length;

        if (tmp_bar & PCI_BAR_IO_SPACE) {
            io->type = PCI_IO_PORT;
            io->addr.port = old_bar & ~((1 << PCI_BAR_ADDR_OFFS_IO) - 1);
            printf("    BAR %u: I/O space, ports 0x%04x-0x%04x\n",
                   bar_num, io->addr.port, io->addr.port + length - 1);
        }
        else if ((old_bar & PCI_BAR_IO_SPACE) != PCI_BAR_SPACE_32 && (old_bar & PCI_BAR_IO_SPACE) != PCI_BAR_SPACE_64) {
            printf("    BAR %u: memory with unknown location 0x%x, ERROR!\n", bar_num, (old_bar >> 1) & 3);
        }
        else {
            uint32_t physical_start = old_bar & ~0xfff;
            void *ptr = x86_map_physical_pages(physical_start, (length + 0xfff) / 0x1000, PT_P | PT_G | PT_RW | PT_PWT | PT_PCD | PT_XD);
            if (!ptr) {
                io->type = PCI_IO_INVALID;
                printf("    BAR %u: memory, physical = 0x%08x-0x%08x, ERROR!\n",
                       bar_num, (unsigned) physical_start, physical_start + length - 1);
            }
            else {
                io->type = PCI_IO_MEM;
                io->addr.ptr = (char *) ptr + (old_bar & ~0xfff & ~((1 << PCI_BAR_ADDR_OFFS_MEM) - 1));
                printf("    BAR %u: memory, physical = 0x%08x-0x%08x, virtual = 0x%08x-0x%08x\n",
                       bar_num,
                       physical_start, physical_start + length - 1,
                       (unsigned) ptr, (uintptr_t) ptr + length - 1);
            }
        }
    }
}

static void pci_find_on_bus(unsigned bus);

void x86_pci_set_irq(struct x86_known_pci_device *d, uint8_t irq_num)
{
    if (d->irq == irq_num) {
        return;
    }

    d->irq = irq_num;
    uint32_t old_3c = x86_pci_read(d->bus, d->dev, d->fun, 0x3c);
    x86_pci_write(d->bus, d->dev, d->fun, 0x3c, (old_3c & ~0xff) | d->irq);

    printf("    IRQ: new = %u, old = %u\n", d->irq, old_3c & 0xff);
}

static void pci_find_function(unsigned bus, unsigned dev, unsigned fun)
{
    union pci_reg_0x00 vendor = x86_pci_read_reg(0x00, bus, dev, fun);
    if (!pci_vendor_id_valid(vendor.vendor_id)) {
        return;
    }

    union pci_reg_0x08 class = x86_pci_read_reg(0x08, bus, dev, fun);
    const char *baseclass_name, *subclass_name = x86_pci_subclass_to_string(class.baseclass,
                                                                            class.subclass,
                                                                            class.programming_interface,
                                                                            &baseclass_name);
    const char *vendor_name, *device_name = x86_pci_device_id_to_string(vendor.vendor_id, vendor.device_id, &vendor_name);
    printf("  %02x:%02x.%x \"%s\": \"%s\" (%s: %s, rev: %02hhx)\n",
           bus, dev, fun, vendor_name, device_name, baseclass_name, subclass_name, class.revision_id);

    /* cppcheck-suppress memleakOnRealloc */
    known_pci_devices = realloc(known_pci_devices, sizeof (*known_pci_devices) * (num_known_pci_devices + 1));
    struct x86_known_pci_device *d = calloc(1, sizeof *d);
    known_pci_devices[num_known_pci_devices] = d;
    ++num_known_pci_devices;

    d->bus = bus;
    d->dev = dev;
    d->fun = fun;
    d->vendor = vendor;
    d->class = class;
    d->managed = false;

    uint32_t old_3c = x86_pci_read(bus, dev, fun, 0x3c);
    if (old_3c & 0xff) {
        d->irq = PCI_IRQ_DEFAULT;
        x86_pci_write(bus, dev, fun, 0x3c, (old_3c & ~0xff) | d->irq);
        printf("    IRQ: new = %u, old = %u\n", d->irq, old_3c & 0xff);
    }

    pci_setup_ios(d);

    if (class.baseclass == 0x06 && class.subclass == 0x04) {
        unsigned secondary_bus = pci_init_secondary_bus(bus, dev, fun);
        if (secondary_bus != 0) {
            pci_find_on_bus(secondary_bus);
        }
    }
}

static void pci_find_on_bus(unsigned bus)
{
    for (unsigned dev = 0; dev < PCI_DEV_COUNT; ++dev) {
        if (!pci_vendor_id_valid(x86_pci_read_reg(0x00, bus, dev, 0).vendor_id)) {
            continue;
        }

        if (x86_pci_read_reg(0x0c, bus, dev, 0).header_type & PCI_HEADER_TYPE_MULTI_FUNCTION) {
            for (unsigned fun = 0; fun < PCI_FUN_COUNT; ++fun) {
                pci_find_function(bus, dev, fun);
            }
        }
        else {
            pci_find_function(bus, dev, 0);
        }
    }
}

static void pci_find(void)
{
    if (x86_pci_read_reg(0x0c, 0, 0, 0).header_type & PCI_HEADER_TYPE_MULTI_FUNCTION) {
        for (unsigned fun = 0; fun < PCI_FUN_COUNT; ++fun) {
            if (pci_vendor_id_valid(x86_pci_read_reg(0x00, 0, 0, fun).vendor_id)) {
                pci_find_on_bus(fun);
            }
        }
    }
    else {
        pci_find_on_bus(0);
    }
}

static void irq_handler(uint8_t irq_num)
{
    for (unsigned i = 0; i < num_known_pci_devices; ++i) {
        struct x86_known_pci_device *d = known_pci_devices[i];
        if (d->managed && d->irq_handler && d->irq == irq_num) {
            d->irq_handler(d);
        }
    }
}

void x86_init_pci(void)
{
    x86_pic_set_handler(PCI_IRQ_ACPI, irq_handler);
    x86_pic_enable_irq(PCI_IRQ_ACPI);
    x86_pic_set_handler(PCI_IRQ_NETWORKING, irq_handler);
    x86_pic_enable_irq(PCI_IRQ_NETWORKING);
    x86_pic_set_handler(PCI_IRQ_DEFAULT, irq_handler);
    x86_pic_enable_irq(PCI_IRQ_DEFAULT);
    x86_pic_set_handler(PCI_IRQ_USB, irq_handler);
    x86_pic_enable_irq(PCI_IRQ_USB);

    puts("Looking up PCI devices");
    pci_find();

    x86_init_pci_devices();
}

struct x86_known_pci_device **x86_enumerate_unmanaged_pci_devices(unsigned *index)
{
    while (*index < num_known_pci_devices) {
        struct x86_known_pci_device **result = &known_pci_devices[*index];
        ++*index;
        if (*result && !(**result).managed) {
            return result;
        }
    }
    return NULL;
}

const struct x86_known_pci_device *x86_enumerate_pci_devices(unsigned *index)
{
    while (*index < num_known_pci_devices) {
        struct x86_known_pci_device *result = known_pci_devices[*index];
        ++*index;
        if (result) {
            return result;
        }
    }
    return NULL;
}
