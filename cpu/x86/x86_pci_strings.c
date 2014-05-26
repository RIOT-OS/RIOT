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
 * @brief       String names of PCI classes, vendors, and devices.
 *
 * @author      René Kijewski <rene.kijewski@fu-berlin.de>
 *
 * @}
 */

#include "x86_pci_strings.h"

#include <stdio.h>

/* TODO: don't always include all strings */

const char *x86_pci_subclass_to_string(unsigned baseclass, unsigned subclass, unsigned interface, const char **baseclass_name)
{
    static char temp_baseclass_number[7], temp_subclass_number[7];

    const char *baseclass_name_null;
    if (!baseclass_name) {
        baseclass_name = &baseclass_name_null;
    }

    // "PCI LOCAL BUS SPECIFICATION, REV. 3.0"; Appendix D.
    switch (baseclass) {
        case 0x00:
            *baseclass_name = "Device was built before Class Code definitions were finalized";
            switch (subclass) {
                case 0x00: return "Non-VGA";
                case 0x01: return "VGA";
            }
            break;

        case 0x01:
            *baseclass_name = "Mass storage controller";
            switch (subclass) {
                case 0x00: return "SCSI bus controller";
                case 0x01: return "IDE controller";
                case 0x02: return "Floppy disk controller";
                case 0x03: return "IPI bus controller";
                case 0x04: return "RAID controller";
                case 0x05: return "ATA controller";
                case 0x06: return "Serial ATA Direct Port Access";
                case 0x80: return "Other mass storage controller";
            }
            break;

        case 0x02:
            *baseclass_name = "Network controller";
            switch (subclass) {
                case 0x00: return "Ethernet controller";
                case 0x01: return "Token Ring controller";
                case 0x02: return "FDDI controller";
                case 0x03: return "ATM controller";
                case 0x04: return "ISDN controller";
                case 0x05: return "WorldFip controller";
                case 0x06: return "PICMG 2.14 Multi Computing";
                case 0x80: return "Other network controller";
            }
            break;

        case 0x03:
            *baseclass_name = "Display controller";
            switch (subclass) {
                case 0x00: switch (interface) {
                    case 0x00: return "VGA-compatible controller";
                    case 0x01: return "8514-compatible controller";
                    default: return "[UNDEFINED IN ACPI 3.0]";
                }
                case 0x01: return "XGA coltroller";
                case 0x02: return "3D controller";
                case 0x80: return "Other display controller";
            }
            break;

        case 0x04:
            *baseclass_name = "Multimedia device";
            switch (subclass) {
                case 0x00: return "Video device";
                case 0x01: return "Audio device";
                case 0x02: return "Computer telephony device";
                case 0x80: return "Other multimedia device";
            }
            break;

        case 0x05:
            *baseclass_name = "Memory controller";
            switch (subclass) {
                case 0x00: return "RAM";
                case 0x01: return "Flash";
                case 0x80: return "Other memory controller";
            }
            break;

        case 0x06:
            *baseclass_name = "Bridge device";
            switch (subclass) {
                case 0x00: return "Host bridge";
                case 0x01: return "ISA bridge";
                case 0x02: return "EISA bridge";
                case 0x03: return "MCA bridge";
                case 0x04: return "PCI-to-PCI bridge";
                case 0x05: return "PCMCIA bridge";
                case 0x06: return "NuBus bridge";
                case 0x07: return "CardBus bridge";
                case 0x08: return "RACEway bridge";
                case 0x09: return "Semi-transparent PCI-to-PCI bridge";
                case 0x0a: return "InfiniBand-to-PCI host bridge";
                case 0x80: return "Other bridge device";
            }
            break;

        case 0x07:
            *baseclass_name = "Simple communication controller";
            switch (subclass) {
                case 0x00:
                    switch (interface) {
                        case 0x00: return "Generic XT-compatible serial controller";
                        case 0x01: return "16450-compatible serial controller";
                        case 0x02: return "16550-compatible serial controller";
                        case 0x03: return "16650-compatible serial controller";
                        case 0x04: return "16750-compatible serial controller";
                        case 0x05: return "16850-compatible serial controller";
                        case 0x06: return "16950-compatible serial controller";
                    }
                    break;
                case 0x01:
                    switch (interface) {
                        case 0x00: return "Parallel port";
                        case 0x01: return "Bi-directional parallel port";
                        case 0x02: return "ECP 1.X compliant parallel port";
                        case 0x03: return "IEEE1284 controller";
                        case 0xef: return "IEEE1284 target device";
                    }
                    break;
                case 0x02: return "Multiport serial controller";
                case 0x03:
                    switch (interface) {
                        case 0x00: return "Generic modem";
                        case 0x01: return "Hayes compatible modem, 16450-compatible interface";
                        case 0x02: return "Hayes compatible modem, 16550-compatible interface";
                        case 0x03: return "Hayes compatible modem, 16650-compatible interface";
                        case 0x04: return "Hayes compatible modem, 16750-compatible interface";
                    }
                    break;
                case 0x04: return "GPIB (IEEE 488.1/2) controller";
                case 0x05: return "Smart Card";
                case 0x80: return "Other communications device";
            }
            break;

        case 0x08:
            *baseclass_name = "Base system peripheral";
            switch (subclass) {
                case 0x00:
                    switch (interface) {
                        case 0x00: return "Generic 8259 PIC";
                        case 0x01: return "ISA PIC";
                        case 0x02: return "EISA PIC";
                        case 0x10: return "I/O APIC interrupt controller (see below)";
                        case 0x20: return "I/O(x) APIC interrupt controller";
                    }
                    break;

                case 0x01:
                    switch (interface) {
                        case 0x00: return "Generic 8237 DMA controller";
                        case 0x01: return "ISA DMA controller";
                        case 0x02: return "EISA DMA controller";
                    }
                    break;

                case 0x02:
                    switch (interface) {
                        case 0x00: return "Generic 8254 system timer";
                        case 0x01: return "ISA system timer";
                        case 0x02: return "EISA system timers (two timers)";
                    }
                    break;

                case 0x03:
                    switch (interface) {
                        case 0x00: return "Generic RTC controller";
                        case 0x01: return "ISA RTC controller";
                    }
                    break;

                case 0x04: return "Generic PCI Hot-Plug controller";
                case 0x80: return "Other system peripheral";
            }
            break;

        case 0x09:
            *baseclass_name = "Input device";
            switch (subclass) {
                case 0x00: return "Keyboard controller";
                case 0x01: return "Digitizer (pen)";
                case 0x02: return "Mouse controller";
                case 0x03: return "Scanner controller";
                case 0x04: return "Gameport controller";
                case 0x80: return "Other input controller";
            }
            break;

        case 0x0a:
            *baseclass_name = "Docking station";
            switch (subclass) {
                case 0x00: return "Generic docking station";
                case 0x80: return "Other type of docking station";
            }
            break;

        case 0x0b:
            *baseclass_name = "Processor";
            switch (subclass) {
                case 0x00: return "386";
                case 0x01: return "486";
                case 0x02: return "Pentium";
                case 0x10: return "Alpha";
                case 0x20: return "PowerPC";
                case 0x30: return "MIPS";
                case 0x40: return "Co-processor";
            }
            break;

        case 0x0c:
            *baseclass_name = "Serial bus controller";
            switch (subclass) {
                case 0x00:
                    switch (interface) {
                        case 0x00: return "IEEE 1394 (FireWire)";
                        case 0x01: return "IEEE 1394 following the 1394 OpenHCI specification";
                    }
                    break;

                case 0x01: return "ACCESS.bus";
                case 0x02: return "SSA";

                case 0x03:
                    switch (interface) {
                        case 0x00: return "Universal Serial Bus (USB) following the Universal Host Controller Specification";
                        case 0x10: return "Universal Serial Bus (USB) following the Open Host Controller Specification";
                        case 0x20: return "USB2 host controller following the Intel Enhanced Host Controller Interface";
                        case 0x80: return "Universal Serial Bus with no specific programming interface";
                        case 0xfe: return "USB device";
                    }
                    break;

                case 0x04: return "Fibre Channel";
                case 0x05: return "SMBus";
                case 0x06: return "InfiniBand";

                case 0x07:
                    switch (interface) {
                        case 0x00: return "IPMI SMIC Interface";
                        case 0x01: return "IPMI Kybd Controller Style Interface";
                        case 0x02: return "IPMI Block Transfer Interface";
                    }
                    break;

                case 0x08: return "SERCOS Interface Standard (IEC 61491)";
                case 0x09: return "CANbus";
            }
            break;

        case 0x0d:
            *baseclass_name = "Wireless controller";
            switch (subclass) {
                case 0x00: return "iRDA compatible controller";
                case 0x01: return "Consumer IR controller";
                case 0x10: return "RF controller";
                case 0x11: return "Bluetooth";
                case 0x12: return "Broadband";
                case 0x20: return "Ethernet (802.11a)";
                case 0x21: return "Ethernet (802.11b)";
                case 0x80: return "Other type of wireless controller";
            }
            break;

        case 0x0e:
            *baseclass_name = "Intelligent I/O controller";
            switch (subclass) {
                case 0x00:
                    switch (interface) {
                        case 0x00: return "Message FIFO at offset 040h";
                        default: return "Intelligent I/O (I2O) Architecture Specification 1.0";
                    }
                    break;
            }
            break;

        case 0x0f:
            *baseclass_name = "Satellite communication controller";
            switch (subclass) {
                case 0x01: return "TV";
                case 0x02: return "Audio";
                case 0x03: return "Voice";
                case 0x04: return "Data";
            }
            break;

        case 0x10:
            *baseclass_name = "Encryption/Decryption controller";
            switch (subclass) {
                case 0x00: return "Network and computing en/decryption";
                case 0x10: return "Entertainment en/decryption";
                case 0x80: return "Other en/decryption";
            }
            break;

        case 0x11:
            *baseclass_name = "Data acquisition and signal processing controller";
            switch (subclass) {
                case 0x00: return "DPIO modules";
                case 0x01: return "Performance counters";
                case 0x10: return "Communications synchronization plus time and frequency test/measurement";
                case 0x20: return "Management card";
                case 0x80: return "Other data acquisition/signal processing controllers";
            }
            break;

        case 0xff:
            *baseclass_name = "Other device";
            break;

        default:
            snprintf(temp_baseclass_number, sizeof temp_baseclass_number, "0x%04x", baseclass);
            *baseclass_name = temp_baseclass_number;
    }
    snprintf(temp_subclass_number, sizeof temp_subclass_number, "0x%04x", subclass);
    return temp_subclass_number;
}

const char *x86_pci_device_id_to_string(unsigned vendor_id, unsigned device_id, const char **vendor_name)
{
    static char temp_vendor_name[7], temp_device_name[7];

    const char *vendor_name_null;
    if (!vendor_name) {
        vendor_name = &vendor_name_null;
    }

    // http://pci-ids.ucw.cz/read/PC/
    // http://www.pcidatabase.com/
    switch (vendor_id) {
        case 0x1013:
            *vendor_name = "Cirrus Logic";
            switch (device_id) {
                case 0x00b8: return "GD 5446";
            }
            break;

        case 0x8086:
            *vendor_name = "Intel Corporation";
            switch (device_id) {
                case 0x100e: return "82540EM Gigabit Ethernet Controller";
                case 0x1237: return "440FX - 82441FX PMC [Natoma]";
                case 0x2415: return "82801AA AC'97 Audio Controller";
                case 0x2668: return "82801FB/FBM/FR/FW/FRW (ICH6 Family) HDA Controller";
                case 0x7000: return "82371SB PIIX3 ISA [Natoma/Triton II]";
                case 0x7010: return "82371SB PIIX3 IDE [Natoma/Triton II]";
                case 0x7020: return "82371SB PIIX3 USB [Natoma/Triton II]";
                case 0x7113: return "82371AB/EB/MB PIIX4 ACPI";

            /* intel galileo */
            /* FIXME: find out the actual names ... */
                case 0x0958: return "Host Bridge";
                case 0x095E: return "Legacy Bridge";
                case 0x11C3: return "PCIe* Root Port 0";
                case 0x11C4: return "PCIe* Root Port 1";
                case 0x08A7: return "SDIO / eMMC Controller";
                case 0x0936: return "HS-UART";
                case 0x0939: return "USB 2.0 Device";
                case 0x093A: return "USB OHCI Host Controller";
                case 0x0937: return "10/100 Ethernet";
                case 0x0935: return "SPI Controller";
                case 0x0934: return "I2C* Controller and GPIO Controller";
            }
            break;

        default:
            snprintf(temp_vendor_name, sizeof temp_vendor_name, "0x%04x", vendor_id);
            *vendor_name = temp_vendor_name;
    }
    snprintf(temp_device_name, sizeof temp_device_name, "0x%04x", device_id);
    return temp_device_name;
}
