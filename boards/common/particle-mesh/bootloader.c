/*
 * SPDX-FileCopyrightText: 2014-2016 Freie Universität Berlin
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @{
 *
 * @file
 * @author  Christian Amsüss <chrysn@fsfe.org>
 */

#ifdef PARTICLE_MONOFIRMWARE
#include <stdint.h>
#include "vectors_cortexm.h"

extern const void *_isr_vectors;

/* As described in Particle's dynalib/inc/module_info.h (there under LGPL-3
 * terms; detailed license compatibility discussion skipped for not crossing
 * any threshold of originality) */
typedef struct module_info_t {
    const void* module_start_address;
    const void* module_end_address;
    uint8_t reserved;
    uint8_t flags;
    uint16_t module_version;
    uint16_t platform_id;
    uint8_t  module_function;
    uint8_t  module_index;
    uint32_t dependency; /* was module_dependency_t; kept in here to ensure it's blank */
    uint32_t dependency2; /* was module_dependency_t */
} module_info_t;

/* 64 words is the distance between the known ISR table and the fixed-position
 * module_info at 0x200. If the ISR length is changed for any reason, this must
 * be adapted until particle_monofirmware_module_info is precisely at 0x30200.
 * (monofirmware-tool.py will complain if it isn't).
 *
 * This is not `static` as *something* from this module needs to be visible to
 * the outside for the __attribute__((used)) and the KEEP on the section in the
 * linker script to work; see also the reference to it in board.c. */
ISR_VECTOR(50) const uint32_t particle_monofirmware_padding[64] = {0, };

#ifdef PARTICLE_MONOFIRMWARE_CHECKSUMLIMIT
/* Watch the ISR vector sequence here: This is placed *after* the module_info
 * in the flash (52 > 51), but placed before it in the code to be referenceable
 * from there.
 *
 * The actual value is replaced by monofirmware-tool.py.
 * */
ISR_VECTOR(52) static const uint8_t particle_monofirmware_checksum[4] = {0, 0, 0, 0};
#endif

ISR_VECTOR(51) static const struct module_info_t particle_monofirmware_module_info = {
    .module_start_address = &_isr_vectors,
#ifdef PARTICLE_MONOFIRMWARE_CHECKSUMLIMIT
    .module_end_address = &particle_monofirmware_checksum,
    /* else, it is set by the monofirmware-tool.
     *
     * For that (more common) case where we check the full firmware, we would
     * want to set set
     *
     * .module_end_address = &_etext + (&_erelocate - &_srelocate),
     *
     * but that can not be determined at compile time. Instead, this will be
     * populated by monofirmware-tool.py, which needs to write in here anyway
     * for the checksum. */
#endif
    .module_version = 0x138, /* module version that micropython uses */

    .platform_id = PARTICLE_PLATFORM_ID,

    .module_index = 3, /* MOD_FUNC_MONO_FIRMWARE from dynalib/inc/module_info.h */
};

#else
typedef int dont_be_pedantic;
#endif

/** @} */
