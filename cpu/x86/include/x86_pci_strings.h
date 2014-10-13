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
 * The string representation of PCI constants. For humans.
 *
 * @ingroup x86-irq
 * @{
 * @file
 * @author  René Kijewski <rene.kijewski@fu-berlin.de>
 */

#ifndef CPU__X86__PCI_NAMES__H__
#define CPU__X86__PCI_NAMES__H__

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief        Get a string representation of the device class.
 * @param[out]   baseclass_name   Name of the base class. Supply NULL if you do not care.
 * @return       A string (lengthy text) that describes the device class.
 *
 * If the device class is unknown, then the hexadecimal value is returned.
 * This function is not thread safe.
 */
const char *x86_pci_subclass_to_string(unsigned baseclass, unsigned subclass, unsigned interface, const char **baseclass_name);

/**
 * @brief        Get a string representation of the device name.
 * @param[out]   vendor_name   Name of the device vendor. Supply NULL if you do not care.
 * @return       A string that tells the device name.
 *
 * If the device ID is unknown, then the hexadecimal value is returned.
 * This function is not thread safe.
 */
const char *x86_pci_device_id_to_string(unsigned vendor_id, unsigned device_id, const char **vendor_name);

#ifdef __cplusplus
}
#endif

#endif

/** @} */
