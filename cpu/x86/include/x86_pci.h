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
 * The PCI subsystem of x86 boards.
 *
 * @ingroup x86-irq
 * @{
 * @file
 * @author  René Kijewski <rene.kijewski@fu-berlin.de>
 */

#ifndef CPU__X86__PCI__H__
#define CPU__X86__PCI__H__

#include "x86_pci_init.h"
#include "x86_pic.h"

#include <machine/endian.h>
#include <stdbool.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Initialize the Peripheral Component Interconnect.
 *
 * This function is called during initialization by x86_startup().
 * You must not call this function on your own accord.
 */
void x86_init_pci(void);

#define PCI_CONFIG_DATA    (0x0cfc)
#define PCI_CONFIG_ADDRESS (0x0cf8)

#define PCI_IO_ENABLE    (1u << 31)
#define PCI_IO_SHIFT_BUS (16u)
#define PCI_IO_SHIFT_DEV (11u)
#define PCI_IO_SHIFT_FUN (8u)
#define PCI_IO_SHIFT_REG (0u)

#define PCI_BUS_COUNT (1u << 8)
#define PCI_DEV_COUNT (1u << 5)
#define PCI_FUN_COUNT (1u << 3)

#if BYTE_ORDER == LITTLE_ENDIAN
#   define U8_PCItoH(x)  (x)
#   define U16_PCItoH(x) (x)
#   define U32_PCItoH(x) (x)

#   define U8_HtoPCI(x)  (x)
#   define U16_HtoPCI(x) (x)
#   define U32_HtoPCI(x) (x)
#else
#   error "Only BYTE_ORDER == LITTLE_ENDIAN is supported right now"
#endif

// "PCI LOCAL BUS SPECIFICATION, REV. 3.0"; Appendix 6.1.

/**
 * @brief   The registers 0x00 through 0x03 of a PCI device.
 */
typedef union pci_reg_0x00 {
    __extension__ struct {
        /**
         * @brief   Vendor ID of the device
         *
         * The values PCI_HEADER_VENDOR_ID_INVALID, PCI_HEADER_VENDOR_ID_UNRESPONSIVE
         * and PCI_HEADER_VENDOR_ID_ABSENT denote a special status.
         */
        uint16_t vendor_id;
        uint16_t device_id; /**< Vendor specific device ID */
    } __attribute__((packed));
    uint32_t packed;
} __attribute__((packed)) pci_reg_0x00_t;

/**
 * @brief   The registers 0x04 through 0x07 of a PCI device.
 */
typedef union pci_reg_0x04 {
    __extension__ struct {
        uint16_t command; /**< Port to tell the PCI device to do things. */
        uint16_t status; /**< Reading the current status of a PCI device. */
    } __attribute__((packed));
    uint32_t packed;
} __attribute__((packed)) pci_reg_0x04_t;

/**
 * @brief   The registers 0x08 through 0x0b of a PCI device.
 */
typedef union pci_reg_0x08 {
    __extension__ struct {
        uint8_t revision_id;
        uint8_t programming_interface;
        uint8_t subclass;
        uint8_t baseclass;
    } __attribute__((packed));
    uint32_t packed;
} __attribute__((packed)) pci_reg_0x08_t;

/**
 * @brief   The registers 0x0c through 0x0f of a PCI device.
 */
typedef union pci_reg_0x0c {
    __extension__ struct {
        uint8_t cache_line_size;
        uint8_t latency_timer;
        uint8_t header_type;
        uint8_t builtin_selftest;
    } __attribute__((packed));
    uint32_t packed;
} __attribute__((packed)) pci_reg_0x0c_t;

/**
 * @brief   Generic layout of the port space of a PCI device.
 */
typedef struct pci_config {
    union pci_reg_0x00 reg_0x00;
    union pci_reg_0x04 reg_0x04;
    union pci_reg_0x08 reg_0x08;
    union pci_reg_0x0c reg_0x0c;

    /* reg = 0x10 */
    uint8_t header_type_specific1[0x34 - 0x10];

    /* reg = 0x34 */
    uint8_t capatibilities_pointer;
    uint8_t header_type_specific2[0x3c - 0x35];

    /* reg = 0x3C */
    uint8_t interrupt_line;
    uint8_t interrupt_pin;

    uint8_t header_type_specific3[0x40 - 0x3e];
} __attribute__((packed)) pci_config_t;

/**
 * @brief   Layout of the port space for PCI devices with `header_type == PCI_HEADER_TYPE_GENERAL_DEVICE`.
 */
typedef struct pci_config_standard {
    union pci_reg_0x00 reg_0x00;
    union pci_reg_0x04 reg_0x04;
    union pci_reg_0x08 reg_0x08;
    union pci_reg_0x0c reg_0x0c;

    /* reg = 0x10 */
    uint32_t bar[6];
    /* reg = 0x28 */
    uint32_t cardbus_cis_pointer;
    /* reg = 0x2c */
    uint16_t subsystem_vendor_id;
    uint16_t subsystem_id;
    // reg = 0x30 */
    uint32_t expansion_rom_address;

    /* reg = 0x34 */
    uint8_t capatibilities_pointer;
    uint8_t header_type_specific2[0x3c - 0x35];

    /* reg = 0x3C */
    uint8_t interrupt_line;
    uint8_t interrupt_pin;

    uint8_t header_type_specific3[0x40 - 0x3e];
} __attribute__((packed)) pci_config_standard_t;

/**
 * @brief   Layout of the port space for PCI devices with `header_type == PCI_HEADER_TYPE_BRIDGE`.
 */
typedef struct pci_config_bridge {
    union pci_reg_0x00 reg_0x00;
    union pci_reg_0x04 reg_0x04;
    union pci_reg_0x08 reg_0x08;
    union pci_reg_0x0c reg_0x0c;

    /* reg = 0x10 */
    uint32_t bar[2];

    /* reg = 0x18 */
    uint8_t bus_primary;
    uint8_t bus_secondary;
    uint8_t bus_subordinary;
    uint8_t bus_secondary_latency_timer;

    /* reg = 0x1c */
    uint8_t io_lower_base;
    uint8_t io_lower_limit;
    uint16_t status_secondary;

    /* reg = 0x20 */
    uint16_t memory_base;
    uint16_t memory_limit;

    /* reg = 0x24 */
    uint16_t prefetchable_lower_base;
    uint16_t prefetchable_lower_limit;

    // reg = 0x28
    uint32_t prefetchable_upper_base;
    // reg = 0x2c
    uint32_t prefetchable_upper_limit;

    // reg = 0x30
    uint16_t io_upper_base;
    uint16_t io_upper_limit;

    // reg = 0x34
    uint8_t capatibilities_pointer;
    uint8_t header_type_specific2[0x38 - 0x35];

    // reg = 0x38
    uint32_t extension_ron_address;

    // reg = 0x3C
    uint8_t interrupt_line;
    uint8_t interrupt_pin;
    uint16_t bridge_control;
} __attribute__((packed)) pci_config_bridge_t;

#define PCI_HEADER_VENDOR_ID_INVALID      (0x0000u)
#define PCI_HEADER_VENDOR_ID_UNRESPONSIVE (0x0001u)
#define PCI_HEADER_VENDOR_ID_ABSENT       (0xffffu)

#define PCI_HEADER_TYPE_GENERAL_DEVICE (0x00u)
#define PCI_HEADER_TYPE_BRIDGE         (0x01u)
#define PCI_HEADER_TYPE_CARD_BUS       (0x02u)

#define PCI_HEADER_TYPE_MULTI_FUNCTION (0x80u)
#define PCI_HEADER_TYPE_MASK (0x7fu)

#define PCI_COMMAND_IO_SPACE            (1u <<  0)
#define PCI_COMMAND_MEMORY_SPACE        (1u <<  1)
#define PCI_COMMAND_BUS_MASTER          (1u <<  2)
#define PCI_COMMAND_SPECIAL_CYCLES      (1u <<  3)
#define PCI_COMMAND_MEM_WRITE_AND_INV   (1u <<  4)
#define PCI_COMMAND_VGA_PALETTE_SNOOP   (1u <<  5)
#define PCI_COMMAND_PARITY_ERR_RESPONSE (1u <<  6)
#define PCI_COMMAND_SERR                (1u <<  8)
#define PCI_COMMAND_FAST_B2B            (1u <<  9)
#define PCI_COMMAND_INTERRUPT_DISABLE   (1u << 10)

#define PRI_STATUS_INTERRUPT           (1u <<  3)
#define PRI_STATUS_CAPATIBILITIES      (1u <<  4)
#define PRI_STATUS_66MHZ               (1u <<  5)
#define PRI_STATUS_FAST_B2B            (1u <<  7)
#define PRI_STATUS_MASTER_PARITY_ERROR (1u <<  8)
#define PRI_STATUS_DEVSEL_FAST         (0u <<  9)
#define PRI_STATUS_DEVSEL_MEDIUM       (1u <<  9)
#define PRI_STATUS_DEVSEL_SLOW         (3u <<  9)
#define PRI_STATUS_DEVSEL_MASK         (3u <<  9)
#define PRI_STATUS_SIG_TARGET_ABORT    (1u << 11)
#define PRI_STATUS_RECV_TARGET_ABORT   (1u << 12)
#define PRI_STATUS_RECV_MASTER_ABORT   (1u << 13)
#define PRI_STATUS_SIG_SYSTEM_ERROR    (1u << 14)
#define PRI_STATUS_PARITY_ERROR        (1u << 15)

#define PCI_CAPS_PTR_MIN (0x40u)
#define PCI_CAPS_PTR_MAX (0xf0u)

#define PCI_INTR_PIN_NONE (0x00u)
#define PCI_INTR_PIN_A    (0x01u)
#define PCI_INTR_PIN_B    (0x02u)
#define PCI_INTR_PIN_C    (0x03u)
#define PCI_INTR_PIN_D    (0x04u)

#define PCI_BAR_IO_SPACE     (1u << 0)
#define PCI_BAR_SPACE_32     (0u << 1)
#define PCI_BAR_SPACE_1M     (1u << 1)
#define PCI_BAR_SPACE_64     (2u << 1)
#define PCI_BAR_SPACE_MASK   (3u << 1)
#define PCI_BAR_PREFETCHABLE (1u << 3)
#define PCI_BAR_ADDR_OFFS_MEM (4u)
#define PCI_BAR_ADDR_OFFS_IO  (2u)

/**
 * @brief   Address type of a struct x86_pci_io.
 */
typedef enum {
    PCI_IO_INVALID = 0, /**< There was an error while configuring this IO port. */
    PCI_IO_MEM, /**< physical memory */
    PCI_IO_PORT, /**< port address */
} x86_pci_io_type_t;

/**
 * @brief   One IO port of a struct x86_known_pci_device.
 */
typedef struct x86_pci_io {
    x86_pci_io_type_t type; /**< Type of this IO port. */
    unsigned long length; /**< Length (in bytes or port) of this IO port. */
    union {
        void *ptr; /**< Base **virtual** memory address. */
        int port; /**< Base port number. */
    } addr; /**< Base address of the IO port. */
    uint8_t bar_num;
} x86_pci_io_t;

struct x86_known_pci_device;

/**
 * @brief       A callback handler if there was an IRQ on the IRQ line of this device.
 * @param[in]   d   Device that (might) need attention.
 *
 * Because PCI is multiplexer, there might not be an IRQ for this device.
 * This callback is called out of the interrupt handler (inISR() == true).
 * Lengthy operations should be handled in a dedicated thread; use msg_send_int().
 * You must no enable interrupt inside the handler.
 */
typedef void (*x86_pci_irq_handler_t)(struct x86_known_pci_device *d);

/**
 * @brief   A PCI device.
 */
typedef struct x86_known_pci_device {
    uint8_t bus, dev, fun;
    pci_reg_0x00_t vendor;
    pci_reg_0x08_t class;

    unsigned io_count; /**< Number of entries in io. */
    struct x86_pci_io **io; /**< Pointer to io_count pointers to configured IO ports. */

    bool managed; /**< To be set by the hardware driver if it is going to manage this device. */
    uint8_t irq; /**< IRQ line of this device. Change through x86_pci_set_irq(). */
    x86_pci_irq_handler_t irq_handler; /**< Callback function if there was an interrupt on `irq`. */
} x86_known_pci_device_t;

/**
 * @brief     Enumerate unmanaged PCI devices.
 * @param     index   Opaque reentrant pointer for the next iteration. Must be zero at first call.
 * @returns   Pointer to next unmanaged PCI device, or NULL.
 *
 * This function shall only be called by hardware drivers.
 * x86_init_pci() will call the hardware drivers once it is ready.
 * To not call earlier or later on your on accord.
 *
 * The result is a pointer to a pointer so that you can use realloc() to extend the struct as needed.
 * You must only do that if you are going to manage this device, set managed iff so.
 */
struct x86_known_pci_device **x86_enumerate_unmanaged_pci_devices(unsigned *index);

/**
 * @brief     Enumerate all PCI devices.
 * @param     index   Opaque reentrant pointer for the next iteration. Must be zero at first call.
 * @returns   Pointer to next PCI device, or NULL.
 */
const struct x86_known_pci_device *x86_enumerate_pci_devices(unsigned *index);

/**
 * @brief   Read a long word from the PCI configuration space.
 * @param   reg   The register must be aligned on a 4 bytes boundary.
 */
uint32_t x86_pci_read(unsigned bus, unsigned dev, unsigned fun, unsigned reg);
/**
 * @brief   Read a byte from the PCI configuration space.
 * @param   reg   The register must be aligned on a 4 bytes boundary.
 */
uint8_t x86_pci_read8(unsigned bus, unsigned dev, unsigned fun, unsigned reg);
/**
 * @brief   Read a short word from the PCI configuration space.
 * @param   reg   The register must be aligned on a 4 bytes boundary.
 */
uint16_t x86_pci_read16(unsigned bus, unsigned dev, unsigned fun, unsigned reg);

/**
 * @brief   Write a long word to the PCI configuration space.
 * @param   reg   The register must be aligned on a 4 bytes boundary.
 */
void x86_pci_write(unsigned bus, unsigned dev, unsigned fun, unsigned reg, uint32_t datum);
/**
 * @brief   Write a byte to the PCI configuration space.
 * @param   reg   The register must be aligned on a 4 bytes boundary.
 */
void x86_pci_write8(unsigned bus, unsigned dev, unsigned fun, unsigned reg, uint8_t datum);
/**
 * @brief   Write a short word to the PCI configuration space.
 * @param   reg   The register must be aligned on a 4 bytes boundary.
 */
void x86_pci_write16(unsigned bus, unsigned dev, unsigned fun, unsigned reg, uint16_t datum);

/**
 * @brief     Read a long word from the PCI configuration space, and cast the datum accordingly.
 * @param     REG   Either `0x00`, `0x04`, `0x08`, or `0x0c`. Must be a constant token.
 * @returns   A `union pci_reg_0x00`, `union pci_reg_0x04`, ...
 */
#define x86_pci_read_reg(REG, BUS, DEV, FUN) \
    ((union pci_reg_##REG) { .packed = x86_pci_read((BUS), (DEV), (FUN), (REG)) })

#define PCI_IRQ_ACPI       (PIC_NUM_LPT2)  /**< IRQ line that may be used by the ACPI. */
#define PCI_IRQ_NETWORKING (PIC_NUM_9)     /**< IRQ line that may be used by networking devices. */
#define PCI_IRQ_DEFAULT    (PIC_NUM_ATA_4) /**< IRQ line that may be used by other devices. */
#define PCI_IRQ_USB        (PIC_NUM_ATA_3) /**< IRQ line that may be used by USB host controllers. */

/**
 * @brief   Set the IRQ line if a PCI device.
 *
 * irq_num must be one of PCI_IRQ_ACPI, PCI_IRQ_NETWORKING, PCI_IRQ_DEFAULT, PCI_IRQ_USB.
 * Bad things will happen if you supply any other value.
 */
void x86_pci_set_irq(struct x86_known_pci_device *d, uint8_t irq_num);

#ifdef __cplusplus
}
#endif

#endif

/** @} */
