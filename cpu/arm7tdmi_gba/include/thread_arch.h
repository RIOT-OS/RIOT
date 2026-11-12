/*
 * SPDX-FileCopyrightText: 2026 HAW Hamburg
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @ingroup     cpu_arm7_common
 * @{
 *
 * @file
 * @brief       Implementation of the kernels thread interface
 */

#include "irq.h"

#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif

#define THREAD_API_INLINED

#ifndef DOXYGEN /* Doxygen is in core/include/thread.h */

/*
 * Thanks to Martin Korth for providing this info in his GBATEK.
 * The GBA has a BIOS that implements syscall functions.
 * Called by using `svc $num` (THUMB) or `svc $num*0x10000` (ARM)
 * 00h SoftReset        10h BitUnPack                      20h SoundWhatever0
 * 01h RegisterRamReset 11h LZ77UnCompReadNormalWrite8bit  21h SoundWhatever1
 * 02h Halt             12h LZ77UnCompReadNormalWrite16bit 22h SoundWhatever2
 * 03h Stop/Sleep       13h HuffUnCompReadNormal           23h SoundWhatever3
 * 04h IntrWait         14h RLUnCompReadNormalWrite8bit    24h SoundWhatever4
 * 05h VBlankIntrWait   15h RLUnCompReadNormalWrite16bit   25h MultiBoot
 * 06h Div              16h Diff8bitUnFilterWrite8bit      26h HardReset
 * 07h DivArm           17h Diff8bitUnFilterWrite16bit     27h CustomHalt
 * 08h Sqrt             18h Diff16bitUnFilter              28h SoundDriverVSyncOff
 * 09h ArcTan           19h SoundBias                      29h SoundDriverVSyncOn
 * 0Ah ArcTan2          1Ah SoundDriverInit                2Ah SoundGetJumpList
 * 0Bh CpuSet           1Bh SoundDriverMode                2Bh+ Crash / Undefined behavior
 * 0Ch CpuFastSet       1Ch SoundDriverMain                D4h Garbage, jumps to 0x2002040 (EWRAM)
 * 0Dh GetBiosChecksum  1Dh SoundDriverVSync
 * 0Eh BgAffineSet      1Eh SoundChannelClear
 * 0Fh ObjAffineSet     1Fh MidiKey2Freq
 *
 * We are particularly interested in the invalid syscall `D4h`. The UB of the BIOS messes
 * a bit with our regs and then jumps to 0x2002040 in EWRAM. This UB is predictable and
 * deterministic. By placing our own code at that address during boot (see `setup_swi:`
 * in startup.s) we can gain access over the flow of control during a SWI. From there
 * its the normal RIOT SWI handling.
 * This hack was found by the uClinux people back in 2004, thanks for documenting!
 */
static inline __attribute__((always_inline)) void thread_yield_higher(void)
{
    if (irq_is_in()) {
        sched_context_switch_request = 1;
    }
    else {
        /* The GBA cpu has a bug where SWIs don't cause it to disable IRQs.
         * So we are doing that instead. */
        unsigned irq_state = irq_disable();
        /* Abusing an "illegal" BIOS syscall */
        __asm__("svc 0xd40000\n");
        /* restore irq */
        irq_restore(irq_state);
    }
}

#endif /* DOXYGEN */

#ifdef __cplusplus
}
#endif

/** @} */
