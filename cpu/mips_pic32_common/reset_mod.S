/*
 * Copyright 2014-2015, Imagination Technologies Limited and/or its
 *                      affiliated group companies.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 * this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 * this list of conditions and the following disclaimer in the documentation
 * and/or other materials provided with the distribution.
 * 3. Neither the name of the copyright holder nor the names of its
 * contributors may be used to endorse or promote products derived from this
 * software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
*/

/* ************ PLEASE READ ME !!!!  ****************

	This file is a copy of the reset_mod.S from $MIPS_ELF_ROOT/share/mips/boot
	(from the 2016.05-03 version) with a couple of modifications:

	#define SKIP_COPY_TO_RAM - prevents the bootloader copying the whole contents
	of flash to ram (as we want to XIP from flash), we copy initialized data from
	flash to ram in 'software_init_hook'.

	move .org's to before the labels to make the vector labels appear at the vector
	addresses.

	In boot_debug_exception vector drop out of debug mode before spining, this allows
	attachment of an external debug program to investigate a hung system.

	Future toolchain versions will have these changes included and this file will
	be no longer needed.

	Note the above copyright/license is 3 Clause BSD and as such is compatible with LGPLv2.1
	as such we grant licensing this file under LGPLv2.1 (See the file LICENSE in the top level
	directory for more details) as well.

	Thanks for reading.
*/



#define _RESETCODE
.set nomips16

#include <mips/regdef.h>
#include <mips/cpu.h>
#include <mips/asm.h>

    .set push
    .set nomicromips
LEAF(__reset_vector)
    lui	  a2, %hi(__cpu_init)
    addiu a2, %lo(__cpu_init)
    mtc0  $0, C0_COUNT		  # Clear cp0 Count (Used to measure boot time.)
    jr    a2
    .space 32			  # Just to cope with a quirk of MIPS malta boards
				  # this can be deleted for anything else.
END(__reset_vector)
    .set pop

LEAF(__cpu_init)

    # Verify the code is here due to a reset and not NMI. If this is an NMI then trigger
    # a debugger breakpoint using a sdbp instruction.

    mfc0  s1, C0_STATUS		  # Read CP0 Status
    ext	  s1, s1, SR_NMI_SHIFT, 1 # extract NMI
    beqz  s1, init_resources     # /* Branch if this is NOT an NMI exception. */
    move  k0, t9		  # Preserve t9
    move  k1, a0		  # Preserve a0
    li	  $25, 15		  # UHI exception operation
    li	  $4, 0			  # Use hard register context
    sdbbp 1			  # Invoke UHI operation

init_resources:

    # Init CP0 Status, Count, Compare, Watch*, and Cause.
    jal	  __init_cp0

    # Initialise L2/L3 cache
    # This could be done from cached code if there is a cca override or similar

    # Determine L2/L3 cache config.

    lui   a2, %hi(__init_l23cache)
    addiu a2, a2, %lo(__init_l23cache)
    jal	  a2

init_ic:
    # Initialize the L1 instruction cache.
    jal	  __init_icache

    # The changing of Kernel mode cacheability must be done from KSEG1
    # Since the code is executing from KSEG0 It needs to do a jump to KSEG1 change K0
    # and jump back to KSEG0

    lui	  a2, %hi(__change_k0_cca)
    addiu a2, a2, %lo(__change_k0_cca)
    li	  a1, 0xf
    ins	  a2, a1, 29, 1		      # changed to KSEG1 address by setting bit 29
    jalr  a2

    .weak __init_l23cache_cached
    lui   a2, %hi(__init_l23cache_cached)
    addiu a2, a2, %lo(__init_l23cache_cached)
    beqz  a2, init_dc
    jal   a2

init_dc:
    # Initialize the L1 data cache
    jal	  __init_dcache

    # Initialize the TLB.
    jal	  __init_tlb

    # Allow everything else to be initialized via a hook.
    .weak __boot_init_hook
    lui	  a2, %hi(__boot_init_hook)
    addiu a2, a2, %lo(__boot_init_hook)
    beqz  a2, 1f
    jalr  a2
1:

#ifndef SKIP_COPY_TO_RAM

    # Copy code and data to RAM
    li    s1, 0xffffffff

    # Copy code and read-only/initialized data from FLASH to (uncached) RAM.
    lui   a1, %hi(__flash_app_start)
    addiu a1, a1, %lo(__flash_app_start)
    ins   a1, s1, 29, 1			# Make it uncached (kseg1)
    lui   a2, %hi(__app_start)
    addiu a2, a2, %lo(__app_start)
    ins   a2, s1, 29, 1			# Make it uncached (kseg1)
    lui   a3, %hi(_edata)
    addiu a3, a3, %lo(_edata)
    ins   a3, s1, 29, 1			# Make it uncached (kseg1)
    beq   a2, a3, $Lcopy_to_ram_done
$Lnext_ram_word:
    lw    a0, 0(a1)
    sw    a0, 0(a2)
    addiu a2, a2, 4
    addiu a1, a1, 4
    bne   a3, a2, $Lnext_ram_word
$Lcopy_to_ram_done:

#endif

    # Prepare for eret to _start
    lui   ra, %hi($Lall_done)		# If main returns then go to all_done.
    addiu ra, ra, %lo($Lall_done)
    lui   v0, %hi(_start)		# Load the address of _start
    addiu v0, v0, %lo(_start)
    mtc0  v0, C0_ERRPC			# Set ErrorEPC to _start
    ehb				# Clear hazards (makes sure write to ErrorPC has completed)
    li	  a0, 0				# UHI compliant null argument setup

    # Return from exception will now execute the application startup code
    eret

$Lall_done:
    # If _start returns it will return to this point.
    # Just spin here reporting the exit.
    li	    $25, 1			# UHI exit operation
    move    $4, v0			# /* Collect exit code for UHI exit */
    sdbbp   1				# Invoke UHI operation
    b       $Lall_done
END(__cpu_init)

/**************************************************************************************
    B O O T   E X C E P T I O N   H A N D L E R S (CP0 Status[BEV] = 1)
**************************************************************************************/
/* NOTE: the linker script must insure that this code starts at start + 0x200 so the exception */
/* vectors will be addressed properly. All .org assume this! */
/* TLB refill, 32 bit task. */
.org 0x200				# TLB refill, 32 bit task.
LEAF(__boot_tlb_refill)
    move  k0, t9			# Preserve t9
    move  k1, a0			# Preserve a0
    li	  $25, 15			# UHI exception operation
    li	  $4, 0				# Use hard register context
    sdbbp 1				# Invoke UHI operation
END(__boot_tlb_refill)

.org 0x280				# XTLB refill, 64 bit task. BEV + 0x280
LEAF(__boot_xtlb_refill)
    move  k0, t9			# Preserve t9
    move  k1, a0			# Preserve a0
    li	  $25, 15			# UHI exception operation
    li	  $4, 0				# Use hard register context
    sdbbp 1				# Invoke UHI operation
END(__boot_xtlb_refill)

.org 0x300				# Cache error exception. BEV + 0x300
LEAF(__boot_cache_error)
    move  k0, t9			# Preserve t9
    move  k1, a0			# Preserve a0
    li	  $25, 15			# UHI exception operation
    li	  $4, 0				# Use hard register context
    sdbbp 1				# Invoke UHI operation
END(__boot_cache_error)

.org 0x380				# General exception. BEV + 0x380
LEAF(__boot_general_exception)
    move  k0, t9			# Preserve t9
    move  k1, a0			# Preserve a0
    li	  $25, 15			# UHI exception operation
    li	  $4, 0				# Use hard register context
    sdbbp 1				# Invoke UHI operation
END(__boot_general_exception)

# If you want the above code to fit into 1k flash you will need to leave
# out the code below. This is the code that covers the debug exception
# which you normally will not get.
.org 0x480
LEAF(__boot_debug_exception)
	# EJTAG Debug (with ProbEn = 0 in the EJTAG Control Register)
    mfc0 k1, C0_DEPC	# Save Debug exception point in DESAVE
    mtc0 k1, C0_DESAVE
    LA k1, 1f
	# Drop out of debug mode before spinning (To allow a JTAG probe in).
    mtc0 k1, C0_DEPC
    ehb
    deret
1:
    b 1b			 	#Spin indefinitely
END(__boot_debug_exception)
