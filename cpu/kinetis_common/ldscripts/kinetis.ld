/*
 * Copyright (C) 2015 PHYTEC Messtechnik GmbH
 * Copyright (C) 2015 Eistec AB
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @addtogroup      cpu_kinetis
 * @{
 *
 * @file
 * @brief           Sections definitions for the Freescale Kinetis MCUs
 *
 * @author          Johann Fischer <j.fischer@phytec.de>
 * @author          Joakim Nohlgård <joakim.nohlgard@eistec.se>
 *
 * @}
 */

SECTIONS
{
    /* Interrupt vectors 0x00-0x3ff. */
    .vectors :
    {
        _isr_vectors = .;
        KEEP(*(.vectors .vectors.*))
    } > vectors
    ASSERT (SIZEOF(.vectors) == 0x400, "Interrupt vector table of invalid size.")
    ASSERT (ADDR(.vectors) == 0x00000000, "Interrupt vector table at invalid location (linker-script error?)")
    ASSERT (LOADADDR(.vectors) == 0x00000000, "Interrupt vector table at invalid location (linker-script error?)")

    /* Flash configuration field, very important in order to not accidentally lock the device */
    /* Flash configuration field 0x400-0x40f. */
    .fcfield :
    {
        . = ALIGN(4);
        KEEP(*(.fcfield))
        . = ALIGN(4);
    } > flashsec
    ASSERT (SIZEOF(.fcfield) == 0x10, "Flash configuration field of invalid size (linker-script error?)")
    ASSERT (ADDR(.fcfield) == 0x400, "Flash configuration field at invalid position (linker-script error?)")
    ASSERT (LOADADDR(.fcfield) == 0x400, "Flash configuration field at invalid position (linker-script error?)")
}

INCLUDE cortexm_base.ld
